#ifndef _AXAM_COPY_H_
#define _AXAM_COPY_H_

#include "types.h"
#include "error.h"
#include "trackdisk.h"
#include "stream.h"


extern error_t copy(read_fn reader, write_fn writer, int32_t size, int32_t *size_ret);


#endif
