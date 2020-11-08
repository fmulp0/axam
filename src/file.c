#include "file.h"

#include <proto/dos.h>

int32_t file_handle = 0;

error_t file_open(char *name)
{
    error_t error = { ERR_OK, 0 };

    file_handle = Open(name, MODE_OLDFILE);
    if(file_handle < 0) 
    {
        MAKE_ERROR(error, FILESYSTEM, ORIGIN_AX, ERR_OPEN);
        on_error(error, "file_open: Open failed", 0);
        return error;
    }

    return error;
}

error_t file_create_new(char *name)
{
    error_t error = { ERR_OK, 0 };

    file_handle = Open(name, MODE_NEWFILE);
    if(file_handle < 0) 
    {
        MAKE_ERROR(error, FILESYSTEM, ORIGIN_AX, ERR_EXISTS);
        on_error(error, "file_create_new: Open failed", 0);
        return error;
    }

    return error;
}

error_t file_writer(const void *buffer, int32_t size, int32_t *size_ret)
{
    error_t error = { ERR_OK, 0 };

    int32_t bytes_written;

    bytes_written = Write(file_handle, buffer, size);
    if(size_ret)
    {
        *size_ret = bytes_written;
    }

    return error;
}

void file_close(void)
{
    if(file_handle > 0) 
    {
        Close(file_handle);
    }

    file_handle = 0;
}
