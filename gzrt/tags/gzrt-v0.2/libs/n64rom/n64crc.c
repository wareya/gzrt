/* 
 * Copyright notice for this file:
 *  Copyright (C) 2005 Parasyte
 *
 * Based on uCON64's N64 checksum algorithm by Andreas Sterbenz
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include <stdio.h>
#include <stdlib.h>
#include <n64rom.h>

/* Macros */
#define ROL(i, b) (((i) << (b)) | ((i) >> (32 - (b))))
#define CRCBYTES2LONG(b) ( (b)[0] << 24 | \
                        (b)[1] << 16 | \
                        (b)[2] <<  8 | \
                        (b)[3] )

/* Defines */
#define N64_HEADER_SIZE  0x40
#define N64_BC_SIZE      (0x1000 - N64_HEADER_SIZE)
#define N64_CRC1         0x10
#define N64_CRC2         0x14
#define CHECKSUM_START   0x00001000
#define CHECKSUM_LENGTH  0x00100000
#define CHECKSUM_CIC6102 0xF8CA4DDC
#define CHECKSUM_CIC6103 0xA3886759
#define CHECKSUM_CIC6105 0xDF26F436
#define CHECKSUM_CIC6106 0x1FEA617A

static unsigned int crc_table[256];

static void gen_table() 
{
    unsigned int crc, poly;
    int i, j;

    poly = 0xEDB88320;
    for (i = 0; i < 256; i++) {
        crc = i;
        for (j = 8; j > 0; j--) {
            if (crc & 1) crc = (crc >> 1) ^ poly;
            else crc >>= 1;
        }
        crc_table[i] = crc;
    }
}

static unsigned int n64_crc32 (unsigned char *data, int len) 
{
    unsigned int crc = ~0;
    int i;

    for (i = 0; i < len; i++) {
        crc = (crc >> 8) ^ crc_table[(crc ^ data[i]) & 0xFF];
    }

    return ~crc;
}


static int N64GetCIC(unsigned char *data) 
{
    switch (n64_crc32(&data[N64_HEADER_SIZE], N64_BC_SIZE)) {
        case 0x6170A4A1: return 6101;
        case 0x90BB6CB5: return 6102;
        case 0x0B050EE0: return 6103;
        case 0x98BC2C86: return 6105;
        case 0xACC8580A: return 6106;
    }

    return 0;
}

static int N64CalcCRC(unsigned int *crc, unsigned char *data) 
{
    int bootcode, i;
    unsigned int seed;

    unsigned int t1, t2, t3;
    unsigned int t4, t5, t6;
    unsigned int r, d;


    switch ((bootcode = N64GetCIC(data))) {
        case 6101:
        case 6102:
            seed = CHECKSUM_CIC6102;
            break;
        case 6103:
            seed = CHECKSUM_CIC6103;
            break;
        case 6105:
            seed = CHECKSUM_CIC6105;
            break;
        case 6106:
            seed = CHECKSUM_CIC6106;
            break;
        default:
            return 1;
    }

    t1 = t2 = t3 = t4 = t5 = t6 = seed;

    i = CHECKSUM_START;
    while (i < (CHECKSUM_START + CHECKSUM_LENGTH)) {
        d = CRCBYTES2LONG(&data[i]);
        if ((t6 + d) < t6) t4++;
        t6 += d;
        t3 ^= d;
        r = ROL(d, (d & 0x1F));
        t5 += r;
        if (t2 > d) t2 ^= r;
        else t2 ^= t6 ^ d;

        if (bootcode == 6105) t1 += CRCBYTES2LONG(&data[N64_HEADER_SIZE + 0x0710 + (i & 0xFF)]) ^ d;
        else t1 += t5 ^ d;

        i += 4;
    }
    if (bootcode == 6103) {
        crc[0] = (t6 ^ t4) + t3;
        crc[1] = (t5 ^ t2) + t1;
    }
    else if (bootcode == 6106) {
        crc[0] = (t6 * t4) + t3;
        crc[1] = (t5 * t2) + t1;
    }
    else {
        crc[0] = t6 ^ t4 ^ t3;
        crc[1] = t5 ^ t2 ^ t1;
    }
    return 0;
}

/* Get the checksum of a N64Rom context */
void n64rom_crc ( N64Rom * h, unsigned * dest )
{
    unsigned char * buffer;
    static int init;
    
    /* Gen table? */
    if( !init ) {
        gen_table();
        init = 1;
    }
    
    /* Allocate buffer space */
    buffer = malloc(CHECKSUM_LENGTH + CHECKSUM_START);
    
    /* Read section of ROM used for CRC */
    fseek( h->handle, 0, SEEK_SET );
    fread( buffer, 1, CHECKSUM_LENGTH + CHECKSUM_START, h->handle );
    
    /* Calculate CRC */
    N64CalcCRC( dest, buffer );
    
    /* Free buffer */
    free( buffer );
}
