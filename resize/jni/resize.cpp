#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef unsigned char 	_u8;
typedef unsigned short 	_u16;
typedef unsigned int 	_u32;
typedef signed char		_s8;
typedef signed short	_s16;
typedef signed int		_s32;

#define BUFFER_SIZE			1024
#define DEFAULT_THRESHOLD	589824	//576x1024
#define OUT_BYTE_PER_PIXEL	3

int resize_raw_image(_u8* raw_buffer,_u8* dest,int nBpp,int nWidth,int nHeight,int resize_w,int resize_h,_u32 out_pixel_per_bytes);
int conv_image_bpp(_u8* src, _u8* dest, const _u16 w, const _u16 h, _u32 src_bpp, _u32 dest_bpp);
void method_0(int nThreshold,int nBpp,int nWidth,int nHeight,int out_pixel_per_bytes,_u8* resize_buffer,_u8* raw_buffer);

using namespace std;

int main(int argc,char* argv[])
{
	if(argc<7) {
		cout<<"usage: resize <Bytes per Pixel> <width> <height> <source raw file> <resized raw file> <method (0 or 1)><threshold(opt)>"<<endl;
		cout<<"ex. resize 4 1440 2560 dump.raw resized.raw"<<endl;
		cout<<"    threshold is calculated as width x height, default is 576x1024=589824"<<endl;
		cout<<"    method 0: resizing through threshold value and neighborhood algorithm"<<endl;
		cout<<"    method 1: resizing by changing Bytes per Pixel from 4bytes to 3bytes(only works for Bytes per Pixel is 4)"<<endl;
		cout<<"Copyright© by Funzin"<<endl<<endl;
		return -1;
	}
	
	int nBpp = atoi(argv[1]);
	int nWidth = atoi(argv[2]);
	int nHeight = atoi(argv[3]);	
	int nThreshold = DEFAULT_THRESHOLD;
	int nMethod = atoi(argv[6]);
	if(argc>7) nThreshold = atoi(argv[7]);
	cout<<"Bytes per Pixel:"<<nBpp<<",width:"<<nWidth<<",height:"<<nHeight<<",method:"<<nMethod<<",threshold:"<<nThreshold<<endl;
	cout<<"source raw file size:"<<nBpp*nWidth*nHeight<<"bytes"<<endl;
	
	if(nMethod==1 && nBpp!=4) {
		cout<<"Unsupported raw format"<<endl;
		return -1;
	}
	
	FILE* fp = fopen(argv[4],"rb");
	if(!fp) {
		cout<<"file open failure"<<argv[1]<<endl;
		return -1;
	}
	
	int nReadUnit = BUFFER_SIZE;
	int nRead=0,nTotalRead = 0;
	_u8 buffer[BUFFER_SIZE];
	_u8* resize_buffer = new _u8[nWidth*nHeight*OUT_BYTE_PER_PIXEL];
	while( 0 < (nRead = fread(buffer, 1, nReadUnit, fp)) ) {
		nTotalRead += nRead;
	}
	delete [] resize_buffer;
	fclose(fp);
	cout<<"Total read:"<<nTotalRead<<"bytes"<<endl;
	
	assert(nWidth*nHeight*nBpp==nTotalRead);
	cout<<"resize end"<<endl;
	return 0;
}

void method_0(int nThreshold,int nBpp,int nWidth,int nHeight,int out_pixel_per_bytes,_u8* resize_buffer,_u8* raw_buffer)
{
	assert(resize_buffer);
	
	int resize_w = nWidth;
	int resize_h = nHeight;
	
	if((nWidth * nHeight) > nThreshold) {
		float ratio = nWidth / (float)nHeight;
		while(resize_w * resize_h > nThreshold) {
				resize_h--;
				resize_w = (int)((float)resize_h * ratio);
		}
		resize_raw_image(resize_buffer,raw_buffer,nBpp,nWidth,nHeight,resize_w,resize_h,out_pixel_per_bytes);
	}
}

int resize_raw_image(_u8* dest,_u8* raw_buffer,int nBpp,int nWidth,int nHeight,int resize_w,int resize_h,_u32 out_pixel_per_bytes)
{
	_u32 pixel_per_bytes = nBpp; 
	_u32 x_ratio = (_u32)((nWidth<<16)/resize_w) + 1;
	_u32 y_ratio = (_u32)((nHeight<<16)/resize_h) + 1;

	_u32 i, j;
	_u32 x2, y2;
	for(i = 0 ; i < resize_h ; ++i) {
		y2 = ((i * y_ratio)>>16) ;
		for(j = 0 ; j < resize_w ; ++j) {
			x2 = ((j * x_ratio)>>16) ;

			dest[(i*resize_w*out_pixel_per_bytes)+(j*out_pixel_per_bytes)+0] = 
				raw_buffer[(y2*nWidth*pixel_per_bytes)+(x2*pixel_per_bytes)+2];

			dest[(i*resize_w*out_pixel_per_bytes)+(j*out_pixel_per_bytes)+1] = 
				raw_buffer[(y2*nWidth*pixel_per_bytes)+(x2*pixel_per_bytes)+1];

			dest[(i*resize_w*out_pixel_per_bytes)+(j*out_pixel_per_bytes)+2] = 
				raw_buffer[(y2*nWidth*pixel_per_bytes)+(x2*pixel_per_bytes)+0];
		}
	}
	return 0;
}

int conv_image_bpp(_u8* src, _u8* dest, const _u16 w, const _u16 h, _u32 src_bpp, _u32 dest_bpp) {
	_u32 i, j;
	src_bpp /= 8; /* maybe 32 */
	dest_bpp /= 8; /* maybe 24 */

	for(i = 0 ; i < h ; ++i) {
		for(j = 0 ; j < w ; ++j) {
			dest[(i * w * dest_bpp) + (j * dest_bpp) + 0] = src[(i * w * src_bpp) + (j * src_bpp) + 2];
			dest[(i * w * dest_bpp) + (j * dest_bpp) + 1] = src[(i * w * src_bpp) + (j * src_bpp) + 1];
			dest[(i * w * dest_bpp) + (j * dest_bpp) + 2] = src[(i * w * src_bpp) + (j * src_bpp) + 0];
		}
	}
	return 0;
}