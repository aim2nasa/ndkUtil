#include <iostream>
#include <stdio.h>
#include <zlib.h>
#include <assert.h>

typedef unsigned char 	_u8;
typedef unsigned short 	_u16;
typedef unsigned int 	_u32;
typedef signed char 	_s8;
typedef signed short 	_s16;
typedef signed int 		_s32;

#include "fbinfo.h"

#define DDMS_RAWIMAGE_VERSION 1

int _get_raw_buffer(_u8* p,_u32 fbiSize,FILE* fp);
int _get_surface_info(fbinfo& fbi,const int width, const int height, const int format);

using namespace std;

int main(int argc,char* argv[])
{
	FILE* fp = popen("screencap","r");
	if(!fp){
		cout<<"popen() error"<<endl;
		return -1;
	}

	int ret,w,h,f;
	ret = fread(&w,1,sizeof(w),fp);
	ret = fread(&h,1,sizeof(h),fp);
	ret = fread(&f,1,sizeof(f),fp);
	cout<<" -w:"<<w<<",h:"<<h<<",f"<<f<<endl;
	
	struct fbinfo fbi;
	if(_get_surface_info(fbi,w, h, f)){
		cout<<"unknown format"<<endl;
		return -1;
	}	
	cout<<" -bpp:"<<fbi.bpp<<",size:"<<fbi.size<<",width:"<<fbi.size<<",height:"<<fbi.height<<endl;
	cout<<" -red_offset:"<<fbi.red_offset<<",red_length:"<<fbi.red_length<<endl;
	cout<<" -green_offset:"<<fbi.green_offset<<",green_length:"<<fbi.green_length<<endl;
	cout<<" -blue_offset:"<<fbi.blue_offset<<",blue_length:"<<fbi.blue_length<<endl;
	cout<<" -alpha_offset:"<<fbi.alpha_offset<<",alpha_length:"<<fbi.alpha_length<<endl;

	_u8* raw_buffer = new _u8[fbi.width*fbi.height*(fbi.bpp/8)];
	int nRead = _get_raw_buffer(raw_buffer,fbi.size,fp);
	assert(nRead==fbi.size);
	cout<<"get_raw_buffer read:"<<nRead<<endl;
	delete [] raw_buffer;

	pclose(fp);
	cout<<"capture end"<<endl;
	return 0;
}

int _get_raw_buffer(_u8* p,_u32 fbiSize,FILE* fp)
{
	static const _u32 buff_size = 1 * 1024;
	int fb_size = fbiSize ;
	int total_read = 0;
	int read_size = buff_size;
	int ret;
	_u8 buff[buff_size];	
	while( 0 < (ret = fread(buff, 1, read_size, fp)) ) {
		total_read += ret;
		memcpy(p, buff, ret);
		p+=ret;
		if(fb_size - total_read < buff_size)
			read_size = fb_size - total_read;
	}
	return total_read;
}

int _get_surface_info(fbinfo& fbi,const int width, const int height, const int format)
{
	/* see hardware/hardware.h */
	switch(format)
	{
		case 1: /* RGBA_8888 */
			fbi.bpp = 32;
			fbi.size = width * height * 4;
			fbi.red_offset = 0;		fbi.red_length = 8;
			fbi.green_offset = 8;	fbi.green_length = 8;
			fbi.blue_offset = 16;	fbi.blue_length = 8;
			fbi.alpha_offset = 24;	fbi.alpha_length = 8;
			break;
		case 2: /* RGBX_8888 */
			fbi.bpp = 32;
			fbi.size = width * height * 4;
			fbi.red_offset = 0;		fbi.red_length = 8;
			fbi.green_offset = 8;	fbi.green_length = 8;
			fbi.blue_offset = 16;	fbi.blue_length = 8;
			fbi.alpha_offset = 24;	fbi.alpha_length = 0;
			break;
		case 3: /* RGB_888 */
			fbi.bpp = 24;
			fbi.size = width * height * 3;
			fbi.red_offset = 0;		fbi.red_length = 8;
			fbi.green_offset = 8;	fbi.green_length = 8;
			fbi.blue_offset = 16;	fbi.blue_length = 8;
			fbi.alpha_offset = 24;	fbi.alpha_length = 0;
			break;
		case 4: /* RGB_565 */
			fbi.bpp = 16;
			fbi.size = width * height * 2;
			fbi.red_offset = 11;	fbi.red_length = 5;
			fbi.green_offset = 5;	fbi.green_length = 6;
			fbi.blue_offset = 0;	fbi.blue_length = 5;
			fbi.alpha_offset = 0;	fbi.alpha_length = 0;
			break;
		case 5:
			fbi.bpp = 32;
			fbi.size = width * height * 4;
			fbi.red_offset = 16;	fbi.red_length = 8;
			fbi.green_offset = 8;	fbi.green_length = 8;
			fbi.blue_offset = 0;	fbi.blue_length = 8;
			fbi.alpha_offset = 24;	fbi.alpha_length = 8;
			break;
		default: /* unknown type */
			return -1;
	}
	fbi.version = DDMS_RAWIMAGE_VERSION;
	fbi.width = width;		
	fbi.height = height;

	return 0;
}
