#ifndef __ZACTORT_H
#define __ZACTORT_H
#include "z64lib.h"

struct Z64AT_E {
	zfu32 s;
	zfu32 e;
	zfu32 a1;
	zfu32 a2;
	zfu32 unk1;
	zfu32 a3;
	zfu32 a4;
	zfu16 type;
	zfu16 unk2;
};

struct Z64AT {
	struct Z64AT_E *t;
	zfu32 start;
	zfu32 end;
};

typedef struct Z64AT Z64AT;
typedef struct Z64AT_E Z64AT_E;

#define z64at_num_entries(x) (((x)->end-(x)->start)/32)
Z64AT *zat_init ( zfu8 *p, zfu32 length );
void zat_dump_handle ( Z64AT *h );
zfu32 zat_count_valid ( Z64AT *h );

#endif /* __ZACTORT_H */
