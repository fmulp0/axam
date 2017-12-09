#ifndef _AXAM_SERIAL_H_
#define _AXAM_SERIAL_H_

#include "types.h"

extern error_t ser_init(void);
extern void ser_shutdown(void);

extern error_t ser_write_block(const void *buffer, uint32_t size, size_t *size_ret);
extern error_t ser_read_block(void *buffer, uint32_t size, size_t *size_ret);

extern error_t ser_flush(void);
extern error_t ser_read_command(cmd_t *cmd_ret);
extern error_t ser_write_byte(uint8_t data, size_t *size_ret);
extern error_t ser_read_input_size(size_t *size_ret);

#endif
