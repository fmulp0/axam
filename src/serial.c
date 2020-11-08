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
    error_t error = { ERR_OK, 0 };

    _device.SerialMP = (struct MsgPort *) CreatePort(NULL, NULL);

    if(_device.SerialMP == NULL) {
        printf("CreatePort failed\n");

        ser_shutdown();

        MAKE_ERROR(error, SERIAL, ORIGIN_AX, ERR_OPEN);
        return error;
    }

    _device.SerialIO = (struct IOExtSer *) CreateExtIO(_device.SerialMP, sizeof(struct IOExtSer));
    if (_device.SerialIO == NULL) {
        printf("CreateExtIO failed\n");
        ser_shutdown();

        MAKE_ERROR(error, SERIAL, ORIGIN_AX, ERR_OPEN);
        return error;
    }

    _device.SerialIO->io_SerFlags = SERF_7WIRE;

    if (OpenDevice(SERIALNAME, 0, (struct IORequest *) _device.SerialIO, 0L)) {
        printf("OpenDevice failed\n");
        ser_shutdown();

        MAKE_ERROR(error, SERIAL, ORIGIN_AX, ERR_OPEN);
        return error;
    }

    _device.is_open = 1;

    printf("open device %s successful\n", SERIALNAME);
    
    MAKE_ERROR(error, 0, 0, 0);
    return error;
}

error_t ser_read_block(void *data, int32_t size, int32_t *size_ret) {
    error_t error = { ERR_OK, 0 };

    _device.SerialIO->IOSer.io_Command  = CMD_READ;
    _device.SerialIO->IOSer.io_Length = size;
    _device.SerialIO->IOSer.io_Data = data;
    if (DoIO((struct IORequest *)_device.SerialIO)) {
        /* Inform user that query failed */
        printf("ser_read_block. Error: %d\n", _device.SerialIO->IOSer.io_Error);

        MAKE_ERROR(error, SERIAL, ORIGIN_OS, _device.SerialIO->IOSer.io_Error);

        return error;
    }

    if(size_ret)
        *size_ret = _device.SerialIO->IOSer.io_Actual;

    return error;
}

error_t ser_read_int8(int8_t *result) {
    return ser_read_block(result, sizeof(int8_t), 0);
}

error_t ser_read_int16(int16_t *result) {
    return ser_read_block(result, sizeof(int16_t), 0);
}

error_t ser_read_int32(int32_t *result) {
    return ser_read_block(result, sizeof(int32_t), 0);
}

error_t ser_write_block(const void *data, int32_t size, int32_t *size_ret) {
    error_t error = { ERR_OK, 0 };

    _device.SerialIO->IOSer.io_Command  = CMD_WRITE;
    _device.SerialIO->IOSer.io_Length = size;
    _device.SerialIO->IOSer.io_Data = (void *)data;
    if (DoIO((struct IORequest *) _device.SerialIO)) {
        /* Inform user that query failed */
        printf("ser_write_block. Error: %d\n", _device.SerialIO->IOSer.io_Error);

        MAKE_ERROR(error, SERIAL, ORIGIN_OS, _device.SerialIO->IOSer.io_Error);

        return error;
    }

    if(size_ret)
        *size_ret = _device.SerialIO->IOSer.io_Actual;

    return error;
}

error_t ser_flush(void) {
    error_t error = { ERR_OK, 0 };

    _device.SerialIO->IOSer.io_Command  = CMD_FLUSH;
    _device.SerialIO->IOSer.io_Length = 0;
    _device.SerialIO->IOSer.io_Data = (void *)0;

    if (DoIO((struct IORequest *) _device.SerialIO)) {
        /* Inform user that query failed */
        printf("ser_flush. Error - %d\n", _device.SerialIO->IOSer.io_Error);
        MAKE_ERROR(error, SERIAL, ORIGIN_OS, _device.SerialIO->IOSer.io_Error);
    }

    return error;

}


error_t ser_write_int8(int8_t value) {
    return ser_write_block(&value, sizeof(int8_t), 0);
}

error_t ser_write_int16(int16_t value) {
    return ser_write_block(&value, sizeof(int16_t), 0);
}

error_t ser_write_int32(int32_t value) {
    return ser_write_block(&value, sizeof(int32_t), 0);
}


/*
error_t ser_write_byte(uint8_t data, int32_t *size_ret)
{
    char cmd = data;

    return ser_write_block((void *) &cmd, 1, size_ret);
}
*/

error_t ser_read_command(cmd_t *cmd_ret) {
    cmd_t cmd;
    error_t error = { ERR_OK, 0 };
    int32_t len = 0;

    error = ser_read_block((void *) &cmd, sizeof(cmd_t), &len);
    if(FAILED(error))
        return error;

    if(cmd_ret)
        *cmd_ret = cmd;

    return error;
}

/*
error_t ser_read_input_size(int32_t *size_ret) {
    int32_t result;
    int32_t len = 0;
    error_t error = RESULT_OK;


    error = ser_read_block((void *) &result, sizeof(int32_t), &len);
    if(error) {
        return error;
    }

    if(size_ret)
        *size_ret = result;

    return error;
}
*/

error_t ser_write_string(const char *s) {
    int32_t written, len = strlen(s);
    error_t error = { ERR_OK, 0 };
    if(len == 0)
        return error;

    error = ser_write_block((const void *) &len, sizeof(len), &written);
    if (FAILED(error)) {
        on_error(error, "write_string: write length failed", 0);

        return error;
    }

    error = ser_write_block((const void *) s, len, &written);
    if (FAILED(error)) {
        on_error(error, "write_string: write data failed", 0);
    }

    return error;
}

error_t ser_write_error(error_t error) {
    return ser_write_block(&error, sizeof(error), 0);
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
