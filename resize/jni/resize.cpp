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
#define OUT_BYTE_PER_PIXEL	3
#define START_TIMER(t) gettimeofday(&t,0);
#define STOP_TIMER(t) gettimeofday(&t,0);
#define ELAPSED_TIME(start,end,total,name) \
{\
	timersub(&end,&start,&total); \
	printf(""name": %ld.%06ldsec\n",total.tv_sec,total.tv_usec);\
}

using namespace std;

int resize_raw_image(_u8* raw_buffer,_u8* dest,int nBpp,int nWidth,int nHeight,int resize_w,int resize_h,_u32 out_pixel_per_bytes);
int conv_image_bpp(_u8* src, _u8* dest, const _u16 w, const _u16 h, _u32 src_bpp, _u32 dest_bpp);
void method_0(int nThreshold,int nBpp,int nWidth,int nHeight,int* pResize_w,int* pResize_h,int out_pixel_per_bytes,_u8* resize_buffer,_u8* raw_buffer);
void method_1(int nBpp,int nWidth,int nHeight,int out_pixel_per_bytes,_u8* resize_buffer,_u8* raw_buffer);
void dump(const char* pFileName,_u8* buffer,size_t bufferSize);
float ratio(int width,int height);
void usage();
bool inputVerify(int argc,int nMethod,int nBpp);

int main(int argc,char* argv[])
{
	if(argc<7) {
		usage();
		return -1;
	}
	
	int nBpp = atoi(argv[1]);
	int nWidth = atoi(argv[2]);
	int nHeight = atoi(argv[3]);	
	int nMethod = atoi(argv[6]);

	if(inputVerify(argc,nMethod,nBpp)==false) {
		usage();
		return -1;
	}
	
	int nResizeWidth,nResizeHeight;
	if(nMethod==0) {
		nResizeWidth = atoi(argv[7]);
		nResizeHeight = atoi(argv[8]);
	}else if(nMethod==1){
		nResizeWidth = nWidth;
		nResizeHeight = nHeight;
	}else{
		assert(false);
	}

	cout<<"[Input parameters]"<<endl;
	cout<<"bytes per pixel:"<<nBpp<<",width:"<<nWidth<<",height:"<<nHeight<<",method:"<<nMethod;
	cout<<",resize width:"<<nResizeWidth<<",resize height:"<<nResizeHeight<<endl;

	cout<<"source raw file:"<<argv[4]<<endl;
	cout<<"resized raw file:"<<argv[5]<<endl;

	cout<<"source {width("<<nWidth<<") x height("<<nHeight<<"):"<<nWidth*nHeight<<",ratio(width/height):"<<ratio(nWidth,nHeight);
	cout<<",bytes per pixel:"<<nBpp<<",size:"<<nWidth*nHeight*nBpp<<"bytes}"<<endl;

	int nThreshold = nResizeWidth*nResizeHeight;
	cout<<"resized {width("<<nResizeWidth<<") x height("<<nResizeHeight<<"):"<<nThreshold<<",ratio(width/height):"<<ratio(nResizeWidth,nResizeHeight);
	cout<<",bytes per pixel:"<<OUT_BYTE_PER_PIXEL<<",size:"<<nResizeWidth*nResizeHeight*OUT_BYTE_PER_PIXEL<<"bytes}"<<endl;
	
	FILE* fp = fopen(argv[4],"rb");
	if(!fp) {
		cout<<"file open failure"<<argv[1]<<endl;
		return -1;
	}

	cout<<endl<<"[processing with method:"<<nMethod<<"]"<<endl;
	
	int nReadUnit = BUFFER_SIZE;
	int nRead=0,nTotalRead = 0;
	_u8 buffer[BUFFER_SIZE];
	_u8* raw_buffer = new _u8[nWidth*nHeight*nBpp];	
	_u8* resize_buffer = new _u8[nWidth*nHeight*OUT_BYTE_PER_PIXEL];	//편의상 실제 resize크기보다 크게 잡고 이중 일부를 쓰도록 한다
	while( 0 < (nRead = fread(buffer, 1, nReadUnit, fp)) ) {
		memcpy(raw_buffer+nTotalRead,buffer,nRead);
		nTotalRead += nRead;
	}
	cout<<"Total read:"<<nTotalRead<<"bytes from "<<argv[4]<<endl;
	
	int resize_w = nWidth;
	int resize_h = nHeight;

	cout<<"Before resizing, width:"<<resize_w<<",height:"<<resize_h<<endl;
	
	struct timeval startTime;
	struct timeval endTime;
	struct timeval totalTime;	
	
	START_TIMER(startTime)
	if(nMethod==0) method_0(nThreshold,nBpp,nWidth,nHeight,&resize_w,&resize_h,OUT_BYTE_PER_PIXEL,resize_buffer,raw_buffer);
	if(nMethod==1) method_1(nBpp,nWidth,nHeight,OUT_BYTE_PER_PIXEL,resize_buffer,raw_buffer);
	STOP_TIMER(endTime)
	ELAPSED_TIME(startTime,endTime,totalTime,"elapsed time in resizing:")
	
	cout<<"After resizing, width:"<<resize_w<<",height:"<<resize_h<<endl;	
	
	dump(argv[5],resize_buffer,resize_w*resize_h*OUT_BYTE_PER_PIXEL);
	
	delete [] raw_buffer;
	delete [] resize_buffer;
	fclose(fp);
	
	assert(nWidth*nHeight*nBpp==nTotalRead);
	cout<<"resizing done"<<endl;
	return 0;
}

