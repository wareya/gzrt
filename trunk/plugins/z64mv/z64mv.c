#include <stdio.h>
#include <stdlib.h>
#include "lz64mv/z64mv.h"

#include "gzrtplugin.h"

int view ( struct PluginTransac * t   );

/* Plugin information */
struct PluginMeta gzrt_plugin_info =
{
	"Model Viewer",
	"Zelda 64 Model Viewer",
	"ZZT32",
	"64.vg",
	"prelim",
	
	/* Description */
	NULL,
	
	/* Init, menu & file action funcs */
	NULL, NULL, view
};

int view ( struct PluginTransac * t )
{
	struct PluginFileSpec * k = t->file;
	printf( "%08X\n", z64mv_create_from_bin( k->file, k->filesize ) );
}
