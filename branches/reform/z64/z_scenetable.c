/***************************
* Scene Table Modification *
***************************/
#include <gzrt.h>

/*
	Scene table detection for Majora's Mask
*/

Z64ST_MM *z64st_mm_init_r ( zfu8 *block, zfu32 length ){
	
	/* Variables */
	Z64ST_MM *f;
	zfu32 i, a;
	zfu8 KEY_MM_STAR[] = __ST_KEY_MM_STAR,
	KEY_MM_END[] = __ST_KEY_MM_END;
	
	/* Set up the return handle */
	f = (Z64ST_MM*)malloc(sizeof(Z64ST_MM));
	
	/* Loop through the code block in search */
	for( i = 0; i < length; i += 16 ){
		
		/* Check with loop */
		for( a = 0; a < sizeof(KEY_MM_STAR)-1; a++ ){
			
			/* Check for inconsistency */
			if(KEY_MM_STAR[a] != block[i+a])
				goto nofound1;
		}
		
		/* Result found */
		f->t_start = i + 0x70;
		break;
		
		/* Jack shit */
		nofound1:
		continue;
	}
	
	/* Detect end */
	for( ; i < length; i += 16 ){
		
		/* Compare with loop */
		for( a = 0; a < sizeof(KEY_MM_END)-1; a++ ){
			
			/* Check for inconsistency */
			if(KEY_MM_END[a] != block[i+a])
				goto nofound2;
		}
		
		/* Set ending */
		f->t_end = i;
		break;
		
		/* Got nothing */
		nofound2:
		continue;
	}
	
	/* Check for overrun */
	if(i == length)
		return NULL;
	
	/* Copy the table into handle */
	f->table = (zfu8*)malloc(f->t_end-f->t_start);
	memcpy(f->table, block + f->t_start, f->t_end-f->t_start);
	
	/* Final product -> */
	return f;
}

/*
	Scene table detection for Ocarina of Time
*/
Z64ST_OOT *z64st_oot_init_r ( zfu8 *block, zfu32 length ){
	
	/* Variables */
	Z64ST_OOT *f;
	zfu32 i, a, t1, t2;
	zfu8 key1[] = __ST_KEY_OOT_STAR;
	
	/* Set up handle */
	f = calloc(sizeof(Z64ST_OOT),1);
	
	/* Loop through code block */
	for( i = 0; i < length; i += 16 ){
		
		/* Sub loop */
		for( a = 0; a < sizeof(key1)-1; a++ ){
			
			/* Check for inconsistency */
			if(key1[a] != block[i+a])
				goto nfound;
		}
		
		/* Got one! */
		f->t_start = (i += 0x20);
		break;
		
		/* Got nothing */
		nfound:
		continue;
	}
	
	/* Detect the ending */
	for( ; i < length-20; i += 20 ){
		
		t1 = ZFS_BYTES2LONG(block + i);
		t2 = ZFS_BYTES2LONG(block + i + 4);
		
		if(!(t1 < (1<<26) && t2 < (1<<26))){
			f->t_end = i;
			goto cend;
		}
	}
	
	/* Check */
	cend:
	if(i == length)
		return NULL;
	
	/* Copy the table into handle */
	f->table = malloc(f->t_end-f->t_start);
	/*printf("%08X %08X %08X",f->t_end-f->t_start,f->t_end,f->t_start);
	getchar();*/
	memcpy(f->table, block + f->t_start, f->t_end-f->t_start);
	
	/* Return final product */
	return f;
}


	/*
	typedef struct {
	zfu8 *table;
	zfu32 t_start;
	zfu32 t_end;
	zfu16 pos;
	zfu32 off1;
	zfu32 off2;
	zfu32 timg_start;
	zfu32 timg_end;
	zfu32 flag;
	} Z64ST_OOT; 
	*/

