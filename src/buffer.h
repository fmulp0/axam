#ifndef _AXAM_BUFFER_H_
#define _AXAM_BUFFER_H_

#include "types.h"
#include "trackdisk.h"

#define BUFFER_SIZE         TRACK_SIZE

extern uint8_t __chip buffer[];
extern const int32_t buffer_size;


#endif
