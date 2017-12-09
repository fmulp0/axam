#include "command.h"
#include "serial.h"
#include "trackdisk.h"

#include <devices/serial.h>
#include <stdlib.h>
#include <stdio.h>
#include <exec/exec.h>
#include <proto/exec.h>
#include <dos.h>
#include <string.h>
#include <proto/dos.h>

char __chip buffer[TRACK_SIZE + 1];

void write_string(const char *s) {
    size_t written, len = strlen(s);
    error_t error;
    if(len == 0)
        return;

    error = ser_write_block((const void *) &len, sizeof(len), &written);
    if (error) {
        printf("write_string: write length failed: %08x\n", error);

        return;
    }

    error = ser_write_block((const void *) s, len, &written);
    if (error) {
        printf("write_string: write data failed: %08x\n", error);
    }
}

/*
void write_error(error_t code)
{
    sercom_write_block((const void *) &code, sizeof(code));
    if(code != RESULT_OK)
        write_string(errstr);
}
*/


error_t read_string_into_buffer(size_t *size_ret) {
    size_t size = 0, read;
    error_t error = RESULT_OK;

    error = ser_read_input_size(&size);
    if (error) {
        printf("read_string_into_buffer: ser_read_block failed: %08x\n", error);

        return error;
    }
    if(size > 511)
        size = 511;
    error = ser_read_block((void *) buffer, size, &read);
    if(error) {
        printf("read_string_into_buffer: ser_read_block failed: %08x\n", error);

        return error;
    }
    buffer[read] = 0;
    if(size_ret)
        *size_ret = read;

    return error;
}


error_t cmd_handle_message() {
    size_t size, bytes_read, to_read;
    error_t error;

    error = ser_read_input_size(&size);
    if(error) {
        printf("cmd_handle_message: ser_read_input_size failed: %08x", error);

        return error;
    }
    printf("handle message size: %d (0x%x)\n", size, size);

    while(size > 0) {
        if(size > 512)
            to_read = 512;
        else
            to_read = size;

        error = ser_read_block(buffer, to_read, &bytes_read);
        if(error) {
            printf("cmd_handle_message: ser_read_input_size failed: %08x\n", error);

            return error;
        }
        buffer[bytes_read] = 0;
        printf(buffer);
        size -= bytes_read;
        error = ser_write_byte(0, NULL);
        if(error) {
            printf("td_write_sector cannot send status: %0xd\n", error);

            return error;
        }

        error = ser_flush();
        if(error) {
            printf("td_write_sector cannot flush: %0xd\n", error);

            return error;
        }
    }

    printf("\n");

    return RESULT_OK;
}

error_t cmd_handle_put_file(void) {
    int fh;
    size_t size, cur, to_read, bytes_read;
    error_t error;

    printf("handle_put_file");
    error = read_string_into_buffer(NULL);
    if(error) {
        printf("cmd_handle_put_file: read_string_into_buffer failed: %08x\n", error);
    }
    printf(buffer);
    /* fh = _dcreatx(buffer, 0); */

    fh = Open(buffer, MODE_NEWFILE);
    if(fh < 0) {
        error = MAKE_ERROR(FILESYSTEM, ORIGIN_AX, ERR_EXISTS);
        printf("cmd_handle_put_file: Open failed (file already exists): %08x\n", error);

        return error;
    }

    puts("file successfully created");

    error = ser_read_input_size(&size);
    if(error) {
        printf("cmd_handle_put_file: ser_read_input_size failed: %08x\n", error);

        return error;
    }
    cur = 0;
    to_read = 1024;
    while(cur < size) {
        if(size - cur < 1024) {
            to_read = size - cur;
        }

        error = ser_read_block(buffer, to_read, &bytes_read);
        if(error) {
            printf("cmd_handle_put_file: ser_read_block failed: %08x\n", error);
            _dclose(fh);

            return error;
        }
        _dwrite(fh, buffer, bytes_read);

        cur += bytes_read;
        /* ser_start(); */
        error = ser_write_byte(0, NULL);
        if(error) {
            printf("td_write_sector cannot send status: %0xd\n", error);
            _dclose(fh);

            return error;
        }

        error = ser_flush();
        if(error) {
            printf("td_write_sector cannot flush: %0xd\n", error);
            _dclose(fh);

            return error;
        }
    }

    _dclose(fh);

    return RESULT_OK;
}

error_t cmd_handle_put_adf(void) {
    size_t cur_track, rres, wres;
    cmd_t drive;
    error_t error;

    printf("handle_put_adf\n");


    error = ser_read_command(&drive);
    if(error) {
        printf("cmd_handle_put_adf read of drive number failed: %08x\n", error);

        return error;
    }

    error = td_init(drive);
    if(error) {
        printf("cmd_handle_put_adf trackdisk init failed: %08x\n", error);

        return error;
    }


    cur_track = 0;

    while(cur_track < 160) {

        error = ser_read_block(buffer, TRACK_SIZE, &rres);
        if(error) {
            printf("ser_read_block returned error: %0xd\n", error);
            td_shutdown();

            return error;
        }

        error = td_write_sector(buffer, cur_track, &wres);
        if(error) {
            printf("td_write_sector returned error %0xd\n", error);
            td_shutdown();

            return error;
        }

        cur_track++;
        printf("track %d successfully written\n", cur_track);

        error = ser_write_byte(0, NULL);
        if(error) {
            printf("td_write_sector cannot send status: %0xd\n", error);
            td_shutdown();

            return error;
        }

        error = ser_flush();
        if(error) {
            printf("td_write_sector cannot flush: %0xd\n", error);
            td_shutdown();

            return error;
        }
    }

    td_shutdown();
}
