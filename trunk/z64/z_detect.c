/* ~~~~~~~ FILETYPE DETECTION ~~~~~~~ */
#include <gzrt.h>

/* ~~~ MAIN DETECTION ~~~ */
int zd_detect_main ( zfu8 *s, zfu32 length )
{
	zfu8 r;
	if((r=zd_detect_yaz0(s)))
		return r;
	else if((r=zd_detect_zscene(s,length)))
		return r;
	else if((r=zd_detect_zmap(s,length)))
		return r;
	else if((r=zd_detect_zasm(s,length)))
		return r;
	else if((r=zd_detect_zactor(s,length)))
		return r;
	else if((r=zd_detect_zobj(s,length)))
		return r;
	/*else if((r=detect_zgi(s,length))!=false)
		return r;*/ /* uncomment to enable zgi */
	else if((r=zd_detect_ztxt(s,length)))
		return r;
	else    return type_zdata;
}

int zd_detect_main_old (Z64FB *file/*zfu8 *s, zfu32 length*/)
{
	zfu8 r, *s = file->data; zfu32 length = file->length;
	if((r=zd_detect_yaz0(s)))
		return r;
	else if((r=zd_detect_zscene(s,length)))
		return r;
	else if((r=zd_detect_zmap(s,length)))
		return r;
	else if((r=zd_detect_zasm(s,length)))
		return r;
	else if((r=zd_detect_zactor(s,length)))
		return r;
	else if((r=zd_detect_zobj(s,length)))
		return r;
	/*else if((r=detect_zgi(s,length))!=false)
		return r;*/ /* uncomment to enable zgi */
	else if((r=zd_detect_ztxt(s,length)))
		return r;
	else    return type_zdata;
}

/* name detection */
int zd_detect_name_main ( char *D ){
	if(strstr(D,"_room_"))
		return type_zmap;
	else if(strstr(D,"_scene"))
		return type_zscene;
	else if(strstr(D,"ovl_"))
		return type_zactor;
	else if(strstr(D,"_message_"))
		return type_ztxt;
	else if(strstr(D,"code"))
		return type_zasm;
	else if(strstr(D,"Audio"))
		return type_zres;
	else if(strstr(D,"object_"))
		return type_zobj;
	else
		return type_zdata;
}

/* string filetype */
char *string_filetype ( zfu8 type )
{
	static char names[][7] = 
	{
		"", "yaz0", "zmap", "zscene", "zobj", "zactor",
		"zdata", "ztxt", "zasm", "zres", "zgi"
	};
	
	return names[type];
}


/*       *
  *     *  ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
   *   *
    * *
     *     Individual detection functions
    * *
   *   *
  *     *  ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~ ~
 *       */

/* compressed yaz0 blocks */
zfu8 zd_detect_yaz0 (zfu8 *s){
	if(s[0]=='Y'&&s[1]=='a'&&s[2]=='z'&&s[3]=='0')
		return type_yaz0;
	else
		return 0;
}

/* room files */
zfu8 zd_detect_zmap (zfu8 *s, zfu32 length){
	if( (s[0]==0x16&&s[1]<0xF) ||
	    (s[0]==0x18&&s[1]<0xF) )
		return type_zmap;
	else
		return 0;
}

/* scene files */
zfu8 zd_detect_zscene (zfu8 *s, zfu32 length){
		
	if( (s[0]==0x18&&s[1]==0&&s[2]==0&&s[3]==0&&s[4]==2)
	||  (s[0]==0x04&&s[1]==1&&s[2]==0&&s[3]==0&&s[4]==2) 
	||  (s[0]==0x15&&s[1]<0xA) )
		return type_zscene;
	else
		return 0;
}

