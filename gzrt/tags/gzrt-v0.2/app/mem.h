/********************
* Memory management *
********************/
#ifndef __GZRT_MEM_H
#define __GZRT_MEM_H

/* Functions */
void   * gzrt_malloc  ( unsigned size );
void   * gzrt_calloc  ( unsigned size );
void     gzrt_free    ( void * pass   );
unsigned gzrt_mem_use ( void          );

#endif /* __GZRT_MEM_H */
