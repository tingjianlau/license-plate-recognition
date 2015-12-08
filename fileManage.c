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

//void thinner(char* source, char* dest) {
//	FILE* fpSrcBmp, *fpDecBmp;
//	BYTE* imageArr;
//	int i, j, count8;
//	BYTE Bmpnul = 0;
//	LONG curWidth, curHeight;
//
//
//	BITMAP_FILE_HEADER curFileHeader; // bmp文件头
//	BITMAP_INFO_HEADER curInfoHeader; // 位图信息头
//	memset(&curFileHeader, 0, sizeof(BITMAP_FILE_HEADER));
//	memset(&curInfoHeader, 0, sizeof(BITMAP_INFO_HEADER));
//
//	fpSrcBmp = loadImage(source, &curFileHeader, &curInfoHeader);
//	//printf("%d \n",ftell(fpSrcBmp));
//	imageArr = creatImageArr(fpSrcBmp, &curFileHeader, &curInfoHeader);
//
//	curWidth = curInfoHeader.biWidth;
//	curHeight = curInfoHeader.biHeight;
//	//printf("%d \n", curInfoHeader.biHeight);
//	count8 = (4 - (curInfoHeader.biWidth) % 4) % 4;
//
//	// 将数据转化为0,1值
//	for (i = 0; i<curHeight; ++i) {
//		for (j = 0; j<curWidth; ++j) {
//			if (imageArr[i*curWidth + j]> 0) // 将白色255改成1
//				imageArr[i*curWidth + j] = 1;
//		}
//	}
//
//	///===============================
//	//fflush(fpSrcBmp);
//	fclose(fpSrcBmp);
//
//	thinnerRosenfeld(imageArr, curWidth, curHeight);
//	//thinnerHilditch(imageArr,curWidth, curHeight);
//	//thinnerPavlidis(imageArr,curWidth, curHeight);
//
//
//	for (i = 0; i<curHeight; ++i) {
//		for (j = 0; j<curWidth; ++j) {
//			if (imageArr[i*curWidth + j] == 1) // 将白色255改成1
//				imageArr[i*curWidth + j] = 255;
//		}
//	}
//
//
//	//fpDecBmp = creatFile(dest,&curFileHeader,&curInfoHeader);
//	fpDecBmp = creatBmpByImgArr(dest, &curFileHeader, &curInfoHeader, imageArr);
//	//for(i=0; i<curHeight; ++i){
//	//	for(j=0; j<curWidth; ++j){
//
//	//		if(feof(fpDecBmp))
//	//		{
//	//			printErrors(2);
//	//			//free(cur_clr);
//	//			//return -1;
//	//		}
//	//		fwrite(&imageArr[i*curWidth+j],1,1,fpDecBmp);
//
//	//	}
//	//	//printf("%d\n",ftell(fpDecBmp));
//	//	fwrite(&Bmpnul,count8,1,fpDecBmp);
//	//}
//
//	for (i = 0; i<curWidth*curHeight; ++i) {
//		printf("%4d ", imageArr[i]);
//		if ((i + 1) % (NORMALIZED_WIDTH / 2) == 0) printf("\n");
//	}
//	fclose(fpSrcBmp);
//}

