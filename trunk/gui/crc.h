#ifndef __GZRT_WCRC_H
#define __GZRT_WCRC_H

struct crcarg
{
	MAINWIN *parent;
	unsigned int *crc;
};

void gzrt_wcrc_create ( MAINWIN *parent );
GtkWidget * create_CRC_Checker ( MAINWIN *parent );
void gzrt_wcrc_write ( struct crcarg *c, GtkWidget *w );
#endif /* __GZRT_WCRC_H */
