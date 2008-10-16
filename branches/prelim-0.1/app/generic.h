#ifndef __GZRT_GENERIC_H
#define __GZRT_GENERIC_H
void trim     ( char *s );
int is_dir    ( char *n );
int can_write ( char *n );
int stringcmp ( char *s, char *e );
double time_since_start ( void );
#endif
