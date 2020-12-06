#ifndef _AXAM_TYPES_H_
#define _AXAM_TYPES_H_

#include <stdlib.h>
#include <stdio.h>
#undef NULL
#define NULL 0
#include <exec/types.h>

typedef signed char         int8_t;
typedef unsigned char       uint8_t;

typedef signed short        int16_t;
typedef unsigned short      uint16_t;

typedef signed int          int32_t;
typedef unsigned int        uint32_t;

typedef uint8_t             bool_t;

#define false               0
#define true                1

#endif
