#ifndef _AXAM_SERIAL_H_
#define _AXAM_SERIAL_H_

#include "types.h"

extern error_t ser_init(void);
extern void ser_shutdown(void);

extern size_t ser_write_block(const void *buffer, uint32_t size, error_t *error_ret);
extern size_t ser_read_block(void *buffer, uint32_t size, error_t *error_ret);

extern error_t ser_flush(void);
extern cmd_t ser_read_command(error_t *error_ret);
extern uint32_t ser_write_byte(uint8_t data, error_t *error_ret);
extern size_t ser_read_input_size(error_t *error_ret);

#endif
