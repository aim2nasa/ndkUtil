#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <zlib.h>
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

void dump(const char* pFileName,_u8* buffer,size_t bufferSize);
float ratio(int width,int height);
void usage();
void compressOk(_u8* cmp_buffer,_u32 cmp_buffer_size,_u8* raw_buffer,_u32 raw_buffer_size,int nCompLevel,const char* pDumpFileName);

int main(int argc,char* argv[])
{
	if(argc<6) {
		usage();
		return -1;
	}
	
	int nBpp = atoi(argv[1]);
	int nWidth = atoi(argv[2]);
	int nHeight = atoi(argv[3]);
	int nCompLevel = 1;
	if(argc>6) nCompLevel = atoi(argv[6]);
	
	cout<<"[Input parameters]"<<endl;
	cout<<"bytes per pixel:"<<nBpp<<",width:"<<nWidth<<",height:"<<nHeight<<endl;

	cout<<"source raw input file:"<<argv[4]<<endl;
	cout<<"compressed output file:"<<argv[5]<<endl;
	cout<<"compression level:"<<nCompLevel<<endl;

	cout<<"source {width("<<nWidth<<") x height("<<nHeight<<"):"<<nWidth*nHeight<<",ratio(width/height):"<<ratio(nWidth,nHeight);
	cout<<",bytes per pixel:"<<nBpp<<",size:"<<nWidth*nHeight*nBpp<<"bytes}"<<endl;
	
	FILE* fp = fopen(argv[4],"rb");
	if(!fp) {
		cout<<"file open failure:"<<argv[4]<<endl;
		return -1;
	}

	struct timeval startTime;
	struct timeval endTime;
	struct timeval totalTime;

	cout<<endl<<"[processing]"<<endl;
	
	int nReadUnit = BUFFER_SIZE;
	int nRead=0,nTotalRead = 0;
	_u8 buffer[BUFFER_SIZE];
	_u32 raw_buffer_size = nWidth*nHeight*nBpp;
	_u32 cmp_buffer_size = raw_buffer_size;
	_u8* raw_buffer = new _u8[raw_buffer_size];	
	_u8* cmp_buffer = new _u8[cmp_buffer_size];
	while( 0 < (nRead = fread(buffer, 1, nReadUnit, fp)) ) {
		memcpy(raw_buffer+nTotalRead,buffer,nRead);
		nTotalRead += nRead;
	}
	fclose(fp);
	cout<<"Total read:"<<nTotalRead<<"bytes from "<<argv[4]<<endl;

	START_TIMER(startTime)
	int nRtn = compress2(cmp_buffer,(uLongf*)&cmp_buffer_size,raw_buffer,raw_buffer_size,nCompLevel);
	switch(nRtn){
		case Z_OK:
			compressOk(cmp_buffer,cmp_buffer_size,raw_buffer,raw_buffer_size,nCompLevel,argv[5]);
			break;
		case Z_MEM_ERROR:
			cout<<"Z_MEM_ERROR("<<nRtn<<"), Insufficient memory"<<endl;
			break;
		case Z_BUF_ERROR:
			cout<<"Z_BUF_ERROR("<<nRtn<<"), The buffer dest was not large enough to hold the compressed data"<<endl;
			break;
		case Z_STREAM_ERROR:
			cout<<"Z_STREAM_ERROR("<<nRtn<<"), The level was not Z_DEFAULT_LEVEL, or was not between 0 and 9"<<endl;
			break;
		default:
			cout<<"Undefined error("<<nRtn<<")"<<endl;
			break;
	}
	STOP_TIMER(endTime)
	ELAPSED_TIME(startTime,endTime,totalTime,"elapsed time in compression:")

	delete [] cmp_buffer;	
	delete [] raw_buffer;
	
	assert(nWidth*nHeight*nBpp==nTotalRead);
	cout<<"compress done"<<endl;
	return 0;
}

void usage()
{
	cout<<"usage: zc <bytes per pixel> <width> <height> <source raw file> <compressed file> <compression level>"<<endl;
	cout<<"ex. zc 3 576 1024 resized.raw resized.zip 1"<<endl;
	cout<<"    -bytes per pixel : ex. 3 for RGB, 4 for RGBA"<<endl;
	cout<<"    -width : width of source raw file"<<endl;
	cout<<"    -height : height of source raw file"<<endl;
	cout<<"    -source raw file : input raw file"<<endl;
	cout<<"    -compressed file : compressed file of source"<<endl;
	cout<<"    -compression level : unless specified, 1 is used as default value"<<endl;
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

void compressOk(_u8* cmp_buffer,_u32 cmp_buffer_size,_u8* raw_buffer,_u32 raw_buffer_size,int nCompLevel,const char* pDumpFileName)
{
	dump(pDumpFileName,cmp_buffer,cmp_buffer_size);
	cout<<"Raw buffer size:"<<raw_buffer_size<<",Compressed buffer size:"<<cmp_buffer_size<<",compLevel:"<<nCompLevel<<endl;
}