Z64ST_MM * z64st_init ( char * filename, zfu8 ram){
	
	/* Variables */
	FILE *handle;
	static Z64ST_MM *f;
	zfu32 i, a, filesize;
	zfu8 KEY_MM_STAR[] = __ST_KEY_MM_STAR, *buff,
	KEY_MM_END[] = __ST_KEY_MM_END;
	zfu8 *read_buffer = (zfu8*)malloc(ZF_MAX(sizeof(KEY_MM_END),sizeof(KEY_MM_STAR)));
	
	/* Set */
	f = (Z64ST_MM*)malloc(sizeof(Z64ST_MM));
	if(!(handle = fopen(filename,"rb")))
		ZERROR("Cannot open file \"%s\" for reading. The error returned was:\n%s\n",filename,strerror(errno));
	ZF_FILESIZE(filesize,handle);
	
	/* Load file into RAM if necessary */
	if(ram==1){
		fseek(handle,0,SEEK_SET);
		buff = (zfu8*)malloc(filesize);
		fread(buff,1,filesize,handle);
	}
	
	/* Loop through and begin search */
	for(i=0; i < filesize; i+= 16){
		
		/* Fill buffer printf("%i\n",sizeof(KEY_MM_STAR));
		fread(read_buffer,1,sizeof(KEY_MM_STAR),handle);*/
		
		/* Check */
		for(a = 0; a < sizeof(KEY_MM_STAR)-1; a++){
			/*if(i == 0xC764A0UL)
				printf("%02X|%02X\n",KEY_MM_STAR[a],buff[i+a]);*/
			if(KEY_MM_STAR[a]!=buff[i+a])
				break;
		}
		if(a == sizeof(KEY_MM_STAR)-1){
			f->t_start = i + 0x70;
			break;
		}
	}
	
	/* Detect end 
	fseek(handle,i,SEEK_SET);*/
	for(; i < filesize; i+= 16){
		
		/* Fill buffer 
		fread(read_buffer,1,16,handle);*/
		
		/* Compare */
		for(a = 0; a < sizeof(KEY_MM_END)-1; a++){
			
			/*if(i == 0xC76C20UL)
				printf("%02X|%02X\n",KEY_MM_END[a],read_buffer[a]);*/
			if(KEY_MM_END[a] != buff[i+a])
				break;
		}
		if(a == sizeof(KEY_MM_END)-1){
			f->t_end = i;
			break;
		}
	}
	
	/* Check */
	if(i == filesize)
		return NULL;
	
	/* Read the table into RAM */
	f->table = (zfu8*)malloc(f->t_end-f->t_start);
	fseek(handle,f->t_start,SEEK_SET);
	fread(f->table,1,f->t_end-f->t_start,handle);
	fclose(handle);
	
	/* Return final product */
	return f;
}

void z64st_mm_dump (Z64ST_MM *h){
	printf("Table point.:\t$%08X\n"
		"Table start:\t$%08X\n"
		"Table end:\t$%08X\n"
		"Table pos.:\t$%08X\n"
		"Cur.Offset1:\t$%08X\n"
		"Cur.Offset2:\t$%08X\n"
		"Cur.Flags:\t$%016X\n",
	h->table,h->t_start,h->t_end,h->pos,h->off1,h->off2,h->flags);
}

void z64st_mm_read_next (Z64ST_MM *h){
	
	z64st_mm_read_arbitrary(h, h->pos/16+1);
}

void z64st_oot_read_next (Z64ST_OOT *h){
	
	z64st_oot_read_arbitrary(h, h->pos/20);
}

void z64st_mm_read_arbitrary (Z64ST_MM *h, zfu16 id){
	
	
	h->off1=ZFS_BYTES2LONG(h->table+(id*16));
	h->off2=ZFS_BYTES2LONG(h->table+(id*16)+4);
	h->flags=ZFS_BYTES2LONG(h->table+(id*16)+8);
	h->flags=(h->flags<<32);
	h->pos+=16;
}

void z64st_oot_read_arbitrary (Z64ST_OOT *h, zfu16 id){
	
	h->off1 = ZFS_BYTES2LONG(h->table + (id*20));
	h->off2 = ZFS_BYTES2LONG(h->table + (id*20) + 4);
	h->timg_start = ZFS_BYTES2LONG(h->table + (id*20) + 8);
	h->timg_end   = ZFS_BYTES2LONG(h->table + (id*20) + 12);
	h->flag = ZFS_BYTES2LONG(h->table + (id*20) + 16);
	h->pos += 20;
}

void z64st_mm_dump_small (Z64ST_MM *h){
	/*
	typedef struct {
	zfu8 *table;
	zfu32 t_start;
	zfu32 t_end;
	zfu16 pos;
	zfu32 off1;
	zfu32 off2;
	zfu64 flags;
} Z64ST_MM;
	*/
	
	printf("[%04i|$%08X-$%08X|$%016X]\n",
	h->pos/16,h->off1,h->off2,h->flags);
}

void z64st_oot_dump_small (Z64ST_OOT *h){
	printf("[%04i|$%08X-$%08X|$%08X-$%08X|$%08X?]\n",h->pos/20,
	h->off1,h->off2,h->timg_start,h->timg_end,h->flag);
}

void z64st_oot_close (Z64ST_OOT *h){
	free(h->table);
}
