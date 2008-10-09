#include <gzrt.h>

/* Time at program start */
struct timeval program_start;

int main ( int argc, char **argv )
{
	gtk_init( &argc, &argv );
	gzrt_init( argc, argv );
	gtk_main( );
}

/* Initialize the application */
void gzrt_init ( int argc, char **argv )
{	
	/* Set the application start time */
	gettimeofday( &program_start, NULL );
	
	/* Initialize the GUI */
	gzrt_gui_init( argc, argv );
}
