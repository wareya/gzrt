#include <stdio.h>
#include "../z64fs.h"

int main ( int argc, char ** argv )
{
	Z64FS * f = z64fs_open( fopen(argv[1], "rb") );
	printf( "%08X\n", f->start );
}
