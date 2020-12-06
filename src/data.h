#ifndef _AXAM_DATA_H_
#define _AXAM_DATA_H_

#include "types.h"
#include "error.h"
#include "stream.h"

/* reads at most PACKET_MAX_LENGTH bytes */
extern error_t data_stream_packet_read(uint8_t *data, read_fn fn, int32_t *size_ret);

/* writes at most PACKET_MAX_LENGTH bytes */
extern error_t data_stream_packet_write(const uint8_t *data, write_fn fn, int32_t size, int32_t *size_ret);

#endif