/////////////////////////////////////////////////////////////////////////
//Rosenfeld细化算法
//功能：对图象进行细化
//参数：image：代表图象的一维数组
//      lx：图象宽度
//      ly：图象高度
//      无返回值
void thinnerRosenfeld(BYTE *image,	LONG width, LONG height)
{
	char *f, *g;
	char n[10];
	char a[5] = { 0, -1, 1, 0, 0 };
	char b[5] = { 0, 0, 0, 1, -1 };
	char nrnd, cond, n48, n26, n24, n46, n68, n82, n123, n345, n567, n781;
	short k, shori;
	LONG i, j;
	LONG ii, jj, kk, kk1, kk2, kk3, size;

	for (i = 0; i < height; i++)
	{
		for ( j = 0; j < width; j++)
		{
			if (image[i*width+j] == 255)
			{
				image[i*width + j] = 1;
			}
		}
	}

	size = (LONG)width * (LONG)height;

	g = (char *)malloc(size);
	if (g == NULL)
	{
		printf("error in alocating mmeory!\n");
		return;
	}

	f = (char *)image;
	for (kk = 0l; kk<size; kk++)
	{
		g[kk] = f[kk];
	}

	do
	{
		shori = 0;
		for (k = 1; k <= 4; k++)
		{
			for (i = 1; i<width - 1; i++)
			{
				ii = i + a[k];

				for (j = 1; j<height - 1; j++)
				{
					kk = i*height + j;

					if (!f[kk])
						continue;

					jj = j + b[k];
					kk1 = ii*height + jj;

					if (f[kk1])
						continue;

					kk1 = kk - height - 1;
					kk2 = kk1 + 1;
					kk3 = kk2 + 1;
					n[3] = f[kk1];
					n[2] = f[kk2];
					n[1] = f[kk3];
					kk1 = kk - 1;
					kk3 = kk + 1;
					n[4] = f[kk1];
					n[8] = f[kk3];
					kk1 = kk + height - 1;
					kk2 = kk1 + 1;
					kk3 = kk2 + 1;
					n[5] = f[kk1];
					n[6] = f[kk2];
					n[7] = f[kk3];

					nrnd = n[1] + n[2] + n[3] + n[4]
						+ n[5] + n[6] + n[7] + n[8];
					if (nrnd <= 1)
						continue;

					cond = 0;
					n48 = n[4] + n[8];
					n26 = n[2] + n[6];
					n24 = n[2] + n[4];
					n46 = n[4] + n[6];
					n68 = n[6] + n[8];
					n82 = n[8] + n[2];
					n123 = n[1] + n[2] + n[3];
					n345 = n[3] + n[4] + n[5];
					n567 = n[5] + n[6] + n[7];
					n781 = n[7] + n[8] + n[1];

					if (n[2] == 1 && n48 == 0 && n567>0)
					{
						if (!cond)
							continue;
						g[kk] = 0;
						shori = 1;
						continue;
					}
					if (n[6] == 1 && n48 == 0 && n123>0)
					{
						if (!cond)
							continue;
						g[kk] = 0;
						shori = 1;
						continue;
					}

					if (n[8] == 1 && n26 == 0 && n345>0)
					{
						if (!cond)
							continue;
						g[kk] = 0;
						shori = 1;
						continue;
					}

					if (n[4] == 1 && n26 == 0 && n781>0)
					{
						if (!cond)
							continue;
						g[kk] = 0;
						shori = 1;
						continue;
					}

					if (n[5] == 1 && n46 == 0)
					{
						if (!cond)
							continue;
						g[kk] = 0;
						shori = 1;
						continue;
					}

					if (n[7] == 1 && n68 == 0)
					{
						if (!cond)
							continue;
						g[kk] = 0;
						shori = 1;
						continue;
					}

					if (n[1] == 1 && n82 == 0)
					{
						if (!cond)
							continue;
						g[kk] = 0;
						shori = 1;
						continue;
					}

					if (n[3] == 1 && n24 == 0)
					{
						if (!cond)
							continue;
						g[kk] = 0;
						shori = 1;
						continue;
					}

					cond = 1;
					if (!cond)
						continue;
					g[kk] = 0;
					shori = 1;
				}
			}

			for (i = 0; i<width; i++)
			{
				for (j = 0; j<height; j++)
				{
					kk = i*height + j;
					f[kk] = g[kk];
				}
			}
		}
	} while (shori);
	free(g);
	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			if (image[i*width + j] == 1)
			{
				image[i*width + j] = 255;
			}
		}
	}
}

