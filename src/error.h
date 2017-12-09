#ifndef _AXAM_ERROR_H_
#define _AXAM_ERROR_H_

/* error sources */
#define SERIAL                  1
#define TRACKDISK               2
#define FILESYSTEM              3

/* error code origin */
#define ORIGIN_OS               1
#define ORIGIN_AX               2

/* error codes (for ORIGIN_AX) */
#define ERR_EXISTS              1
#define ERR_NO_SPACE            2
#define ERR_CREATE              3
#define ERR_OPEN                4
#define ERR_READ_FAILED         5
#define ERR_WRITE_FAILED        6

#define RESULT_OK               0

#define MAKE_ERROR(src, kind, err)   ((((src) & 0xF) << 28) | (((kind) & 0xF) << 24) | ((err) & 0xFFFF))

extern void drain_serial(void);
extern void error(error_t code, const char *message, int shutdown);



#endif
