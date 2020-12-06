#ifndef _AXAM_STREAM_H_
#define _AXAM_STREAM_H_

#include "error.h"


typedef error_t (*read_fn)(void *buffer, int32_t size, int32_t *size_ret);
typedef error_t (*write_fn)(const void *buffer, int32_t size, int32_t *size_ret);


/*

typedef struct stream_config 
{
    read_fn reader;
    write_fn writer;
    void *stream_private;
} stream_config_t;

typedef error_t (*check_fn)(const void *buffer, int32_t size);
typedef void (*destroy_fn)(void *data);

extern stream_config_t stream_init(void *);
extern void stream_cleanup(stream_config_t cfg);
*/


extern error_t stream_read(void *buffer, int32_t size, read_fn fn, int32_t *size_ret);
extern error_t stream_write(const void *buffer, int32_t size, write_fn fn, int32_t *size_ret);

#endif
