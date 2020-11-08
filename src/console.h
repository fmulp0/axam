#ifndef _AXAM_CONSOLE_H_
#define _AXAM_CONSOLE_H_

#include "types.h"
#include "error.h"

extern error_t console_writer(const void *buffer, int32_t size, int32_t *size_ret);

#endif 