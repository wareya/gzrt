/*************************
* ZACTOR TABLE DETECTION *
*************************/
#include <gzrt.h>

/*
   The rundown of what we're looking for; OOT:
   0000 0000 0000 0000 0000 0000 0000 0000 
   0000 0000 8012 AF90 8013 7040 0000 0000
   MM:
   0000 0000 0000 0000 0000 0000 0000 0000 
   0000 0000 801C FDD0 0000 0000 0000 0000
*/

/*
00E54EB0 00E55050 80A9EE40 80A9EFE0 
00000000 80A9EF60 801373C8 00020000
	 asm ?    asm ?   type^
*/	

/*
	This function should be universal...
*/
Z64AT *zat_init ( zfu8 *p, zfu32 length )
{
	
	/* Variables */
	Z64AT *h;
	zfu32 i,a,*d;
	
	/* Detect... */
	for(i = 0; i < length - 0x30; i+=0x10){
		if(ZFS_BYTES2LONG(p+i) == 0xDF000000UL && !ZFS_BYTES2LONG(p+4+i)){
			if(ZFS_BYTES2LONG(p+i+16) > (1<<26))
				i+=0x20;
			for(a=0; a<5; a++)
				if(ZFS_BYTES2LONG(p+a*4+16+i)){
					goto next;
				}
			if(!ZFS_BYTES2LONG(p+a*4+16+i))
				goto next;
			goto found;
		}	
		next: continue;
	}
	
	/* Check */
	if(i == length - 0x30)
		return NULL;
	
	/* Ok, now we has. */
	found:
	h = (Z64AT*)malloc(sizeof(Z64AT));
	h->start = (i += 0x10);
	
	/* Find ending */
	for(; i < length - 0x30; i+=0x20){
		if(!(ZFS_BYTES2LONG(p+i+20)>0x80000000UL||(!ZFS_BYTES2LONG(p+i) && !ZFS_BYTES2LONG(p+i+4) &&!ZFS_BYTES2LONG(p+i+20))))
			goto endfound;
	}
	
	/* Damn, got nothing... */
	if(i == length){
		free(h);
		return NULL;
	}
		
	/* kay */
	endfound:
	h->end = i;
	
	/* Read all of the entries into RAM */
	h->t = (Z64AT_E*)malloc(sizeof(Z64AT_E)*z64at_num_entries(h));
	for(i = 0; i < z64at_num_entries(h); i++){
		for(d = (zfu32*)&h->t[i], a = 0; a < 7; a++)
			d[a] = ZFS_BYTES2LONG(p+h->start+i*32+a*4);
		h->t[i].type = ZFS_BYTES2LONG(p+h->start+i*32+a*4)>>16;
		h->t[i].unk2 = ZFS_BYTES2LONG(p+h->start+i*32+a*4)&0xFFFF;
	}
	
	return h;
}

void zat_dump_handle ( Z64AT *h )
{
	int i;
	printf("$%08X -> $%08X; %u entries (%u valid). First batch:\n",
	h->start,h->end,z64at_num_entries(h),zat_count_valid(h));
	for(i=0;i<z64at_num_entries(h);i++)
		printf("h->t[%u] = {0x%08X, 0x%08X}\n",i,h->t[i].s,h->t[i].e);
	printf(". . .\n");
}

zfu32 zat_count_valid ( Z64AT *h )
{
	int i,v;
	for(i=0,v=0;i<z64at_num_entries(h);i++)
		if(h->t[i].s&&h->t[i].e)
			v++;
	return v;
}
