#ifndef _AXAM_COMMAND_H_
#define _AXAM_COMMAND_H_

#include "types.h"
#include "error.h"

#define CMD_EXIT                        1   /* Exit axam */
#define CMD_ABORT                       2   /* Abort current operation */
#define CMD_MESSAGE                     3   /* print a message */
#define CMD_PUT_FILE                    4   /* Send a file */
#define CMD_PUT_ADF                     5   /* Send .adf-image to device */
#define CMD_PWD                         6   /* Get current working dir */

extern error_t cmd_handle_message(void);
extern error_t cmd_handle_put_file(void);
extern error_t cmd_handle_put_adf(void);

#endif
