#ifndef _AXAM_PACKET_H_
#define _AXAM_PACKET_H_

#include "types.h"
#include "error.h"
#include "stream.h"

/*   
      1
      5      87      0
     +----------------+     
     +0000---length---+
     +----------------+     
     +     payload    +
     +       ...      +
     +       ...      +
     +----------------+     
     +      crc32     +
     +      crc32     +
     +----------------+     
*/

#define PACKET_MAX_LENGTH                 4096
#define PACKET_MAX_RESEND                 3

typedef enum ack_enum
{
      ACK_OK,
      ACK_NOK,
      ACK_RESEND
} 
ack_t;


extern uint16_t packet_get_length(const void *packet);
extern uint8_t *packet_get_data(void *packet);
extern uint32_t packet_get_crc(const void *packet);

extern error_t packet_stream_write(const void *packet, write_fn fn, int32_t *size_ret);
extern error_t packet_stream_read(void *packet, read_fn fn, int32_t *size_ret);

extern error_t packet_calc_crc(const void *packet, uint32_t *crc_ret);

extern error_t packet_ack(write_fn fn, ack_t ack);

extern error_t packet_wait_ack(read_fn fn, ack_t *ack_ret);

extern error_t packet_create(void *packet, uint8_t *data, int32_t size);

#endif
