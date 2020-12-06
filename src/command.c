#include "buffer.h"
#include "command.h"
#include "serial.h"
#include "trackdisk.h"
#include "console.h"
#include "file.h"
#include "copy.h"
#include "data.h"

#include <devices/serial.h>
#include <proto/exec.h>
#include <string.h>
#include <proto/dos.h>

#include <stdlib.h>
#include <stdio.h>


error_t cmd_get(uint8_t *data, int32_t size, cmd_t *cmd_ret)
{
    error_t error = ERR_OK;

    if(data == NULL || cmd_ret == NULL)
    {
        error = ERR_NULLPOINTER;
    }

    if(error == ERR_OK)
    {
        if(size != sizeof(cmd_t))
        {
            error = ERR_READ_FAILED;
        }
    }

    if(error == ERR_OK)
    {
        *cmd_ret = *(cmd_t *) data;
    }

    return error;
}


/*
error_t read_string_into_buffer(int32_t *size_ret) {
    int32_t size = 0, read;
    error_t error = ERR_OK;

    error = ser_read_int32(&size);
    if (FAILED(error)) {
        on_error(error, "read_string_into_buffer: ser_read_int32 failed", 0);
    }
    if(size > 511)
        size = 511;
    error = ser_read_block((void *) buffer, size, &read);
    if(FAILED(error)) {
        on_error(error, "read_string_into_buffer: ser_read_block failed", 0);
    }

    buffer[read] = 0;
    if(size_ret)
        *size_ret = read;

    return error;
}
*/

error_t cmd_check(cmd_t command)
{
    error_t error = ERR_OK;
    switch(command)
    {
        case CMD_EXIT:
        case CMD_MESSAGE:
            break;
        default:
            error = ERR_INVALID_ARG;
            break;
    }

    return error;
}

error_t cmd_handle_message(void) 
{
    int32_t size, bytes_read, to_write, bytes_written;
    error_t error = ERR_OK;
    uint8_t *data;

    error = data_stream_packet_read(buffer, ser_reader, &size);
    if(error == ERR_OK)
    {
        to_write = size;
        do
        {
            error = console_writer((const void *)buffer, size, &bytes_written);
            to_write -= bytes_written;
        }
        while(error == ERR_OK && to_write > 0);
    }

    return error;
}


