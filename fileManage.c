#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>

#include "bitmapStruct.h"
#include "bmpManage.h"
#include "utils.h"
#include "math.h"

#define UP		0
#define DOWN	1
#define LEFT	2
#define RIGHT	3

void	printBmpHeader(const PBITMAP_IMAGE bmpImage);
void	checkBmp(const PBITMAP_IMAGE bmpImage);

int		realWidth(const PBITMAP_INFO_HEADER pbiheader){
	return (((pbiheader->biWidth * pbiheader->biBitCount) + 31) & ~31) / 8;
}

void	getBmpHeader(FILE* fpBmp, PBITMAP_IMAGE bmpImage){
	fread(&bmpImage->fileHeader, sizeof(BITMAP_FILE_HEADER), 1,fpBmp);
	fread(&bmpImage->infoHeader, sizeof(BITMAP_INFO_HEADER), 1,fpBmp);
}

void	updateBmpHeader(PBITMAP_IMAGE bmpImage, WORD wType)
{
	int		flag = (wType==8)?1:3;
	bmpImage->infoHeader.biBitCount = wType; // 24 或者 8
	bmpImage->infoHeader.biClrUsed = (wType == 24) ? 0 : 256;
	bmpImage->fileHeader.bfOffBits = 54 + bmpImage->infoHeader.biClrUsed * sizeof(RGB_QUAD);
	// //计算位图的实际宽度并确保它为32的倍数 
	// 计算将图片存入内存空间需要的字节数
	bmpImage->infoHeader.biSizeImage = realWidth(&bmpImage->infoHeader) * bmpImage->infoHeader.biHeight;
	bmpImage->fileHeader.bfSize = bmpImage->fileHeader.bfOffBits + bmpImage->infoHeader.biSizeImage;
}

void	setBmpHeader(FILE* fpBmp, const PBITMAP_IMAGE bmpImage)
{
	fwrite(&bmpImage->fileHeader, sizeof(BITMAP_FILE_HEADER), 1, fpBmp);
	fwrite(&bmpImage->infoHeader, sizeof(BITMAP_INFO_HEADER), 1, fpBmp);
}

void	setRGBQUAD(FILE* fpBmp){
	int		i;
	RGB_QUAD rgbquad[256];
	for(i=0;i<256;i++) {
		rgbquad[i].rgbBlue = i;
		rgbquad[i].rgbGreen = i;
		rgbquad[i].rgbRed = i;
		rgbquad[i].rgbReserved = 0;
	}
	fwrite(rgbquad, 256 * sizeof(RGB_QUAD), 1, fpBmp); // 灰度图有256中颜色表项
}

FILE*	loadImage(char* src, PBITMAP_IMAGE bmpImage){
	FILE* fpBmp;

	if ((fpBmp = fopen(src,"rb"))==NULL){
		printErrors("Open the file failed");
		exit(0);
	}
	rewind(fpBmp);

	getBmpHeader(fpBmp, bmpImage); 
	//checkBmp(bmpImage);
	//printBmpHeader(bmpImage);
	//定位到文件数据区
	fseek(fpBmp, bmpImage->fileHeader.bfOffBits, SEEK_SET);

	printSuccess("loadImage");
	return fpBmp;
}

BYTE*	creatImageArr(FILE* fpBmp, const PBITMAP_IMAGE bmpImage){
	int		i, j, k, iRealWidth, flag=1, 
			curWidth = bmpImage->infoHeader.biWidth,
			curHeight = bmpImage->infoHeader.biHeight;
	BYTE	*cur_clr, *imageArr;
	BYTE	count8, count24;
	
	iRealWidth = realWidth(&bmpImage->infoHeader);
	count8	= COUNT_8(curWidth);
	count24 = COUNT_24(curWidth);
	
	flag = (bmpImage->infoHeader.biBitCount == 8) ? 1 : 3;
	cur_clr = (BYTE*)calloc(flag,sizeof(BYTE));
	imageArr = (BYTE*)calloc(curWidth * curWidth * flag, sizeof(BYTE));
	
	fseek(fpBmp, bmpImage->fileHeader.bfOffBits, SEEK_SET);
	for(i=0, k=0; i< curHeight; ++i){
		for (j=0; j<curWidth; ++j){
			fread(cur_clr,flag,1,fpBmp);
			if(bmpImage->infoHeader.biBitCount == 8){ //8位灰度图
				imageArr[i*curWidth+j] = *cur_clr;
			}else if (bmpImage->infoHeader.biBitCount == 24){ // 24位真色位图
				//fread(cur_clr,3,1,fpBmp);
				imageArr[k] = *cur_clr;
				imageArr[k+1] = *(cur_clr+1);
				imageArr[k+2] = *(cur_clr+2);
				k += 3;
			}
			if(feof(fpBmp)){
				printErrors("Read Pixel data failed");
				free(cur_clr);
				exit(0);
			}
		}
		if(bmpImage->infoHeader.biBitCount == 8){ //8位灰度图
			fseek(fpBmp, count8, SEEK_CUR);
		}else if (bmpImage->infoHeader.biBitCount == 24){ // 24位真色位图
			fseek(fpBmp, count24, SEEK_CUR);
		}
	}
	free(cur_clr);
	fclose(fpBmp);
	//printBmpHeader(bmpImage);
	printSuccess("creatImageArr");
	return imageArr;
}

