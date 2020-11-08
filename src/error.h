#ifndef _AXAM_ERROR_H_
#define _AXAM_ERROR_H_

/* error sources */
#define SERIAL                  1
#define TRACKDISK               2
#define FILESYSTEM              3

/* error code origin */
#define ORIGIN_OS               1
#define ORIGIN_AX               2

#define ERR_OK                  0
/* error codes (for ORIGIN_AX) */
#define ERR_EXISTS              1
#define ERR_NO_SPACE            2
#define ERR_CREATE              3
#define ERR_OPEN                4
#define ERR_READ_FAILED         5
#define ERR_WRITE_FAILED        6
#define ERR_LOCK                7
#define ERR_EXAMINE             8


#define SUCCEEDED(err_t)        ((err_t).code == ERR_OK)
#define FAILED(err_t)           ((err_t).code != ERR_OK)

#define MAKE_ERROR(error, source, origin, errcode) do { (error).code = (errcode); (error).source_origin = (((origin) & 0xf) << 4) || ((source) & 0x0f); } while(0)

#define ERROR_SOURCE(error)     ((error).source_origin & 0xf)
#define ERROR_ORIGIN(error)     ((error).source_origin >> 4)
#define ERROR_CODE(error)       ((error).code)

typedef struct {
    int code;
    int source_origin;
} error_t;

typedef void (*error_function)(error_t e, const char *message, int shutdown);

extern void drain_serial(void);
extern void on_error(error_t code, const char *message, int shutdown);
extern void set_error_function(error_function fn);

#endif
