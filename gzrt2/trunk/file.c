/*****************************
* GZRT File (ROM) Management *
*****************************/
#include <stdio.h>
#include <glib.h>
#include <n64rom.h>
#include <z64.h>
#include "gzrt.h"

/* List of files that are open */
static GList * files;

/* Open a new ROM */