BYTE*	extractBmpByBound(const BYTE* bmpArr, LONG widht, LONG height, LONG bound[], WORD wType){
	int		flag = (wType==8)?1:3;
	BYTE	*tempArr = (BYTE *)calloc((bound[UP]-bound[DOWN]+1)*(bound[RIGHT]-bound[LEFT]+1)*flag, sizeof(BYTE));
	int		i, j, k, g;
	LONG	tmpIndex;
	
	if (bound[UP]>bound[DOWN] && bound[RIGHT]>bound[LEFT]){ // 满足要求的定位边界
		for (i=bound[DOWN], k=0; i<=bound[UP]; ++i){
			for (j=bound[LEFT]; j<=bound[RIGHT]; ++j){
				if (wType==8){
					tempArr[k++] = bmpArr[i*widht+j];
				}else if(wType==24){  
					tmpIndex = (i*widht+j)*3;
					tempArr[k] = bmpArr[tmpIndex];  // 注意此处的赋值
					tempArr[k+1] = bmpArr[tmpIndex+1];
					tempArr[k+2] = bmpArr[tmpIndex+2];
					k += 3;
				}
			}
		}
	}
	return tempArr;
}

void	creatBmpByArr(char* src, PBITMAP_IMAGE bmpImage, BYTE* imageArr, WORD wType){
	FILE*	fpBmp;
	int		i, j, k;
	LONG	curWidth, curHeight;
	BYTE	count8, bmpNull = 0, count24;

	if((fpBmp=fopen(src,"wb"))==NULL) 
	{
		printErrors("Open the file failed");
		exit(0);
	}

	//printf("%d %d", bmpImage->infoHeader.biHeight, bmpImage->infoHeader.biWidth);
	updateBmpHeader(bmpImage, wType);
	setBmpHeader(fpBmp, bmpImage);
	if (wType==8){
		setRGBQUAD(fpBmp);
	}
	//printBmpHeader(bmpImage);
	curWidth = bmpImage->infoHeader.biWidth;
	curHeight = bmpImage->infoHeader.biHeight;
	count8 = COUNT_8(curWidth);
	count24 = COUNT_24(curWidth);

	//printf("%d === \n", ftell(fpBmp));
	for(i=0, k=0; i<curHeight; ++i){
		for(j=0; j<curWidth; ++j){

			if(feof(fpBmp))
			{
				printErrors("Read Pixel data failed");
				exit(0);
			}
			if (wType == BF_TYPE_8){
				fwrite(&imageArr[i*curWidth+j],1,1,fpBmp);
			}else if(wType == BF_TYPE_24){
				fwrite(&imageArr[k],1,1,fpBmp);
				fwrite(&imageArr[k+1],1,1,fpBmp);
				fwrite(&imageArr[k+2],1,1,fpBmp);
				k += 3;
			}
		}
		if (wType == BF_TYPE_8){
			fwrite(&bmpNull,count8,1,fpBmp);
		}else if(wType == BF_TYPE_24){
			fwrite(&bmpNull,count24,1,fpBmp);
		}
	}
	//free(imageArr);
	fclose(fpBmp);
	printSuccess("creatBmpByArr");
}

void	printBmpHeader(const PBITMAP_IMAGE bmpImage){
	BITMAP_FILE_HEADER bmFileHeader = bmpImage->fileHeader;
	BITMAP_INFO_HEADER bmInfoHeader = bmpImage->infoHeader; 
	printf("============打印位图文件头============\n");
	printf("位图文件的类型:		%ld\n", bmFileHeader.bfType);
	printf("位图文件的大小:		%ld\n", bmFileHeader.bfSize);
	printf("位图文件保留字:%ld\n", bmFileHeader.bfReserved1);
	printf("位图文件保留字:%ld\n", bmFileHeader.bfReserved2);
	printf("实际位图数据的偏移字节数:	%ld\n", bmFileHeader.bfOffBits);
	printf("============打印位图信息头============\n");
	printf("图像描述信息块所占用字节数: %ld\n", bmInfoHeader.biSize);
	printf("位图的宽度: %ld\n", bmInfoHeader.biWidth);
	printf("位图的高度: %ld\n", bmInfoHeader.biHeight);
	printf("图像描述信息块所占用字节数:	%ld\n", bmInfoHeader.biSize);
	printf("每个像素所需的位数:	%ld\n", bmInfoHeader.biBitCount);
	printf("图像数据区的大小:	%ld\n", bmInfoHeader.biSizeImage);
	printf("水平每米有多少像素: %ld\n", bmInfoHeader.biXPelsPerMeter);
	printf("垂直每米有多少像素: %ld\n", bmInfoHeader.biYPelsPerMeter);
	printf("位图实际使用的颜色表中的颜色数: %ld\n", bmInfoHeader.biClrUsed);
	printf("位图显示过程中重要的颜色数: %ld\n", bmInfoHeader.biClrImportant);
}

