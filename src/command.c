#include "command.h"
#include "serial.h"
#include "trackdisk.h"

#include <devices/serial.h>
#include <proto/exec.h>
#include <string.h>
#include <proto/dos.h>

#include <stdlib.h>
#include <stdio.h>

char __chip buffer[TRACK_SIZE + 1];

/*
void write_error(error_t code)
{
    sercom_write_block((const void *) &code, sizeof(code));
    if(code != RESULT_OK)
        write_string(errstr);
}
*/


error_t read_string_into_buffer(int32_t *size_ret) {
    int32_t size = 0, read;
    error_t error = { RESULT_OK, 0 };

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


void cmd_handle_message() {
    int32_t size, bytes_read, to_read;
    error_t error = { RESULT_OK, 0 };

    error = ser_read_int32(&size);
    if(FAILED(error)) {
        on_error(error, "cmd_handle_message: ser_read_input_size failed", 1);
    }

    /* printf("handle message size: %d (0x%x)\n", size, size); */

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

void cmd_handle_put_file(void) {
    int fh;
    int32_t size, cur, to_read, bytes_read;
    error_t error;

    printf("handle_put_file\n");
    error = read_string_into_buffer((int32_t *)0);
    if(FAILED(error)) {
        on_error(error, "cmd_handle_put_file: read_string_into_buffer failed", 1);
    }
    printf(buffer);
    printf("\n");
    /* fh = _dcreatx(buffer, 0); */

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
        /* ser_start(); */
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

void cmd_handle_examine(void)
{
    error_t error = { RESULT_OK, 0 };
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

void cmd_handle_list_dir(void)
{
    error_t error = { RESULT_OK, 0 };
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

