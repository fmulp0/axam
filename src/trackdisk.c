#include "trackdisk.h"
#include "error.h"
#include <stdlib.h>
#include <stdio.h>
#include <exec/exec.h>
#include <proto/exec.h>

int                     is_open;
struct MsgPort *        TDMP;
struct IOExtTD *        TDIO;


error_t td_init(uint8_t drive) {
    error_t error = RESULT_OK;

    TDMP = (struct MsgPort *) CreatePort(NULL, NULL);

    if(TDMP == NULL) {
        printf("CreatePort failed\n");

        td_shutdown();

        return MAKE_ERROR(TRACKDISK, ORIGIN_AX, ERR_OPEN);
    }

    TDIO = (struct IOExtTD *) CreateExtIO(TDMP, sizeof(struct IOExtTD));
    if (TDIO == NULL) {
        printf("CreateExtIO failed\n");
        td_shutdown();

        return MAKE_ERROR(TRACKDISK, ORIGIN_AX, ERR_OPEN);
    }

    if (OpenDevice("trackdisk.device", drive, (struct IORequest *) TDIO, 0L)) {
        printf("OpenDevice failed\n");
        td_shutdown();

        return MAKE_ERROR(TRACKDISK, ORIGIN_AX, ERR_OPEN);
    }

    is_open = 1;

    td_motor(1, &error);

    if(error) {
        printf("open device trackdisk.device failed\n");

        return error;
    }

    printf("open device %s successful\n", "trackdisk.device");

    return RESULT_OK;
}

uint32_t td_read_sector(void *data, uint32_t sector_number, error_t *error_ret) {
    error_t error = RESULT_OK;

    TDIO->iotd_Count = 0;
    TDIO->iotd_Req.io_Offset = sector_number * SECTOR_LEN;
    TDIO->iotd_Req.io_Command  = CMD_READ;
    /* TDIO->iotd_Req.io_Flags = IOTDF_INDEXSNYC; */
    TDIO->iotd_Req.io_Length = SECTOR_LEN;
    TDIO->iotd_Req.io_Data = data;
    if (DoIO((struct IORequest *)TDIO)) {
        /* Inform user that query failed */
        printf("td_read_sector. Error - %d\n", TDIO->iotd_Req.io_Error);
        error = MAKE_ERROR(TRACKDISK, ORIGIN_OS, TDIO->iotd_Req.io_Error);
    }

    if(error_ret)
        *error_ret = error;

    return TDIO->iotd_Req.io_Actual;
}

uint32_t td_write_sector(const void *data, uint32_t sector_number, error_t *error_ret) {
    error_t error = RESULT_OK;

    TDIO->iotd_Count = 0;
    TDIO->iotd_Req.io_Offset = sector_number * TRACK_SIZE;
    /* TDIO->iotd_Req.io_Flags = IOTDF_INDEXSNYC; */
    TDIO->iotd_Req.io_Command  = CMD_WRITE;
    TDIO->iotd_Req.io_Length = TRACK_SIZE;
    TDIO->iotd_Req.io_Data = (void *)data;
    if (DoIO((struct IORequest *)TDIO)) {
        /* Inform user that query failed */
        printf("td_write_sector. Error - %d\n", TDIO->iotd_Req.io_Error);

        error = MAKE_ERROR(TRACKDISK, ORIGIN_OS, TDIO->iotd_Req.io_Error);
    }

    if(error_ret)
        *error_ret = error;

    return TDIO->iotd_Req.io_Actual;
}
/*
int td_disk_change_count() {
    TDIO->iotd_Req.io_Command  = TD_CHANGENUM;
    if (DoIO((struct IORequest *)TDIO)) {
        printf("td_disk_change_count. Error - %d\n", TDIO->iotd_Req.io_Error);
    }

    return (int)TDIO->iotd_Req.io_Actual;
}
*/

void td_motor(uint8_t on, error_t *error_ret) {
    error_t error = RESULT_OK;

    TDIO->iotd_Req.io_Length = on ? 1 : 0;
    TDIO->iotd_Req.io_Command  = TD_MOTOR;
    if (DoIO((struct IORequest *)TDIO)) {
        /* Inform user that query failed */
        printf("td_motor. Error - %d\n", TDIO->iotd_Req.io_Error);

        error = MAKE_ERROR(TRACKDISK, ORIGIN_OS, TDIO->iotd_Req.io_Error);
    }

    if(error_ret) {
        *error_ret = error;
    }
}

void td_shutdown(void) {
    if(is_open) {
        td_motor(0, NULL);

        AbortIO((struct IORequest *) TDIO);
        WaitIO((struct IORequest *) TDIO);
        CloseDevice((struct IORequest *) TDIO);
        is_open = 0;
    }

    if(TDIO != NULL) {
        DeleteExtIO((struct IORequest *) TDIO);
        TDIO = NULL;
    }

    if(TDMP != NULL) {
        DeletePort(TDMP);
        TDMP = NULL;
    }
}
