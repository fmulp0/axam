#include "serial.h"
#include "packet.h"
#include "error.h"
#include <devices/serial.h>
#include <stdlib.h>
#include <stdio.h>
#include <exec/exec.h>
#include <proto/exec.h>

struct SercomDevice {
    int                     is_open;
    struct MsgPort *        SerialMP;       /* pointer to our message port */
    struct IOExtSer *       SerialIO;       /* pointer to our I/O request */
};

struct SercomDevice _device;

static uint8_t __chip ser_buffer[PACKET_MAX_LENGTH];

error_t ser_init(void) 
{
    error_t error = ERR_OK;

    _device.SerialMP = (struct MsgPort *) CreatePort(NULL, NULL);

    if(_device.SerialMP == NULL) 
    {
        printf("CreatePort failed\n");
        error_update_last(IoErr());
        ser_shutdown();
        error = ERR_OPEN;
    }

    if(error == ERR_OK)
    {
        _device.SerialIO = (struct IOExtSer *) CreateExtIO(_device.SerialMP, sizeof(struct IOExtSer));
        if (_device.SerialIO == NULL) {
            printf("CreateExtIO failed\n");
            error_update_last(IoErr());
            ser_shutdown();
            error = ERR_OPEN;
        }
    }

    _device.SerialIO->io_SerFlags = SERF_7WIRE;
    
    if(error == ERR_OK)
    {
        if (OpenDevice(SERIALNAME, 0, (struct IORequest *) _device.SerialIO, 0L)) {
            printf("OpenDevice failed\n");
            error_update_last();
            ser_shutdown();

            error = ERR_OPEN;
        }
    }

    if(error == ERR_OK)
    {
        _device.is_open = 1;
    }

    if(error == ERR_OK)
    {
        printf("open device %s successful\n", SERIALNAME);
    }
    
    return error;
}

error_t ser_reader(void *data, int32_t size, int32_t *size_ret) 
{
    error_t error = ERR_OK;

    if(data == NULL || size_ret == NULL)
    {
        error = ERR_NULLPOINTER;
    }

    if(error == ERR_OK)
    {
        _device.SerialIO->IOSer.io_Command  = CMD_READ;
        _device.SerialIO->IOSer.io_Length = size;
        _device.SerialIO->IOSer.io_Data = data;
        if (DoIO((struct IORequest *)_device.SerialIO)) {
            /* Inform user that query failed */
            printf("ser_read_block. Error: %d\n", _device.SerialIO->IOSer.io_Error);
            error_update_last(_device.SerialIO->IOSer.io_Error);
            error = ERR_READ_FAILED;
        }
    }

    if(error == ERR_OK)
    {
        *size_ret = _device.SerialIO->IOSer.io_Actual;
    }

    return error;
}

/*
error_t ser_read_int8(int8_t *result) {
    return ser_read_block(result, sizeof(int8_t), 0);
}

error_t ser_read_int16(int16_t *result) {
    return ser_read_block(result, sizeof(int16_t), 0);
}

error_t ser_read_int32(int32_t *result) {
    return ser_read_block(result, sizeof(int32_t), 0);
}
*/
error_t ser_writer(const void *buffer, int32_t size, int32_t *size_ret) 
{
    error_t error = ERR_OK;

    if(buffer == NULL || size_ret == NULL)
    {
        error = ERR_NULLPOINTER;
    }

    if(error == ERR_OK)
    {
        _device.SerialIO->IOSer.io_Command  = CMD_WRITE;
        _device.SerialIO->IOSer.io_Length = size;
        _device.SerialIO->IOSer.io_Data = (void *) buffer;
        if (DoIO((struct IORequest *) _device.SerialIO)) {
            /* Inform user that query failed */
            printf("ser_write_block. Error: %d\n", _device.SerialIO->IOSer.io_Error);
            error_update_last(_device.SerialIO->IOSer.io_Error);
            return error;
        }
    }

    if(error == ERR_OK)
    {
        *size_ret = _device.SerialIO->IOSer.io_Actual;
    }

    return error;
}

error_t ser_flush(void) 
{
    error_t error = ERR_OK;

    _device.SerialIO->IOSer.io_Command  = CMD_FLUSH;
    _device.SerialIO->IOSer.io_Length = 0;
    _device.SerialIO->IOSer.io_Data = (void *)0;

    if (DoIO((struct IORequest *) _device.SerialIO)) {
        /* Inform user that query failed */
        printf("ser_flush. Error - %d\n", _device.SerialIO->IOSer.io_Error);
        error = ERR_WRITE_FAILED;
        error_update_last(_device.SerialIO->IOSer.io_Error);
    }

    return error;

}

/*
error_t ser_write_int8(int8_t value) {
    return ser_write_block(&value, sizeof(int8_t), 0);
}

error_t ser_write_int16(int16_t value) {
    return ser_write_block(&value, sizeof(int16_t), 0);
}

error_t ser_write_int32(int32_t value) 
{
    return ser_write_block(&value, sizeof(int32_t), 0);
}
*/

error_t ser_read_data(uint8_t **buffer, int32_t *size_ret)
{
    error_t error = ERR_OK;
    uint16_t length = 0;
    int32_t size = 0;

    if(buffer == NULL || size_ret == NULL)
    {
        error = ERR_NULLPOINTER;
    }

    if(error == ERR_OK)
    {
        error = data_stream_packet_read(ser_buffer, ser_reader, &size);
    }

    if(error == ERR_OK)
    {
        *size_ret = size;
        *buffer = &ser_buffer[0];
    }

    return error;
}

void ser_shutdown(void) {
    if(_device.is_open) {
        AbortIO((struct IORequest *) _device.SerialIO);
        WaitIO((struct IORequest *) _device.SerialIO);
        CloseDevice((struct IORequest *) _device.SerialIO);
        _device.is_open = 0;
    }

    if(_device.SerialIO != NULL) {
        DeleteExtIO((struct IORequest *) _device.SerialIO);
        _device.SerialIO = NULL;
    }

    if(_device.SerialMP != NULL) {
        DeletePort(_device.SerialMP);
        _device.SerialMP = NULL;
    }
}
