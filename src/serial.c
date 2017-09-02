#include "serial.h"
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


error_t ser_init(void) {
    _device.SerialMP = (struct MsgPort *) CreatePort(NULL, NULL);

    if(_device.SerialMP == NULL) {
        printf("CreatePort failed\n");

        ser_shutdown();

        return MAKE_ERROR(SERIAL, ORIGIN_AX, ERR_OPEN);
    }

    _device.SerialIO = (struct IOExtSer *) CreateExtIO(_device.SerialMP, sizeof(struct IOExtSer));
    if (_device.SerialIO == NULL) {
        printf("CreateExtIO failed\n");
        ser_shutdown();

        return MAKE_ERROR(SERIAL, ORIGIN_AX, ERR_OPEN);
    }

    _device.SerialIO->io_SerFlags = SERF_7WIRE;

    if (OpenDevice(SERIALNAME, 0, (struct IORequest *) _device.SerialIO, 0L)) {
        printf("OpenDevice failed\n");
        ser_shutdown();

        return MAKE_ERROR(SERIAL, ORIGIN_AX, ERR_OPEN);
    }

    _device.is_open = 1;

    printf("open device %s successful\n", SERIALNAME);

    return RESULT_OK;
}

size_t ser_read_block(void *data, uint32_t size, error_t *error_ret) {
    _device.SerialIO->IOSer.io_Command  = CMD_READ;
    _device.SerialIO->IOSer.io_Length = size;
    _device.SerialIO->IOSer.io_Data = data;
    if (DoIO((struct IORequest *)_device.SerialIO)) {
        /* Inform user that query failed */
        printf("ser_read_block. Error: %d\n", _device.SerialIO->IOSer.io_Error);

        if(error_ret)
            *error_ret = MAKE_ERROR(SERIAL, ORIGIN_OS, _device.SerialIO->IOSer.io_Error);

        return 0;
    }

    if(error_ret)
        *error_ret = 0;


    return _device.SerialIO->IOSer.io_Actual;
}

size_t ser_write_block(const void *data, uint32_t size, error_t *error_ret) {
    _device.SerialIO->IOSer.io_Command  = CMD_WRITE;
    _device.SerialIO->IOSer.io_Length = size;
    _device.SerialIO->IOSer.io_Data = (void *)data;
    if (DoIO((struct IORequest *) _device.SerialIO)) {
        /* Inform user that query failed */
        printf("ser_write_block. Error: %d\n", _device.SerialIO->IOSer.io_Error);

        if(error_ret)
            *error_ret = MAKE_ERROR(SERIAL, ORIGIN_OS, _device.SerialIO->IOSer.io_Error);

        return 0;
    }

    if(error_ret)
        *error_ret = RESULT_OK;

    return _device.SerialIO->IOSer.io_Actual;
}

error_t ser_flush(void) {
    error_t error = RESULT_OK;

    _device.SerialIO->IOSer.io_Command  = CMD_FLUSH;
    _device.SerialIO->IOSer.io_Length = 0;
    _device.SerialIO->IOSer.io_Data = (void *)0;

    if (DoIO((struct IORequest *) _device.SerialIO)) {
        /* Inform user that query failed */
        printf("ser_flush. Error - %d\n", _device.SerialIO->IOSer.io_Error);
        error = MAKE_ERROR(SERIAL, ORIGIN_OS, _device.SerialIO->IOSer.io_Error);
    }

    return error;

}

uint32_t ser_write_byte(uint8_t data, error_t *error_ret)
{
    char cmd = data;

    return ser_write_block((void *) &cmd, 1, error_ret);
}


cmd_t ser_read_command(error_t *error_ret) {
    cmd_t cmd;

    uint32_t len = ser_read_block((void *) &cmd, sizeof(cmd_t), error_ret);
    if(len != sizeof(cmd_t)) {
        return 0;
    }

    if(error_ret)
        *error_ret = RESULT_OK;

    return cmd;
}

uint32_t ser_read_input_size(error_t *error_ret) {
    uint32_t result;


    uint32_t len = ser_read_block((void *) &result, sizeof(uint32_t), error_ret);
    if(len != sizeof(uint32_t)) {
        return 0;
    }

    if(error_ret)
        *error_ret = RESULT_OK;

    return result;
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
