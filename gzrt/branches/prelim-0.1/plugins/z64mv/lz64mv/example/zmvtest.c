#include <stdio.h>
#include <z64mv.h>
#include <unistd.h>

int main ( int argc, char **argv )
{
    Z64MV * h = z64mv_create_from_file( argv[1] );
    
    while( 1 )
        sleep( 1 );
}
