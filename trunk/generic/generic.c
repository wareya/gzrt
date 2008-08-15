/*****************************
* Generic Functions for GZRT *
*****************************/
#include <gzrt.h>

void trim ( char *s )
{
	int l;
	
	for( l = strlen(s) - 1; s[l] == ' '; l-- );
	s[l + 1] = 0;
}

int is_dir ( char *n )
{
	DIR *h;
	
	if( !(h = opendir(n)) )
		return 0;
	else {
		closedir( h );
		return 1;
	}
}

int can_write ( char *n )
{
	FILE *h;
	
	if( !(h = fopen(n, "rb+")) )
		return 0;
	else {
		fclose(h);
		return 1;
	}
}

/* String compare; case insensitive (Windows) */
int stringcmp ( char *s, char *e )
{
	while( tolower(*s) == tolower(*e) )
	{
		if( !*s | !*e )
			break;
		
		s++;
		e++;
	}
	
	return tolower(*s) - tolower(*e);
}
