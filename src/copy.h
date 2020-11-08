#ifndef _AXAM_COPY_H_
#define _AXAM_COPY_H_

#include "types.h"
#include "error.h"
#include "trackdisk.h"


typedef error_t (*read_fn)(void *buffer, int32_t size, int32_t *size_ret);
typedef error_t (*write_fn)(const void *buffer, int32_t size, int32_t *size_ret);

/*
typedef error_t (*check_fn)(const void *buffer, int32_t size);
typedef void (*destroy_fn)(void *data);

typedef struct stream_config 
{
    read_fn reader;
    write_fn writer;
    void *stream_private;
} stream_config_t;
*/

extern uint32_t crc32(const void *buffer, int32_t size);

extern error_t copy(read_fn reader, write_fn writer, int32_t size, int32_t *size_ret);

/*
extern error_t stream_copy(stream_config_t input_stream, stream_config_t output_stream, int32_t size, int32_t *size_ret);
*/

#endif