/////////////////////////////////////////////////////////////////////////
//Hilditch细化算法
//功能：对图象进行细化
//参数：image：代表图象的一维数组
//      lx：图象宽度
//      ly：图象高度
//      无返回值
void thinnerHilditch(void *image, unsigned long lx, unsigned long ly)
{
	char *f, *g;
	char n[10];
	unsigned int counter;
	short k, shori, xx, nrn;
	unsigned long i, j;
	long kk, kk11, kk12, kk13, kk21, kk22, kk23, kk31, kk32, kk33, size;
	size = (long)lx * (long)ly;
	g = (char *)malloc(size);

	if (g == NULL)
	{
		printf("error in allocating memory!\n");
		return;
	}

	f = (char *)image;
	for (i = 0; i<lx; i++)
	{
		for (j = 0; j<ly; j++)
		{
			kk = i*ly + j;
			if (f[kk] != 0)
			{
				f[kk] = 1;
				g[kk] = f[kk];
			}
		}
	}

	counter = 1;

	do
	{
		printf("%4d*", counter);
		counter++;
		shori = 0;

		for (i = 0; i<lx; i++)
		{
			for (j = 0; j<ly; j++)
			{
				kk = i*ly + j;
				if (f[kk]<0)
					f[kk] = 0;
				g[kk] = f[kk];
			}
		}

		for (i = 1; i<lx - 1; i++)
		{
			for (j = 1; j<ly - 1; j++)
			{
				kk = i*ly + j;

				if (f[kk] != 1)
					continue;

				kk11 = (i - 1)*ly + j - 1;
				kk12 = kk11 + 1;
				kk13 = kk12 + 1;
				kk21 = i*ly + j - 1;
				kk22 = kk21 + 1;
				kk23 = kk22 + 1;
				kk31 = (i + 1)*ly + j - 1;
				kk32 = kk31 + 1;
				kk33 = kk32 + 1;

				if ((g[kk12] && g[kk21] && g[kk23] && g[kk32]) != 0)
					continue;

				nrn = g[kk11] + g[kk12] + g[kk13] + g[kk21] + g[kk23] +
					g[kk31] + g[kk32] + g[kk33];

				if (nrn <= 1)
				{
					f[kk22] = 2;
					continue;
				}

				n[4] = f[kk11];
				n[3] = f[kk12];
				n[2] = f[kk13];
				n[5] = f[kk21];
				n[1] = f[kk23];
				n[6] = f[kk31];
				n[7] = f[kk32];
				n[8] = f[kk33];
				n[9] = n[1];
				xx = 0;

				for (k = 1; k<8; k = k + 2)
				{
					if ((!n[k]) && (n[k + 1] || n[k + 2]))
						xx++;
				}

				if (xx != 1)
				{
					f[kk22] = 2;
					continue;
				}

				if (f[kk12] == -1)
				{
					f[kk12] = 0;
					n[3] = 0;
					xx = 0;

					for (k = 1; k<8; k = k + 2)
					{
						if ((!n[k]) && (n[k + 1] || n[k + 2]))
							xx++;
					}

					if (xx != 1)
					{
						f[kk12] = -1;
						continue;
					}

					f[kk12] = -1;
					n[3] = -1;
				}

				if (f[kk21] != -1)
				{
					f[kk22] = -1;
					shori = 1;
					continue;
				}

				f[kk21] = 0;
				n[5] = 0;
				xx = 0;

				for (k = 1; k<8; k = k + 2)
				{
					if ((!n[k]) && (n[k + 1] || n[k + 2]))
					{
						xx++;
					}
				}

				if (xx == 1)
				{
					f[kk21] = -1;
					f[kk22] = -1;
					shori = 1;
				}
				else
					f[kk21] = -1;
			}
		}
	} while (shori);

	free(g);
}

