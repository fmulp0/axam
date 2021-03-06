#ifndef _AXAM_FILE_H_
#define _AXAM_FILE_H_

#include "types.h"
#include "error.h"

extern int32_t file_handle;            /* opened file handle */
extern struct FileInfoBlock file_info; /* file info of opened file */

extern error_t file_open(char *name);
extern error_t file_create_new(char *name);
extern void file_close(void);
extern error_t file_examine(const char *name, struct FileInfoBlock *info_ret);
extern error_t file_writer(const void *buffer, int32_t size, int32_t *size_ret);
extern error_t file_reader(void *buffer, int32_t size, int32_t *size_ret);

#endif