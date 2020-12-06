#ifndef _AXAM_CRC_H_
#define _AXAM_CRC_H_

#include "types.h"

static const uint32_t crc_polynom = 0x04c11db7;

extern void crc32_init(void);

extern uint32_t crc32(const uint8_t *data, uint16_t length);

#endif
