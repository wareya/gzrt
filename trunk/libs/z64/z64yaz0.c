/********************************
* Zelda 64 Compression Handling *
********************************/
#include <stdio.h>
#include <z64yaz0.h>

void z64yaz0_decode (unsigned char * src, unsigned char * dst, int uncompressedSize)
{
  int srcPlace = 0, dstPlace = 0; /*current read/write positions*/
  
  unsigned int validBitCount = 0; /*number of valid bits left in "code" byte*/
  unsigned char currCodeByte;
  while(dstPlace < uncompressedSize)
  {
    /*read new "code" byte if the current one is used up*/
    if(validBitCount == 0)
    {
      currCodeByte = src[srcPlace];
      ++srcPlace;
      validBitCount = 8;
    }
    
    if((currCodeByte & 0x80) != 0)
    {
      /*straight copy*/
      dst[dstPlace] = src[srcPlace];
      dstPlace++;
      srcPlace++;
    }
    else
    {
      /*RLE part*/
      unsigned char byte1 = src[srcPlace];
      unsigned char byte2 = src[srcPlace + 1];
      srcPlace += 2;
      
      unsigned int dist = ((byte1 & 0xF) << 8) | byte2;
      unsigned int copySource = dstPlace - (dist + 1);

      unsigned int numBytes = byte1 >> 4;
      if(numBytes == 0)
      {
        numBytes = src[srcPlace] + 0x12;
        srcPlace++;
      }
      else
        numBytes += 2;

      /*copy run*/
      int i;
      for(i = 0; i < numBytes; ++i)
      {
        dst[dstPlace] = dst[copySource];
        copySource++;
        dstPlace++;
      }
    }
    
    /*use next bit from "code" byte*/
    currCodeByte <<= 1;
    validBitCount-=1;    
  }
}
