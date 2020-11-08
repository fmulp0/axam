#include "console.h"

error_t console_writer(const void *buffer, int32_t size, int32_t *size_ret)
{
    uint8_t *c = buffer;
    int32_t to_write = size;
    error_t error = { ERR_OK, 0 };

    while(to_write > 0)
    {
        putchar(*c++);
        to_write--;
    }

    if(size_ret) 
    {
        *size_ret = size - to_write;
    }

    return error;
}