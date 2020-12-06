#ifndef _AXAM_COMMAND_H_
#define _AXAM_COMMAND_H_

#include "types.h"
#include "error.h"

typedef enum cmd {
    CMD_EXIT        = 0x01,     /* exit axam */
    CMD_ABORT       = 0x02,     /* abort current operation */
    CMD_MESSAGE     = 0x03,     /* print a message */
    CMD_PUT_FILE    = 0x04,     /* send a file */
    CMD_PUT_ADF     = 0x05,     /* send .adf-image to device */
    CMD_LIST_DIR    = 0x06,     /* list contents of current working dir */
    CMD_EXAMINE     = 0x07,     /* get information of dir/file */
    CMD_GET_FILE    = 0x08,     /* receive file */
    CMD_GET_ADF     = 0x09,     /* receive .adf-image from device */
    CMD_SET_CWD     = 0x0a,     /* set current working dir */
    CMD_GET_CWD     = 0x0b,     /* get current working dir */
    CMD_PING        = 0x0c      /* check if the other side is still alive */
} cmd_t;

extern error_t cmd_check(cmd_t command);

extern error_t cmd_handle_message(void);
/*
extern void cmd_handle_put_file(void);
extern void cmd_handle_put_adf(void);
extern void cmd_handle_examine(void);
extern void cmd_handle_list_dir(void);
*/

extern error_t cmd_get(uint8_t *data, int32_t size, cmd_t *cmd_ret);

#endif
