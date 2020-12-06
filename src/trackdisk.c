#include "types.h"
#include "trackdisk.h"
#include "error.h"

#include <devices/trackdisk.h>
#include <exec/exec.h>
#include <stdio.h>
#include <stdlib.h>

bool_t                  is_open;
struct MsgPort *        TDMP;
struct IOExtTD *        TDIO;

error_t td_init(int8_t drive) 
{
    error_t error = ERR_OK;

    if(drive < 0 || drive > 3)
    {
        error = ERR_INVALID_ARG;
    }

    if(error == ERR_OK)
    {
        TDMP = (struct MsgPort *) CreatePort(NULL, NULL);

        if(TDMP == NULL) {
            puts("CreatePort failed\n");
            td_shutdown();
            error = ERR_OPEN;
            error_update_last(IoErr());
        }
    }

    if(error == ERR_OK)
    {
        TDIO = (struct IOExtTD *) CreateExtIO(TDMP, sizeof(struct IOExtTD));
        if (TDIO == NULL) {
            printf("CreateExtIO failed\n");
            td_shutdown();
            error = ERR_OPEN;
            error_update_last(IoErr());
        }
    }

    if(error == ERR_OK)
    {
        if (OpenDevice("trackdisk.device", drive, (struct IORequest *) TDIO, 0L)) {
            printf("OpenDevice failed\n");
            td_shutdown();

            error = ERR_OPEN;
        }
    }

    if(error == ERR_OK)
    {
        is_open = true;

        error = td_motor(1);
    }

    if(error == ERR_OK) 
    {
        printf("open device %s successful\n", "trackdisk.device");
    }
    else
    {
        printf("open device trackdisk.device failed\n");
    }

    return error;
}

error_t td_read_sector(void *buffer, uint32_t sector_number, int32_t *size_ret) 
{
    error_t error = ERR_OK;

    if(buffer == NULL || size_ret == NULL)
    {
        error = ERR_NULLPOINTER;
    }

    if(error == ERR_OK)
    {
        if(sector_number >= NUMSECS)
        {
            error = ERR_INVALID_ARG;
        }
    }

    if(error == ERR_OK)
    {
        TDIO->iotd_Count = 0;
        TDIO->iotd_Req.io_Offset = sector_number * SECTOR_LEN;
        TDIO->iotd_Req.io_Command  = CMD_READ;
        /* TDIO->iotd_Req.io_Flags = IOTDF_INDEXSNYC; */
        TDIO->iotd_Req.io_Length = SECTOR_LEN;
        TDIO->iotd_Req.io_Data = buffer;
        if (DoIO((struct IORequest *)TDIO)) {
            /* Inform user that query failed */
            printf("td_read_sector. Error - %d\n", TDIO->iotd_Req.io_Error);
            error = ERR_READ_FAILED;
            error_update_last(TDIO->iotd_Req.io_Error);
        }

    }

    if(error == ERR_OK)
    {
        *size_ret = TDIO->iotd_Req.io_Actual;
    }

    return error;
}

error_t td_write_sector(const void *buffer, uint32_t sector_number, int32_t *size_ret) {
    error_t error = ERR_OK;

    if(buffer == NULL || size_ret == NULL)
    {
        error = ERR_NULLPOINTER;
    }

    if(error == ERR_OK)
    {
        if(sector_number >= NUMSECS)
        {
            error = ERR_INVALID_ARG;
        }
    }

    if(error == ERR_OK)
    {
        TDIO->iotd_Count = 0;
        TDIO->iotd_Req.io_Offset = sector_number * TRACK_SIZE;
        /* TDIO->iotd_Req.io_Flags = IOTDF_INDEXSNYC; */
        TDIO->iotd_Req.io_Command  = CMD_WRITE;
        TDIO->iotd_Req.io_Length = TRACK_SIZE;
        TDIO->iotd_Req.io_Data = (void *)buffer;
        if (DoIO((struct IORequest *)TDIO)) {
            /* Inform user that query failed */
            printf("td_write_sector. Error - %d\n", TDIO->iotd_Req.io_Error);
            error = ERR_WRITE_FAILED;
            error_update_last(TDIO->iotd_Req.io_Error);
        }
    }

    if(error == ERR_OK)
    {
        *size_ret = TDIO->iotd_Req.io_Actual;
    }

    return error;
}

error_t td_format_sector(const void *buffer, uint32_t sector_number) {
    error_t error = ERR_OK;

    if(buffer == NULL)
    {
        error = ERR_NULLPOINTER;
    }

    if(error == ERR_OK)
    {
        if(sector_number >= NUMSECS)
        {
            error = ERR_INVALID_ARG;
        }
    }

    if(error == ERR_OK)
    {
        TDIO->iotd_Count = 0;
        TDIO->iotd_Req.io_Offset = sector_number * TRACK_SIZE;
        /* TDIO->iotd_Req.io_Flags = IOTDF_INDEXSNYC; */
        TDIO->iotd_Req.io_Command  = TD_FORMAT;
        TDIO->iotd_Req.io_Length = TRACK_SIZE;
        TDIO->iotd_Req.io_Data = (void *)buffer;
        if (DoIO((struct IORequest *)TDIO)) 
        {
            /* Inform user that query failed */
            printf("td_format_sector. Error - %d\n", TDIO->iotd_Req.io_Error);
            error = ERR_WRITE_FAILED;
            error_update_last(TDIO->iotd_Req.io_Error);
        }
    }

    return error;
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

error_t td_motor(uint8_t on) {
    error_t error = ERR_OK;

    TDIO->iotd_Req.io_Length = on ? 1 : 0;
    TDIO->iotd_Req.io_Command  = TD_MOTOR;
    if (DoIO((struct IORequest *)TDIO)) {
        /* Inform user that query failed */
        printf("td_motor. Error - %d\n", TDIO->iotd_Req.io_Error);
        error = ERR_MOTOR;
        error_update_last(TDIO->iotd_Req.io_Error);
    }

    return error;
}

void td_shutdown(void) {
    if(is_open) {
        td_motor(0);

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
