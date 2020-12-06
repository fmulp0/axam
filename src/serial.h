#ifndef _AXAM_SERIAL_H_
#define _AXAM_SERIAL_H_

#include "types.h"
#include "error.h"

extern error_t ser_init(void);
extern void ser_shutdown(void);

extern error_t ser_read_data(uint8_t **data, int32_t *size_ret);

extern error_t ser_writer(const void *buffer, int32_t size, int32_t *size_ret);
/*
extern error_t ser_write_int8(int8_t value);
extern error_t ser_write_int16(int16_t value);
extern error_t ser_write_int32(int32_t value);
*/
extern error_t ser_reader(void *buffer, int32_t size, int32_t *size_ret);
/*
extern error_t ser_read_int8(int8_t *result);
extern error_t ser_read_int16(int16_t *result);
extern error_t ser_read_int32(int32_t *result);
*/
extern error_t ser_flush(void);
/*
extern error_t ser_read_command(cmd_t *cmd_ret);
extern error_t ser_write_string(const char *s);
extern error_t ser_write_error(error_t error);
*/
#endif
