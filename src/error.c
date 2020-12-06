#include "error.h"
#include "serial.h"

#include <stdio.h>

static error_fn err_fn = 0;
static int32_t error_io_last = 0;

void drain_serial(void)
{
    
}

void error_update_last(int32_t err)
{
    error_io_last = err;
}

void on_error(error_t e, const char *message, int shutdown)
{
    printf("error code: %x, message: %s\n", e, message);
    if(err_fn)
    {
        err_fn(e, message, shutdown);
    }

    if(shutdown)
    {
        exit(1);
    }
}
