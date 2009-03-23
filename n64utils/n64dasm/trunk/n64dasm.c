/*******************
* N64 Disassembler *
*******************/
#include <stdio.h>
#include "decoder.h"

int main ( int argc, char ** argv )
{
	static unsigned char buffer[4], dasm[512];
	unsigned w, pc = 0;
	
	while( !feof(stdin) )
	{
		fread( buffer, 1, 4, stdin );
		w = buffer[0] << 24 | buffer[1] << 16 | buffer[2] << 8 | buffer[3];
		mr4kd_disassemble( w, pc, dasm );
		pc += 4;
		
		printf( "%08X: %s\n", pc, dasm );
		fflush( stdout );
	}
	
	return 0;
}
