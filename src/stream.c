#include "stream.h"
#include "types.h"

error_t stream_read(void *buffer, int32_t size, read_fn fn, int32_t *size_ret)
{
    error_t err = ERR_OK;

    if((buffer != NULL) && (size_ret != NULL)) {
        err = fn(buffer, size, size_ret);
    } else {
        err = ERR_NULLPOINTER;
    }

    return err;
}

error_t stream_write(const void *buffer, int32_t size, write_fn fn, int32_t *size_ret)
{
    error_t err = ERR_OK;

    if((buffer != NULL) && (size_ret != NULL)) {
        err = fn(buffer, size, size_ret);
    } else {
        err = ERR_NULLPOINTER;
    }

    return err;
}
