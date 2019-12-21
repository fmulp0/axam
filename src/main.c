#include <stdlib.h>
#include <stdio.h>
#include "serial.h"
#include "command.h"
#include "error.h"
#include "config.h"

error_t axam_init(void) {
    error_t error;

    error = ser_init();

    return error;
}

void axam_shutdown(void) {
    ser_shutdown();
}

void axam_main_loop(void) {
    cmd_t cmd = 0;
    error_t error = { RESULT_OK, 0 };

    do {
        error = ser_read_command(&cmd);
        if(FAILED(error)) {
            on_error(error, "main loop error reading command", 1);
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
            case CMD_EXAMINE:
                cmd_handle_examine();
                break;
            case CMD_LIST_DIR:
                cmd_handle_list_dir();
                break;
        }
    } while(1);
}

int main(void) {
    printf("AXAM version: %d.%d.%d\n", MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION);

    axam_init();
    drain_serial();
    printf("initialization successfull\n");
    axam_main_loop();
    axam_shutdown();
}
