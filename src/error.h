#ifndef _AXAM_ERROR_H_
#define _AXAM_ERROR_H_

#include "types.h"

/* error sources */

typedef enum error_source {
    ERR_SOURCE_SERIAL = 1,
    ERR_SOURCE_TRACKDISK,
    ERR_SOURCE_FILESYSTEM
} error_source_t;

/* error code origin */

typedef enum error_origin {
    ERR_ORIGIN_OS = 1,
    ERR_ORIGIN_AX
} error_origin_t;

typedef enum error_code {
    ERR_OK = 0,
    ERR_NULLPOINTER,
    ERR_INVALID_ARG,
    ERR_NOT_COMPLETE,
    ERR_EXISTS,
    ERR_NO_SPACE,
    ERR_CREATE,
    ERR_OPEN,
    ERR_READ_FAILED,
    ERR_WRITE_FAILED,
    ERR_LOCK,
    ERR_EXAMINE,
    ERR_MOTOR
} 
error_t;

typedef void (*error_fn)(error_t e, const char *message, int shutdown);

extern void drain_serial(void);
extern void on_error(error_t code, const char *message, int shutdown);
extern void set_error_function(error_fn fn);

extern int32_t io_get_last_error(void);
extern int32_t error_last_get(void);
extern void update_error_last(int32_t code);

#endif
