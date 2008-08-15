#include <gzrt.h>

N64ROM * rom_context;
Z64FS  * fs_context;

int main ( int argc, char **argv )
{
	gtk_init( &argc, &argv );
	/* gzrt_wsplash_init( gzrt_wsplash_close ); */
	gzrt_gui_init( argc, argv );
	gtk_main(  );
}
