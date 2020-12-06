#include <stdlib.h>
#include <stdio.h>
#include "serial.h"
#include "command.h"
#include "error.h"
#include "config.h"
#include "data.h"


/* pointer returned by ser_read_packet */
uint8_t *axam_buffer = NULL;
/* size returned by ser_read_packet */
int32_t size;
/* the last command received */
cmd_t command;

bool_t should_exit = false;

error_t axam_init(void) {
    error_t error;

    error = ser_init();

    return error;
}

void axam_shutdown(void) 
{
    ser_shutdown();
}

/*
void axam_main_loop(void) {
    cmd_t cmd = 0;
    error_t error = { ERR_OK, 0 };

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
*/

error_t axam_get_command(void)
{
    error_t error = ERR_OK;

    error = ser_read_data(&axam_buffer, &size);
    if(error = ERR_OK)
    {
        if(size != sizeof(cmd_t)) 
        {
            error = ERR_READ_FAILED;
            printf("unexpected packet size in read command\n");
        }
    }

    error = cmd_get(axam_buffer, size, &command);

    return error;
}

error_t axam_handle_command(cmd_t cmd)
{
    error_t error = ERR_OK;

    switch(cmd)
    {
        case CMD_EXIT:
            should_exit = true;
            error = ERR_OK;
            break;
        case CMD_MESSAGE:
            error = cmd_handle_message();
            break;
        default:
            error = ERR_INVALID_ARG;
            break;
    }

    return error;
}

error_t axam_main_loop(void)
{
    error_t error = ERR_OK;

    do {
        error = axam_get_command();
        if(error == ERR_OK)
        {
            error = cmd_check(command);
        }

        if(error = ERR_OK)
        {
            error = axam_handle_command(command);
        }
    } 
    while(error == ERR_OK && should_exit == false);

    return error;
}

int main(void) 
{
    error_t error = ERR_OK;


    printf("AXAM version: %d.%d.%d\n", MAJOR_VERSION, MINOR_VERSION, PATCH_VERSION);

    error = axam_init();
    if(error == ERR_OK)
    {
        drain_serial();
        error = axam_main_loop();
    }

    if(error = ERR_OK)
    {
        printf("init ok\n");    
    }

    axam_shutdown();
}
