#include <gzrt.h>

/* ~~~~~~~~~~~~~~~~~ */

Z64FS * z64fs_init (char *file){
	
	/* Variables */
	Z64FS * fs_handle;
	FILE *handle;
	zfu32 filesize, i, a;
	zfu8 buffer[16], compare[]="zelda@", c,
	len = strlen(compare);
	
	/* Init filesystem */
	fs_handle = (Z64FS*)calloc(sizeof(Z64FS),1);
	fs_handle->init        = 0x64;
	fs_handle->fs_creator = malloc(32);
	fs_handle->fs_date    = malloc(32);
	fs_handle->fs_time    = malloc(32);
	
	/* Open file */
	if(!(handle=fopen(file,"rb")))
		exit(0);
	ZF_FILESIZE(filesize,handle);
	
	/* Detect start */
	for(i = 0; i < filesize; i += 16){
		
		/* Read next 16 bytes of ROM */
		fread(buffer,1,16,handle);
		buffer[len]=0;
		
		/* Check for matching string */
		if(strncmp(compare,buffer,sizeof(compare))==0){
			
			/* Set starting position */
			i += 0x30;
			fs_handle->rstart = i;
			break;
		}
		
		/* Check for EOF reach */
		if(i+16 >=filesize ){
			
			/* Die */
			return NULL;
		}
	}
	
	/* Read ROM creator */
	fseek(handle, fs_handle->rstart-0x30, SEEK_SET);
	for(a = 0; (fs_handle->fs_creator[a]=fgetc(handle))!=0; a++);
	while((c=fgetc(handle))==0);
	for(a = 1, fs_handle->fs_date[0]=c;(fs_handle->fs_date[a]=fgetc(handle))!=' '; a++);
	for(fs_handle->fs_date[a] = 0,a = 0; (fs_handle->fs_time[a]=fgetc(handle))!=0; a++);
	fseek(handle, fs_handle->rstart, SEEK_SET);
	
	for( ; i < filesize; i+=16){
		
		/* Read next 16 bytes of ROM */
		fread(buffer,1,16,handle);
		
		/* Check for 16 null bytes */
		for(a=0;a<16;a++)
			if(buffer[a]!=0) break;
		if(a == 16){
			
			/* Set end */
			fs_handle->rend = i;
			break;
		}
		
		/* Check for EOF reach */
		if(i+16 >=filesize ){
			
			/* Die */
			return NULL;
		}
	}
	
	/* Read into RAM */
	if(!(fs_handle->fs_table = (zfu8*)malloc(fs_handle->rend-fs_handle->rstart)))
		ZERROR("Not enough memory to store file offset table (%.2fkb required).\n",
		(float)(fs_handle->rend-fs_handle->rstart)/1024.0);
	fseek(handle,fs_handle->rstart,SEEK_SET);
	fread(fs_handle->fs_table,1,fs_handle->rend-fs_handle->rstart,handle);
	fclose(handle);
	
	/* Return final product */
	return fs_handle;
}


/*
	Detailed handle information dump
*/
void z64fs_dump_handle (Z64FS *fs_handle){
	
	/* Dump info */
	printf("\n=> FS Information Dump <="
	"\nFile table start: $%08lX\n"
	"File table end  : $%08lX\n"
	"Num. entries    : %04lu\n"
	"Current position: $%04lX (item %lu)\n"
	"ROM Creator     : %s\n"
	"Creation date   : %s %s\n"
	"Current entry:\n"
	"    Offset 1:     $%08lX\n"
	"    Offset 2:     $%08lX\n"
	"    Offset 3:     $%08lX\n"
	"    Offset 4:     $%08lX\n\n",
	fs_handle->rstart,fs_handle->rend,
	((fs_handle->rend)-(fs_handle->rstart))/16,
	fs_handle->pos,fs_handle->pos/16,
	fs_handle->fs_creator,fs_handle->fs_date, fs_handle->fs_time,
	fs_handle->file_start,fs_handle->file_end,
	fs_handle->file_vstart,fs_handle->file_vend);
}


/*
	Print some information about file entry
*/
void z64fs_dump_file_info (Z64FS *handle){
	
	/* Print */
	printf("#%04u [$%08lX - $%08lX]:[$%08lX - $%08lX] | %.2fkb\n",
	handle->pos/16,handle->file_start,handle->file_end,
	handle->file_vstart,handle->file_vend,
	(float)(handle->file_end - handle->file_start)/1024.0);
}


/*
	Read arbitrary file entry
*/
void z64fs_read_arbitrary (Z64FS *fs_handle, zfu16 id){
	
	zfu8 * cpos; 
	
	/* Check for overflow */
	if(id*16>fs_handle->rend-fs_handle->rstart)
		ZERROR("Filesystem overflow, application read too far ahead!");
	
	/* Read four offsets */
	cpos = fs_handle->fs_table+(id*16);
	fs_handle->file_start  = ZFS_BYTES2LONG(cpos);
	fs_handle->file_end    = ZFS_BYTES2LONG(cpos+4);
	fs_handle->file_vstart = ZFS_BYTES2LONG(cpos+8);
	fs_handle->file_vend   = ZFS_BYTES2LONG(cpos+12);
}


/*
	Read next file entry
*/
void z64fs_read_next (Z64FS *fs_handle){
	
	/* Read next four offsets */
	z64fs_read_arbitrary(fs_handle,fs_handle->pos/16);
	
	/* Update position */
	fs_handle->pos += 16;
}


/*
	Set seeker back by amount positions
	~ If read_bool == true, read offsets
*/	
void z64fs_rewind (Z64FS *handle, zfu16 amount, zfu8 read_bool){
	
	/* Set position */
	handle->pos -= (amount+1)*16;
	
	/* Read if applicable */
	if(read_bool)
		z64fs_read_next(handle);
}


/*
	Close Z64FS handle and free memory
*/
void z64fs_close_handle (Z64FS *handle){
	
	/* Free memory */
	free(handle->fs_table);
	free(handle->fs_creator);
	free(handle->fs_date);
	free(handle->fs_time);
	memset(handle,0,sizeof(Z64FS));
}


/*
	Calculate filesystem size, decompressed
*/
zfu32 z64fs_calc_size_decompressed (Z64FS *handle){
	
	zfu32 final_size = 0, i, a, o[4];
	
	for(i = 0; i < z64fs_num_entries(handle) * 16; i += 16){
		
		for(a = 0; a < 2; ++a)
			o[a] = ZFS_BYTES2LONG(handle->fs_table+i+a*4);
		final_size += o[1]-o[0];
	}
	
	return final_size;
}


/*
	Calculate filesystem size, compressed
*/
zfu32 z64fs_calc_size_compressed (Z64FS *handle){
	
	zfu32 final_size = 0, i, a, o[4];
	
	for(i = 0; i < z64fs_num_entries(handle) * 16; i += 16){
		
		for(a = 0; a < 4; ++a)
			o[a] = ZFS_BYTES2LONG(handle->fs_table+i+a*4);
		final_size += (o[3]?o[3]-o[2]:o[1]-o[0]);
	}
	
	return final_size;
}
