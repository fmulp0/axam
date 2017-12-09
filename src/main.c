
#include <stdio.h>
#include "serial.h"
#include "command.h"
#include "error.h"
#include "config.h"

error_t axam_init(void) {
    error_t error;

    error = ser_init();
    if(error) {
        return error;
    }

    return RESULT_OK;
}

void axam_shutdown(void) {
    ser_shutdown();
}

void axam_main_loop(void) {
    cmd_t cmd = 0;
    error_t error = RESULT_OK;

    do {
        error = ser_read_command(&cmd);
        if(error) {
            printf("main loop error reading command: %08x\n", error);

            return;
        }
        printf("read_command: %d\n", cmd);

        if(cmd == CMD_EXIT) {
            return;
        }
        switch(cmd) {
            case CMD_MESSAGE:
                cmd_handle_message();
                break;
            case CMD_PUT_FILE:
                cmd_handle_put_file();
                break;
            case CMD_PUT_ADF:
                cmd_handle_put_adf();
                break;
        }
    } while(1);
}

int main(void) {
    error_t error;

    printf("AXAM version: %d.%d.%d\n", MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION);

    error = axam_init();
    if(error) {
        return 1;
    }

    printf("initialization successfull\n");

    axam_main_loop();

    axam_shutdown();
}
