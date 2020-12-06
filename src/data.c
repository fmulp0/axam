#include "data.h"
#include "packet.h"
#include "stream.h"

#include <string.h>

uint8_t data_buffer[PACKET_MAX_LENGTH];

error_t data_stream_packet_read(uint8_t *data, read_fn fn, int32_t *size_ret)
{
    error_t error = ERR_OK;
    uint16_t length = 0;
    int32_t bytes_to_copy = 0, size = 0;


    if(data == NULL || size_ret == NULL)
    {
        error = ERR_NULLPOINTER;
    }

    if(error == ERR_OK)
    {
        error = packet_stream_read((void *)data_buffer, fn, &size);
    }

    if(error == ERR_OK)
    {
        if(size < 6) 
        {
            error = ERR_READ_FAILED;
        }
    }

    if(error == ERR_OK)
    {
        length = packet_get_length((const void *) data_buffer);
        if(length < 6)
        {
            error = ERR_READ_FAILED;
        }       
    }

    if(error == ERR_OK)
    {
        bytes_to_copy = (int32_t) length - 6;
        if(bytes_to_copy > 0) 
        {
            memcpy((void *) data, (const void *) data_buffer, bytes_to_copy);
        }

        *size_ret = bytes_to_copy;
    }

    return error;
}

error_t data_stream_packet_write(const uint8_t *data, write_fn fn, int32_t size, int32_t *size_ret)
{
    error_t error = ERR_OK;
    uint8_t *pos = NULL;
    uint16_t length = 0;
    uint32_t crc = 0;
    int32_t bytes_written = 0;

    if(data == NULL || size_ret == NULL || fn == NULL)
    {
        error = ERR_NULLPOINTER;
    }

    if(error == ERR_OK)
    {
        if(size == 0 || (size > PACKET_MAX_LENGTH - 6))
        {
            error = ERR_INVALID_ARG;
        }
    }

    if(error == ERR_OK)
    {
        length = (uint16_t) size + 6;
        *(uint16_t *) data_buffer = length;
        memcpy(&data_buffer[2], data, size);
        error = packet_calc_crc((const void *) data_buffer, &crc);
    }

    if(error == ERR_OK)
    {
        pos = &data_buffer[length - 4];
        *(uint32_t *) pos = crc;

        error = packet_stream_write((const void *) data_buffer, fn, &bytes_written);
    }

    if(error != ERR_OK)
    {
        if(bytes_written < 6)
        {
            error = ERR_WRITE_FAILED;
        }
    }

    if(error == ERR_OK)
    {
        *size_ret = bytes_written - 6;
    }

    return error;
}

error_t data_write_string(const char *str, write_fn fn, int32_t *size_ret)
{
    error_t error = ERR_OK;
    int32_t size = 0;

    if(str == NULL || size_ret == NULL || fn == NULL)
    {
        error = ERR_NULLPOINTER;
    }

    if(error == ERR_OK)
    {
        size = strlen(str) + 1;
        if(size > PACKET_MAX_LENGTH)
        {
            error = ERR_INVALID_ARG;
        }
    }

    if(error == ERR_OK)
    {
        error = data_stream_packet_write(str, fn, size, size_ret);
    }

    return error;
}

