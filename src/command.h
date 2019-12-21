#ifndef _AXAM_COMMAND_H_
#define _AXAM_COMMAND_H_

#include "types.h"
#include "error.h"

#define CMD_EXIT                        0x01    /* Exit axam */
#define CMD_ABORT                       0x02    /* Abort current operation */
#define CMD_MESSAGE                     0x03    /* print a message */
#define CMD_PUT_FILE                    0x04    /* Send a file */
#define CMD_PUT_ADF                     0x05    /* Send .adf-image to device */
#define CMD_LIST_DIR                    0x06    /* List contents of current working dir */
#define CMD_EXAMINE                     0x07    /* Gets information of current dir/file */
#define CMD_GET_FILE                    0x08    /* receive file */

extern void cmd_handle_message(void);
extern void cmd_handle_put_file(void);
extern void cmd_handle_put_adf(void);
extern void cmd_handle_examine(void);
extern void cmd_handle_list_dir(void);

#endif
