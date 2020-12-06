#include "file.h"

#include <proto/dos.h>

int32_t file_handle = 0;
struct FileInfoBlock file_info = {0};

error_t file_open(char *name)
{
    error_t error = ERR_OK;


    error = file_examine(name, &file_info);

    if(error == ERR_OK)
    {
        file_handle = Open(name, MODE_OLDFILE);
        if(file_handle < 0) 
        {
            error_update_last();
            error = ERR_OPEN;
        }
    }
    return error;
}

error_t file_create_new(char *name)
{
    error_t error = ERR_OK;

    file_handle = Open(name, MODE_NEWFILE);
    if(file_handle < 0) 
    {
        error = ERR_EXISTS;
        error_update_last();
    }

    return error;
}

error_t file_examine(const char *name, struct FileInfoBlock *info_ret)
{
    error_t error = ERR_OK;
    BPTR lock = 0;
    bool_t unlock = false;

    if(name != NULL && info_ret != NULL)
    {
        error = ERR_NULLPOINTER;
    }

    if(error == ERR_OK)
    {
        lock = Lock((char *)name, ACCESS_READ);
        if(!lock) 
        {
            error_update_last();
            error = ERR_LOCK;
        } 
        else 
        {
            unlock = true;
        }
    }

    if(error == ERR_OK)
    {
        if(!Examine(lock, info_ret))  
        {
            error_update_last();
            error = ERR_EXAMINE;
        }
    }

    if(unlock)
    {
        UnLock(lock);
    }

    return error;
}

error_t file_writer(const void *buffer, int32_t size, int32_t *size_ret)
{
    error_t error = ERR_OK;
    int32_t bytes_written;

    if(buffer == NULL || size_ret == NULL)
    {
        error = ERR_NULLPOINTER;
    }

    if(error == ERR_OK)
    {
        bytes_written = Write(file_handle, buffer, size);
        if(bytes_written < 0)
        {
            error_update_last();
            error = ERR_WRITE_FAILED;
        }
        else
        {
            *size_ret = bytes_written;
        }
    }
    return error;
}

error_t file_reader(void *buffer, int32_t size, int32_t *size_ret)
{
    error_t error = ERR_OK;
    int32_t bytes_read = 0;

    if(buffer == NULL || size_ret == NULL)
    {
        error = ERR_NULLPOINTER;
    }

    if(error == ERR_OK)
    {
        bytes_read = Read(file_handle, buffer, size);
        if(bytes_read < 0) 
        {
            error_update_last();
        }
        else
        {
            *size_ret = bytes_read;
        }
    }

    return error;
}

void file_close(void)
{
    if(file_handle != 0) 
    {
        Close(file_handle);
    }

    file_handle = 0;
}
