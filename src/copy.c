#include "copy.h"
#include "macros.h"
#include "buffer.h"

static uint8_t __chip buf[BUFFER_SIZE + 1];

static error_t copy_buffer(read_fn reader, write_fn writer, void *buffer, int32_t buf_size, int32_t size, int32_t *size_ret)
{
    int32_t bytes_read = 0, read_size, write_size, bytes_to_read, bytes_to_write, block_size;
    error_t error = { ERR_OK, 0 };

    block_size = MIN(buf_size, size);

    while(bytes_read < size) 
    {
        bytes_to_read = MIN(block_size, size - bytes_read);

        error = reader(buffer, bytes_to_read, &read_size);
        if(FAILED(error)) 
        {
            on_error(error, "read failed", 0);
        }

        if(bytes_read == 0)
            break;

        bytes_to_write = bytes_read;
        do {
            error = writer(buffer, bytes_to_write, &write_size);
            if(FAILED(error))
            {
                on_error(error, "write failed", 0);            
            }
            bytes_to_write -= write_size;
        } while(bytes_to_write > 0);

        bytes_read += read_size;
    }

    if(size_ret) 
    {
        *size_ret = bytes_read;
    }
    return error;
}


error_t copy(read_fn reader, write_fn writer, int32_t size, int32_t *size_ret)
{
    return copy_buffer(reader, writer, &buf[0], BUFFER_SIZE, size, size_ret);
}
