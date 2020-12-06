#include "crc.h"

static uint8_t crc32_table[256];

void crc_init(void)
{
}

uint32_t crc32(const uint8_t *data, uint16_t length)
{
    uint32_t crc = 0;
    uint32_t i = 0;
    uint8_t b = 0;

    for(;i < length; i++) {
        uint8_t byte = data[i];
        for(b = 0; b < 8; b++) {
            if(((crc >> 31) & 1) != ((byte >> 7) & 1))
                crc = (crc << 1) ^ crc_polynom;
            else
                crc = (crc << 1);

            byte = byte << 1;            
        }
    }

    return crc;
}
