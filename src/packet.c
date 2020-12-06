#include "packet.h"
#include "crc.h"
#include "buffer.h"

static uint8_t ack_data[] = "OK";
static uint8_t nack_data[] = "KO";
static uint8_t resend_data[] = "RE";

static uint8_t packet_buffer[PACKET_MAX_LENGTH];

uint16_t packet_get_length(const void *packet)
{
    uint16_t size = 0;
    if(packet != NULL)
        size = *(uint16_t *) packet; 

    return size;
}

uint8_t *packet_get_data(void *packet)
{
    uint8_t *data = NULL;
    if(packet != NULL) {
        data = ((uint8_t *) (packet) + 2);
    }

    return data;
}

uint32_t packet_get_crc(const void *packet)
{
    uint32_t crc = 0;
    uint8_t *packet_bytes = NULL;
    uint16_t length = 0;

    if(packet != NULL) {
        length = packet_get_length(packet);
        packet_bytes = (uint8_t *) packet;
        packet_bytes += length - 4;
        crc = *(uint32_t *) packet_bytes;
    }

    return crc;
}

error_t packet_stream_write(const void *packet, write_fn fn, int32_t *size_ret)
{
    int16_t length = 0;
    error_t err = ERR_OK;

    if(packet != NULL) {
        length = packet_get_length(packet);
        err = fn(packet, length, size_ret);
    } else {
        err = ERR_NULLPOINTER;
    }

    return err;
} 

static error_t read_length(read_fn fn, uint16_t *length)
{
    int32_t size = 0;
    error_t err = ERR_OK;
    err = fn(length, 2, &size);
    return err;
}

error_t packet_stream_read(void *packet, read_fn fn, int32_t *size_ret)
{
    int32_t size = 0;
    uint16_t length = 0;
    uint8_t *data = NULL;
    error_t error = ERR_OK;

    if(packet == NULL || fn == NULL || size_ret == NULL)
    {
        error = ERR_NULLPOINTER;
    }

    if(error == ERR_OK) 
    {
        error = read_length(fn, &length);
    }

    if(error == ERR_OK) 
    {
        *(uint16_t *) packet = length;
        data = ((uint8_t *) packet) + 2;
        error = fn(data, length - 2, &size);
    }

    if(error == ERR_OK)
    {
        *size_ret = size + 2;
    }

    return error;
} 

error_t packet_calc_crc(const void *packet, uint32_t *crc_ret)
{
    uint16_t length = 0;
    uint32_t crc = 0;
    error_t error = ERR_OK;

    if((packet == NULL) || (crc_ret == NULL)) 
    {
        error = ERR_NULLPOINTER;
    }

    if(error == ERR_OK)
    {
        length = packet_get_length(packet);
        *crc_ret = crc32((uint8_t *) packet, length - 4);
    }

    return error;
}

bool_t packet_check(uint32_t crc, const void *packet)
{
    uint32_t packet_crc = 0;
    error_t err = ERR_OK;
    bool_t ok = 0;

    err = packet_calc_crc(packet, &packet_crc);

    if((err == ERR_OK) && (crc == packet_crc))
    {
        ok = 1;
    }
    
    return ok;
}


error_t packet_ack(write_fn fn, ack_t ack)
{
    error_t error = ERR_OK;
    int32_t size = 0;
    uint8_t *data = NULL;

    if(fn == NULL)
    {
        error = ERR_NULLPOINTER;
    }

    if(error == ERR_OK)
    {
        switch(ack)
        {
            case ACK_OK:
                data = ack_data;
                break;
            case ACK_NOK:
                data = nack_data;
                break;
            case ACK_RESEND:
                data = resend_data;
                break;
            default:
                error = ERR_INVALID_ARG;
                break;
        }
    }
    if(error == ERR_OK)
    {
        error = packet_create((void *) packet_buffer, data, 2);
    }

    if(error == ERR_OK)
    {
        error = packet_stream_write(packet_buffer, fn, &size);
    }

    if(error == ERR_OK)
    {
        if(size != 2)
        {
            error = ERR_WRITE_FAILED;
        }
    }

    return error;
}

error_t packet_wait_ack(read_fn fn, ack_t *ack_ret)
{
    error_t error = ERR_OK;
    int32_t size = 0;
    bool_t ok = false, ko = false, re = false;
    

    if(fn == NULL)
    {
        error = ERR_NULLPOINTER;
    }

    if(error == ERR_OK)
    {
        error = packet_stream_read(buffer, fn, &size);
    }

    if(error == ERR_OK)
    {
        if(size != 2)
        {
            error = ERR_WRITE_FAILED;
        }
    }

    if(error == ERR_OK)
    {   
        buffer[2] = 0;

        ok = strcmp(buffer, "OK") == 0;
        ko = strcmp(buffer, "KO") == 0;
        re = strcmp(buffer, "RE") == 0;
        
        if(ok == false && ko == false && re == false)
        {
            error = ERR_INVALID_ARG;
        }
    }

    if(error == ERR_OK)
    {
        if(ok)
        {
            *ack_ret = ACK_OK;
        }
        else if(ko)
        {
            *ack_ret = ACK_NOK;
        }
        else if(re)
        {
            *ack_ret = ACK_RESEND;
        }
    }

    return error;
}

error_t packet_create(void *packet, uint8_t *data, int32_t size)
{
    error_t error = ERR_OK;
    uint16_t length = 0;
    uint32_t crc = 0;
    uint8_t *pos = 0;

    if(packet == NULL || data == NULL)
    {
        error = ERR_NULLPOINTER;
    }

    if(error == ERR_OK)
    {
        if(size <= 0 || size >= PACKET_MAX_LENGTH - 6)
        {
            error = ERR_INVALID_ARG;
        }
    }

    if(error == ERR_OK)
    {
        length = (uint16_t) size + 6;
        *(uint16_t *) packet = length;
        memcpy((uint8_t *) packet + 2, data, size);
        error = packet_calc_crc(packet, &crc);
    }

    if(error == ERR_OK)
    {
        pos = (uint8_t *) packet + length - 4;
        *(uint32_t *) pos = crc;
    }

    return error;
}