/* zactor files */
zfu8 zd_detect_zactor (zfu8 *s, zfu32 length){
	/*
	Just a side note. Actors are just compiled code
	like everything else in the game, hence why some
	code blocks are wrongly detected as them.
	*/
	zfu32 i,a; zfu8 amnt=6;
	zfu8 asm_header[16][4] = 
	{
		{0x27,0xBD,0xFD,0xD0},
		{0x27,0xBD,0xFF,0xC8},
		{0x03,0xE0,0x00,0x08},
		{0x3C,0x0E,0x80,0x80},
		{0x3C,0x01,0x00,0x01},
		{0x44,0x80,0x00,0x00}
	};
	
	for(i=0;i<amnt;i++){
		for(a=0;a<4;a++)
			if(asm_header[i][a]!=s[a])
				goto end;
		end:
		if(a==4) return type_zactor;
	}
	
	return 0;
}

/* detect zobj 
zfu8 zd_detect_zobj (zfu8 *s, zfu32 length){
	zfu32 i; zfu8 a; zfu32 amnt=0,enty=0;
	zfu8 entry[]={0xE7,0,0,0};
	for(i=0;i<length;i+=16){
		for(a=0;a<16;a+=8) 
			if(s[i+a]==0x06&&s[i+a+1]<0xFF)
				amnt++;
		for(a=0;a<4;a++)
			if(s[i+a]!=entry[a])
				break;
		if(a==4) enty++;
	}
	return ((amnt>=8&&enty>2)?type_zobj:0);
}*/

zfu8 zd_detect_zobj (zfu8 *s, zfu32 length)
{ 
	zfu32 i = 0;
	
	if( !length )
		return 0;
	
	for(i=0;i<length-8;i+=8){ 
		if(s[i] == 0x01 && s[i+4] == 0x06 && (s[i+8] == 0x06 || s[i+8] == 0x05 )) 
			goto found;
	} 
	return 0; 
	found: return type_zobj;
}

/* detect blocks of text */
zfu8 zd_detect_ztxt (zfu8 *s, zfu32 length){
	zfu32 a,amnt=0; float calc;
	for(a=0;a<length;a++)
		if(s[a]<='~'&&s[a]>=' ')
			amnt++;
	calc=(float)amnt/(float)length;
	return ((calc>=0.8)?type_ztxt:0);
}

/* basically, just detect the game's main code block */
zfu8 zd_detect_zasm (zfu8 *s, zfu32 length){
	zfu8 code_block[] = {
	/*0x15, 0x50, 0x1B, 0x84, 0x12, 0x08, 0x10, 0xE8, 0x01, 0xE0, 0x01, 0xF0, 0x00, 0xF8, 0x02, 0xB0, */
	0x00, 0x10, 0x20, 0x00, 0xC0, 0x00, 0x08, 0x00, 0x00, 0x40, 0x00, 0x02, 0x00, 0x01, 0x00, 0x01, 
	0x67, 0x0A, 0x58, 0x24, 0xB6, 0xE3, 0xC5, 0xE3, 0x0F, 0xF0, 0xFF, 0xFF, 0x02, 0x00, 0x08, 0x00, 
	0x18, 0xF9, 0x82, 0x76, 0x6A, 0x6E, 0xB8, 0xE3, 0x47, 0x1D, 0x7D, 0x8A, 0x00, 0x00, 0x00, 0x00, 
	0x5A, 0x82, 0xA5, 0x7E, 0x30, 0xFC, 0x89, 0xBE, 0x76, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x18, 0xF9, 0x6A, 0x6E, 0xB8, 0xE3, 0x82, 0x76, 0x47, 0x1D, 0x18, 0xF9, 0x82, 0x76, 0x6A, 0x6E, 
	0x6A, 0x6E, 0x82, 0x76, 0xE7, 0x07, 0xB8, 0xE3, 0x7D, 0x8A, 0x47, 0x1D, 0x6A, 0x6E, 0x18, 0xF9
	};
	zfu32 i,l=sizeof(code_block); if(length<l) goto end;
	if(length < 500*1024) return 0;
	for(i=0;i<l;i++)
		if(code_block[i]!=s[length-l+i])
			break;
	end:
	return ((i==l)?type_zasm:0);
}