/////////////////////////////////////////////////////////////////////////
//Pavlidis细化算法
//功能：对图象进行细化
//参数：image：代表图象的一维数组
//      lx：图象宽度
//      ly：图象高度
//      无返回值
void thinnerPavlidis(void *image, unsigned long lx, unsigned long ly)
{
	char erase, n[8];
	char *f;
	unsigned char bdr1, bdr2, bdr4, bdr5;
	short c, k, b;
	unsigned long i, j;
	long kk, kk1, kk2, kk3;
	f = (char*)image;

	for (i = 1; i<lx - 1; i++)
	{
		for (j = 1; j<ly - 1; j++)
		{
			kk = i*ly + j;
			if (f[kk])
				f[kk] = 1;
		}
	}

	for (i = 0, kk1 = 0, kk2 = ly - 1; i<lx; i++, kk1 += ly, kk2 += ly)
	{
		f[kk1] = 0;
		f[kk2] = 0;
	}

	for (j = 0, kk = (lx - 1)*ly; j<ly; j++, kk++)
	{
		f[j] = 0;
		f[kk] = 0;
	}

	c = 5;
	erase = 1;
	while (erase)
	{
		c++;
		for (i = 1; i<lx - 1; i++)
		{
			for (j = 1; j<ly - 1; j++)
			{
				kk = i*ly + j;
				if (f[kk] != 1)
					continue;

				kk1 = kk - ly - 1;
				kk2 = kk1 + 1;
				kk3 = kk2 + 1;
				n[3] = f[kk1];
				n[2] = f[kk2];
				n[1] = f[kk3];
				kk1 = kk - 1;
				kk3 = kk + 1;
				n[4] = f[kk1];
				n[0] = f[kk3];
				kk1 = kk + ly - 1;
				kk2 = kk1 + 1;
				kk3 = kk2 + 1;
				n[5] = f[kk1];
				n[6] = f[kk2];
				n[7] = f[kk3];

				bdr1 = 0;
				for (k = 0; k<8; k++)
				{
					if (n[k] >= 1)
						bdr1 |= 0x80 >> k;
				}

				if ((bdr1 & 0252) == 0252)
					continue;
				f[kk] = 2;
				b = 0;

				for (k = 0; k <= 7; k++)
				{
					b += bdr1&(0x80 >> k);
				}

				if (b <= 1)
					f[kk] = 3;

				if ((bdr1 & 0160) != 0 && (bdr1 & 07) != 0 && (bdr1 & 0210) == 0)
					f[kk] = 3;
				else if ((bdr1 && 0301) != 0 && (bdr1 & 034) != 0 && (bdr1 & 042) == 0)
					f[kk] = 3;
				else if ((bdr1 & 0202) == 0 && (bdr1 & 01) != 0)
					f[kk] = 3;
				else if ((bdr1 & 0240) == 0 && (bdr1 & 0100) != 0)
					f[kk] = 3;
				else if ((bdr1 & 050) == 0 && (bdr1 & 020) != 0)
					f[kk] = 3;
				else if ((bdr1 & 012) == 0 && (bdr1 & 04) != 0)
					f[kk] = 3;
			}
		}
		for (i = 1; i<lx - 1; i++)
		{
			for (j = 1; j<ly - 1; j++)
			{
				kk = i*ly + j;
				if (!f[kk])
					continue;

				kk1 = kk - ly - 1;
				kk2 = kk1 + 1;
				kk3 = kk2 + 1;
				n[3] = f[kk1];
				n[2] = f[kk2];
				n[1] = f[kk3];
				kk1 = kk - 1;
				kk2 = kk + 1;
				n[4] = f[kk1];
				n[0] = f[kk3];
				kk1 = kk + ly - 1;
				kk2 = kk1 + 1;
				kk3 = kk2 + 1;
				n[5] = f[kk1];
				n[6] = f[kk2];
				n[7] = f[kk3];
				bdr1 = bdr2 = 0;

				for (k = 0; k <= 7; k++)
				{
					if (n[k] >= 1)
						bdr1 |= 0x80 >> k;
					if (n[k] >= 2)
						bdr2 |= 0x80 >> k;
				}

				if (bdr1 == bdr2)
				{
					f[kk] = 4;
					continue;
				}

				if (f[kk] != 2)
					continue;

				if ((bdr2 & 0200) != 0 && (bdr1 & 010) == 0 &&
					((bdr1 & 0100) != 0 && (bdr1 & 001) != 0 ||
						((bdr1 & 0100) != 0 || (bdr1 & 001) != 0) && (bdr1 & 060) != 0 && (bdr1 & 06) != 0))
				{
					f[kk] = 4;
				}

				else if ((bdr2 & 040) != 0 && (bdr1 & 02) == 0 &&
					((bdr1 & 020) != 0 && (bdr1 & 0100) != 0 ||
						((bdr1 & 020) != 0 || (bdr1 & 0100) != 0) &&
						(bdr1 & 014) != 0 && (bdr1 & 0201) != 0))
				{
					f[kk] = 4;
				}

				else if ((bdr2 & 010) != 0 && (bdr1 & 0200) == 0 &&
					((bdr1 & 04) != 0 && (bdr1 & 020) != 0 ||
						((bdr1 & 04) != 0 || (bdr1 & 020) != 0) &&
						(bdr1 & 03) != 0 && (bdr1 & 0140) != 0))
				{
					f[kk] = 4;
				}

				else if ((bdr2 & 02) != 0 && (bdr1 & 040) == 0 &&
					((bdr1 & 01) != 0 && (bdr1 & 04) != 0 ||
						((bdr1 & 01) != 0 || (bdr1 & 04) != 0) &&
						(bdr1 & 0300) != 0 && (bdr1 & 030) != 0))
				{
					f[kk] = 4;
				}
			}
		}

		for (i = 1; i<lx - 1; i++)
		{
			for (j = 1; j<ly - 1; j++)
			{
				kk = i*ly + j;
				if (f[kk] != 2)
					continue;
				kk1 = kk - ly - 1;
				kk2 = kk1 + 1;
				kk3 = kk2 + 1;
				n[3] = f[kk1];
				n[2] = f[kk2];
				n[1] = f[kk3];
				kk1 = kk - 1;
				kk2 = kk + 1;
				n[4] = f[kk1];
				n[0] = f[kk3];
				kk1 = kk + ly - 1;
				kk2 = kk1 + 1;
				kk3 = kk2 + 1;
				n[5] = f[kk1];
				n[6] = f[kk2];
				n[7] = f[kk3];
				bdr4 = bdr5 = 0;
				for (k = 0; k <= 7; k++)
				{
					if (n[k] >= 4)
						bdr4 |= 0x80 >> k;
					if (n[k] >= 5)
						bdr5 |= 0x80 >> k;
				}
				if ((bdr4 & 010) == 0)
				{
					f[kk] = 5;
					continue;
				}
				if ((bdr4 & 040) == 0 && bdr5 == 0)
				{
					f[kk] = 5;
					continue;
				}
				if (f[kk] == 3 || f[kk] == 4)
					f[kk] = c;
			}
		}

		erase = 0;
		for (i = 1; i<lx - 1; i++)
		{
			for (j = 1; j<ly - 1; j++)
			{
				kk = i*ly + j;
				if (f[kk] == 2 || f[kk] == 5)
				{
					erase = 1;
					f[kk] = 0;
				}
			}
		}
	}
}

FILE*	openTextFile(char* filename, char* mode) {
	FILE*	fp;
	if ((fp = fopen(filename, mode)) == NULL)
	{
		printErrors("open file failed");
		exit(0);
	}
	return fp;
}


// 反转二值图
void	reverseBinImg(BYTE* biImg, LONG width, LONG heigh) {
	LONG i, totalPix = width*heigh;

	for ( i = 0; i < totalPix; i++)
	{
		biImg[i] = ~biImg[i];
	}
}