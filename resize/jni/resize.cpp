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
	while( 0 < (nRead = fread(buffer, 1, nReadUnit, fp)) ) {
		nTotalRead += nRead;
	}
	fclose(fp);
	cout<<"Total read:"<<nTotalRead<<"bytes"<<endl;
	
	assert(nWidth*nHeight*nBpp==nTotalRead);
	cout<<"resize end"<<endl;
	return 0;
}
