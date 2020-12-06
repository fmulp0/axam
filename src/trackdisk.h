#ifndef _AXAM_TRACKDISK_H_
#define _AXAM_TRACKDISK_H_

#include "types.h"
#include "error.h"

#include <exec/types.h>
#include <devices/trackdisk.h>


#define RAW_TRACK_LEN           0x397c
#define SECTOR_LEN              TD_SECTOR

#define TRACK_SIZE              (TD_SECTOR * NUMSECS)

extern error_t td_init(int8_t drive);
extern void td_shutdown(void);

extern error_t td_write_sector(const void *buffer, uint32_t sector_number, int32_t *size_ret);
extern error_t td_read_sector(void *buffer, uint32_t sector_number, int32_t *size_ret);

extern error_t td_format_sector(const void *buffer, uint32_t sector_number);

extern error_t td_motor(uint8_t on);

#endif