void	checkBmp(const PBITMAP_IMAGE bmpImage){
	BITMAP_FILE_HEADER bmFileHeader = bmpImage->fileHeader;
	BITMAP_INFO_HEADER bmInfoHeader = bmpImage->infoHeader;
	if(bmFileHeader.bfType!=0x4D42) // 判断是否为位图
	{
		printf("Error:This file is not bitmap file!\n");
		exit(0);
	}
	if(bmInfoHeader.biBitCount!=24) //
	{
		printf("Error:This bitmap is not 24bit!\n");
		exit(0);
	}
	if(bmInfoHeader.biCompression!=0) // 检查位图是否压缩
	{
		printf("Error:This 8bit bitmap file is not BI_RGB type!\n");
		exit(0);
	}
}

HSI* 	RGB2HSI(const BYTE* imageArr24, LONG width, LONG height) {
	HSI* myHSI;
	double	th, R, G, B, min, max, tmp, tmp2;
	LONG	i, j, k, tmpIndex;

	myHSI = (HSI*)calloc(width*height, sizeof(HSI));
	for (i = 0, k=0; i < height; i++)
	{
		for ( j = 0; j < width; j++, k++)
		{
			tmpIndex = 3 * (i*width + j);
			B = imageArr24[tmpIndex] / 255.0;	// 取得b,g,r的值，并归一化
			G = imageArr24[tmpIndex+1] / 255.0;
			R = imageArr24[tmpIndex+2] / 255.0;

			min = MIN_3(B, G, R);
			max = MAX_3(B, G, R);
			myHSI[k].I = (B + G + R) / 3.0;

			if (myHSI[k].I == 0 || max == min)
			{
				// this is a black image or grayscale image
				myHSI[k].S = 0;
				myHSI[k].H = 0;
			}
			else
			{
				myHSI[k].S = 1 - min / myHSI[k].I;
				th = sqrt((R - G)*(R - G) + (R - B)*(R - B) + (G - B)*(G - B));
				//th = sqrt((R - G)*(R - G) + (R - B)*(G - B));
				th = acos(((R - G) + (R - B)) * 0.5 / th);
				myHSI[k].H = (G >= B) ? th : 2 * PI - th;
			}
			//printf("%lf %lf %lf \n", myHSI[k].H, myHSI[k].I, myHSI[k].S);
		}
	}
	return myHSI;
}

HSV* 	RGB2HSV(const BYTE* imageArr24, LONG width, LONG height) {
	HSV*	myHSV;
	double	th, R, G, B, min, max, tmp, tmp2;
	LONG	i, j, k, tmpIndex;
	const int HMax = 60;	// 代表色调的个数
	myHSV = (HSV*)calloc(width*height, sizeof(HSV));
	for (i = 0, k = 0; i < height; i++)
	{
		for (j = 0; j < width; j++, k++)
		{
			tmpIndex = 3 * (i*width + j);
			B = imageArr24[tmpIndex] / 255.0;	// 取得b,g,r的值，并归一化
			G = imageArr24[tmpIndex + 1] / 255.0;
			R = imageArr24[tmpIndex + 2] / 255.0;

			min = MIN_3(B, G, R);
			max = MAX_3(B, G, R);
			
			myHSV[k].S = (max == 0) ? 0 : (max - min) / max;
			myHSV[k].V = max;
			if (max == min)
			{
				myHSV[k].H = 0;
			} 
			else if (R == max && G >= B)
			{
				myHSV[k].H = (G - B) / (max - min) * HMax ;
			}
			else if (R == max && G < B)
			{
				myHSV[k].H = (6 + (G - B) / (max - min)) * HMax;
			}
			else if (G == max)
			{
				myHSV[k].H = (2 + (B - R)/ (max - min) ) * HMax;
			}
			else if (B == max)
			{
				myHSV[k].H = (4 + (R - G) / (max - min)) * HMax;
			}

			//printf("%lf %lf %lf \n", myHSV[k].H, myHSV[k].S, myHSV[k].V);
		}
	}
	return myHSV;
}