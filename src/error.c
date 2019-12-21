#include "error.h"
#include "serial.h"

#include <stdio.h>

char _buffer[512];

static error_function err_fn = 0;

void drain_serial(void)
{
    
}

void on_error(error_t e, const char *message, int shutdown)
{
    printf("error code: %08x, message: %s\n", e.code, message);
    if(err_fn)
        err_fn(e, message, shutdown);

    if(shutdown)
        exit(1);
}