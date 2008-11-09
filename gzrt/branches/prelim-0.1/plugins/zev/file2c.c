/****************************
* Convert input to C source *
****************************/
#include <stdio.h>

#define LINE    16

/* file2c: arrayname file */

int main ( int argc, char **argv )
{
    int c, cnt = 0;
    FILE * handle;
    
    printf( "#include <stdio.h>\nunsigned char %s[] =\n{\n\t", argv[1] );
    
    if( !(handle = fopen(argv[2], "rb")) )
        return 0;
    fseek( handle, 0, SEEK_SET );
    
    while( (c = fgetc(handle)) != EOF )
    {
        
        printf( "0x%02X, ", c );
        
        if( !((cnt+1)%LINE) )
            printf("\n\t");
        
        cnt++;
    }
    
    printf( "\n};\nint  exesize = sizeof(executable);\n" );
    
    return 0;
}
