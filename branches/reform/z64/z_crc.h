#ifndef __CRC_H
#define __CRC_H

#define ROL(i, b) (((i) << (b)) | ((i) >> (32 - (b))))
#define CRCBYTES2LONG(b) ( (b)[0] << 24 | \
                        (b)[1] << 16 | \
                        (b)[2] <<  8 | \
                        (b)[3] )

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

void gen_table ();
unsigned int crc32 (unsigned char *data, int len);
int N64GetCIC (unsigned char *data);
int N64CalcCRC (unsigned int *crc, unsigned char *data);
void fix_crc (char *filename);

#endif