bool inputVerify(int argc,int nMethod,int nBpp)
{
	if(nMethod==0 && argc<9) {
		cout<<"arguments("<<argc<<") are not sufficient for method:"<<nMethod<<endl;
		return false;
	}

	if(nMethod!=0 && nMethod!=1) {
		cout<<"Unsupported method:"<<nMethod<<endl;
		return false;
	}
	
	if(nMethod==1 && nBpp!=4) {
		cout<<"Unsupported raw format, method:"<<nMethod<<",bytes per pixel:"<<nBpp<<endl;	
		return false;
	}
	return true;
}

void usage()
{
	cout<<"usage: resize <bytes per pixel> <width> <height> <source raw file> <resized raw file> <method (0 or 1)>";
	cout<<" <resize width> <resize height>"<<endl;
	cout<<"ex. resize 4 1440 2560 dump.raw resized.raw 0 576 1024"<<endl;
	cout<<"    -bytes per pixel : ex. 3 for RGB, 4 for RGBA"<<endl;
	cout<<"    -width : width of source raw file"<<endl;
	cout<<"    -height : height of source raw file"<<endl;
	cout<<"    -source raw file : input raw file"<<endl;
	cout<<"    -resized raw file : raw file which to be generated sized as (resize width) x (resize height)"<<endl;
	cout<<"    -method 0: resizing through threshold value and neighborhood algorithm"<<endl;
	cout<<"            1: resizing by changing Bytes per Pixel from 4bytes to 3bytes(only works for Bytes per Pixel is 4)"<<endl;
	cout<<"    -resize width : width of raw file which to be generated"<<endl;
	cout<<"    -resize height : height of raw file which to be generated"<<endl;
	cout<<"Copyright© by Funzin"<<endl<<endl;
}

float ratio(int width,int height)
{
	return (float)width / (float)height;
}

void dump(const char* pFileName,_u8* buffer,size_t bufferSize)
{
	assert(pFileName);
	cout<<"writing "<<bufferSize<<"bytes to "<<pFileName<<"..."<<endl;
	
	FILE* fp = fopen(pFileName,"w");
	size_t nTotal=0,readUnit = BUFFER_SIZE;
	while(nTotal<bufferSize) {
		if((bufferSize-nTotal)<BUFFER_SIZE) readUnit = bufferSize-nTotal;
		size_t nWrite = fwrite(buffer+nTotal,1,readUnit,fp);
		assert(readUnit==nWrite);
		nTotal += nWrite;
	}
	fclose(fp);	
	cout<<bufferSize<<"bytes written to "<<pFileName<<endl;
}

void method_0(int nThreshold,int nBpp,int nWidth,int nHeight,int* pResize_w,int* pResize_h,int out_pixel_per_bytes,_u8* resize_buffer,_u8* raw_buffer)
{
	assert(resize_buffer);
		
	if((nWidth * nHeight) > nThreshold) {
		float ratio = nWidth / (float)nHeight;
		while( (*pResize_w)*(*pResize_h) > nThreshold) {
				(*pResize_h)--;
				(*pResize_w) = (int)((float)(*pResize_h) * ratio);
		}
		resize_raw_image(resize_buffer,raw_buffer,nBpp,nWidth,nHeight,*pResize_w,*pResize_h,out_pixel_per_bytes);
	}
}

void method_1(int nBpp,int nWidth,int nHeight,int out_pixel_per_bytes,_u8* resize_buffer,_u8* raw_buffer)
{
	assert(resize_buffer);
	assert(nBpp==3);
	conv_image_bpp(raw_buffer, resize_buffer, nWidth, nHeight, nBpp, out_pixel_per_bytes);
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

	for(i = 0 ; i < h ; ++i) {
		for(j = 0 ; j < w ; ++j) {
			dest[(i * w * dest_bpp) + (j * dest_bpp) + 0] = src[(i * w * src_bpp) + (j * src_bpp) + 2];
			dest[(i * w * dest_bpp) + (j * dest_bpp) + 1] = src[(i * w * src_bpp) + (j * src_bpp) + 1];
			dest[(i * w * dest_bpp) + (j * dest_bpp) + 2] = src[(i * w * src_bpp) + (j * src_bpp) + 0];
		}
	}
	return 0;
}