/*
    while(size > 0) {
        if(size > 512)
            to_read = 512;
        else
            to_read = size;

        error = ser_read_block(buffer, to_read, &bytes_read);
        if(FAILED(error)) {
            on_error(error, "cmd_handle_message: ser_read_input_size failed", 1);
        }
        buffer[bytes_read] = 0;
        printf(buffer);
        size -= bytes_read;
        error = ser_write_error(error);
        if(FAILED(error)) {
            on_error(error, "ser_write_error cannot send status", 1);
        }

        error = ser_flush();
        if(FAILED(error)) {
            on_error(error, "ser_flush cannot flush", 1);
        }
    }
}
*/
/*
void cmd_handle_put_file(void) {
    int fh;
    int32_t size, cur, to_read, bytes_read, bytes_written;
    error_t error;

    printf("handle_put_file\n");
    error = read_string_into_buffer((int32_t *)0);
    if(FAILED(error)) {
        on_error(error, "cmd_handle_put_file: read_string_into_buffer failed", 1);
    }
    printf(buffer);
    printf("\n");
    error = ser_read_int32(&size);
    if(FAILED(error)) {
        on_error(error, "cmd_handle_put_file: ser_read_input_size failed", 1);
    }

    error = file_create_new(buffer);
    if(FAILED(error))
    {
        on_error(error, "cmd_handle_put_file: file_create_new failed", 1);
    }

    error = copy(ser_read_block, file_writer, size, &bytes_written);
    file_close();
}
*/
/*
    fh = Open(buffer, MODE_NEWFILE);
    if(fh < 0) {
        MAKE_ERROR(error, FILESYSTEM, ORIGIN_AX, ERR_EXISTS);
        on_error(error, "cmd_handle_put_file: Open failed (file already exists)", 1);
    }

    puts("file successfully created\n");

    error = ser_read_int32(&size);
    if(FAILED(error)) {
        on_error(error, "cmd_handle_put_file: ser_read_input_size failed", 1);
    }
    cur = 0;
    to_read = 1024;
    while(cur < size) {
        if(size - cur < 1024) {
            to_read = size - cur;
        }

        error = ser_read_block(buffer, to_read, &bytes_read);
        if(FAILED(error)) {
            on_error(error, "cmd_handle_put_file: ser_read_block failed", 0);
            Close(fh);
            exit(1);
        }
        Write(fh, buffer, bytes_read);

        cur += bytes_read;
        error = ser_write_error(error);
        if(FAILED(error)) {
            on_error(error, "td_write_sector cannot send status", 0);
            Close(fh);
            exit(1);
        }

        error = ser_flush();
        if(FAILED(error)) {
            on_error(error, "td_write_sector cannot flush", 0);
            Close(fh);
            exit(1);
        }

        printf("%d\n", cur);
    }

    Close(fh);
}
*/
/*
void cmd_handle_put_adf(void) {
    int32_t cur_track, rres, wres;
    cmd_t drive;
    error_t error;

    printf("handle_put_adf\n");


    error = ser_read_command(&drive);
    if(FAILED(error)) {
        on_error(error, "cmd_handle_put_adf read of drive number failed", 1);
    }

    error = td_init(drive);
    if(FAILED(error)) {
        on_error(error, "cmd_handle_put_adf trackdisk init failed", 1);
    }

    cur_track = 0;

    while(cur_track < 160) {
        error = ser_read_block(buffer, TRACK_SIZE, &rres);
        if(FAILED(error)) {
            on_error(error, "ser_read_block returned error", 0);
            td_shutdown();
            exit(1);
        }

        error = td_format_sector(buffer, cur_track);
        if(FAILED(error)) {
            on_error(error, "td_format_sector returned error", 0);
            td_shutdown();
            exit(1);
        }

        error = td_write_sector(buffer, cur_track, &wres);
        if(FAILED(error)) {
            on_error(error, "td_write_sector returned error", 0);
            td_shutdown();
            exit(1);
        }

        cur_track++;
        printf("track %d successfully written\n", cur_track);

        error = ser_write_error(error);
        if(FAILED(error)) {
            on_error(error, "td_write_sector cannot send status", 0);
            td_shutdown();
            exit(1);
        }

        error = ser_flush();
        if(FAILED(error)) {
            on_error(error, "td_write_sector cannot flush", 0);
            td_shutdown();
            exit(1);
        }
    }

    td_shutdown();
}
*/
/*
void cmd_handle_examine(void)
{
    error_t error = { ERR_OK, 0 };
    BPTR lock = 0;
    struct FileInfoBlock file_info_block;

    error = read_string_into_buffer(0);
    if(FAILED(error)) {
        on_error(error, "cmd_handle_examine: failed to read string", 1);
    }

    lock = Lock(buffer, ACCESS_READ);
    if(!lock) {
        MAKE_ERROR(error, FILESYSTEM, ORIGIN_AX, ERR_LOCK);
        on_error(error, "cmd_handle_examine: failed to lock", 1);
    }

    if(!Examine(lock, &file_info_block))  {
        MAKE_ERROR(error, FILESYSTEM, ORIGIN_AX, ERR_EXAMINE);
        on_error(error, "cmd_handle_examine: failed to examine", 1);
    }

    error = ser_write_error(error);
    if(FAILED(error)) {
        on_error(error, "cmd_handle_examine: failed to send status", 0);
        UnLock(lock);
        exit(1);
    }

    error = ser_flush();
    if(FAILED(error)) {
        on_error(error, "cmd_handle_examine: failed to flush serial (status)", 0);
        UnLock(lock);
        exit(1);
    }

    error = ser_write_block(&file_info_block, sizeof(file_info_block), NULL);
    UnLock(lock);
    if(FAILED(error)) {
        on_error(error, "cmd_handle_examine: failed to send results", 0);
    }

    error = ser_flush();
    if(FAILED(error)) {
        on_error(error, "cmd_handle_examine: failed to flush serial (results)", 0);
        UnLock(lock);
        exit(1);
    }
}
*/
/*
void cmd_handle_list_dir(void)
{
    error_t error = { ERR_OK, 0 };
    LONG io_err = 0;
    BPTR lock = 0;
    struct FileInfoBlock file_info_block;

    error = read_string_into_buffer(0);
    if(FAILED(error)) {
        on_error(error, "cmd_handle_list_dir: failed to read string", 1);
    }

    lock = Lock(buffer, ACCESS_READ);
    if(!lock) {
        MAKE_ERROR(error, FILESYSTEM, ORIGIN_AX, ERR_LOCK);
        on_error(error, "cmd_handle_list_dir: failed to lock buffer", 1);
    }

    if(!Examine(lock, &file_info_block))  {
        MAKE_ERROR(error, FILESYSTEM, ORIGIN_AX, ERR_EXAMINE);
        on_error(error, "cmd_handle_list_dir: failed to examine", 1);
    }

    if(file_info_block.fib_DirEntryType <= 0) {
        MAKE_ERROR(error, FILESYSTEM, ORIGIN_AX, ERR_EXAMINE);
        on_error(error, "cmd_handle_list_dir: not a directory", 0);
        return;
    }
    do {
        error = ser_write_error(error);
        if(FAILED(error)) {
            UnLock(lock);
            on_error(error, "cmd_handle_list_dir: failed to send status", 0);
        }

        error = ser_flush();
        if(FAILED(error)) {
            UnLock(lock);
            on_error(error, "cmd_handle_list_dir: failed to flush (status)", 0);
            return;
        }

        error = ser_write_block(&file_info_block, sizeof(file_info_block), NULL);
        if(FAILED(error)) {
            UnLock(lock);
            on_error(error, "cmd_handle_list_dir: failed to write results", 1);
        }

        error = ser_flush();
        if(FAILED(error)) {
            UnLock(lock);
            on_error(error, "cmd_handle_list_dir: failed to flush (results)", 0);
            return;
        }

        if(!ExNext(lock, &file_info_block)) {
            io_err = IoErr();
            if(io_err == ERROR_NO_MORE_ENTRIES) {
                break;
            }
        }
    } while(1);

    if(io_err != ERROR_NO_MORE_ENTRIES) {
        MAKE_ERROR(error, io_err, ORIGIN_OS, FILESYSTEM);
        on_error(error, "there was an error while reading dir", 1);
    }

    UnLock(lock);
}

*/
