#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "bitmapStruct.h"
#include "bmpManage.h"
#include "preprocess.h"

typedef	struct bounds
{
	LONG	up;
	LONG	down;
	LONG	left;
	LONG	right;
}BOUNDS, *PBOUNDS;
//LONG*	myRadon(BYTE *imageArr8, int *thetaPtr, LONG width, LONG height, int numAngles);

int		judgePLClr(const PHSV myHSV, LONG width, LONG height);
int		judgePixClr(double H, double S, double V);
BYTE*	binarization_PLClr(BYTE* imageArr24, LONG width, LONG height, PHSV myHSV, int PLClr);
int		judgePLCharClr(int PLClr);
int		getProBmp(BYTE* PLProEdgeBmpArr8, BYTE* charProBmpArr8, LONG width, LONG height, PHSV myHSV, int PLClr);
BYTE*	getPLEdgeBmpArr(const BYTE* imageArr24, LONG width, LONG height, int PLClr);
// 如果以当前白色点为中心的九宫格中第一行和最后一行都是黑色点，则将该点置为黑点
int		removeHorAloneNoise(BYTE* imageArr8, LONG width, LONG height);
// 粗定位水平边界，成功返回1，失败返回0
int		getHorBound_CLS(const BYTE* imageArr8, LONG width, LONG height, PBOUNDS bound);
// 粗定位垂直平边界，成功返回1，失败返回0
int		getVerBound_CLS(const BYTE* imageArr8, LONG width, LONG height, PBOUNDS bound,double ratio);
int		checkBound(const PBOUNDS bound);
// 第一次粗定位，0：成功，1：水平定位失败；2:垂直定位失败
int		firstLocatePL(const BYTE* bmpArr24, LONG width, LONG height, PBOUNDS bound, int PLClr);
int		secLocatePL(BYTE* PLProEdgeBmpArr8, LONG width, LONG height, PBOUNDS bound, int PLClr);
BYTE*	extractBmpByBounds(const BYTE* bmpArr, LONG widht, LONG height, PBOUNDS bound, WORD wType);

int		getHorContiPix(BYTE* imageArr8, LONG width, LONG heigh, PBOUNDS bound);
// 用一个3*12的矩形逐步探测蓝色前景图，如果白点个数超过阈值，则全部置为白点，否则置为黑点
int		removeNoise_rect(BYTE* imageArr8, LONG width, LONG heigh);
//********************************
// 全局变量
BITMAP_IMAGE bmpImage;
BYTE	*PLProEdgeBmpArr8;  // 车牌底色边缘点
BYTE	*charProBmpArr8; // 字符底色边缘点
BYTE	*PLProBmpArr8;  // 车牌底色边缘点
HSV*	myHSV = NULL;

char*		dest;
int		locatePL_clr(char* srcFile, char* destFile) {
	FILE	*fpSrc = NULL, *fpDest = NULL;
	LONG	i, j, k, temp = 0;
	BYTE	*bmpArr8, *bmpArr24, *firstLocateBmpArr, *secLocateBmpArr, *tempArr, *rotateBmpArr;
	int		res, status = 0;
	int		iRotateAngle = 0;
	LONG	origWidth, origHeight, locateWidth, locateHeight, rotateWidth, rotateHeight
		, cWidth=0, cHeight=0;

	const WORD	wTpye = BF_TYPE_24;		//指定提取的对象是8位灰度还是24位真色子图		
	BOUNDS	bound;			// 四个边界，分别代表上下左右
	int		PLClr = 0;

	dest = destFile;

	fpSrc = loadImage(srcFile, &bmpImage);
	//printBmpHeader(&bmpImage);
	bmpArr24 = creatImageArr(fpSrc, &bmpImage);
	origWidth = bmpImage.infoHeader.biWidth;
	origHeight = bmpImage.infoHeader.biHeight;
	for (PLClr = 0; PLClr < 2; PLClr++)
	{
		res = firstLocatePL(bmpArr24, origWidth, origHeight, &bound, PLClr);
		if (res == 0)
		{	// 第一次定位成功
			status = 1;
			break;
		}
		if (res != 0)
		{	// 第一次定位失败
			bound.up = -1;		// 初始边界，避免第一次定位失败对第二次定位的影响
			bound.down = -1;
			bound.left = -1;
			bound.right = -1;
			res = secLocatePL(PLProEdgeBmpArr8, origWidth, origHeight, &bound, PLClr);
			//creatBmpByArr(destFile, &bmpImage, PLProEdgeBmpArr8, 8);
			if (res == 0)
			{ // 第二次定位成功
				status = 2;
				break;
			}
		}
	}
	printf("车牌颜色为: %d \n", PLClr);
	//printf("%d \n", PLProEdgeBmpArr8[0]);
	//bmpArr8 = binarization_PLClr(bmpArr24, origWidht, origHeight, myHSV, 0);
	//medianFilter(bmpArr8, origWidht, origHeight);
	
	//getHorBound_Clr(bmpArr8,origWidht, origHeight, bound);
	//getVerBound(bmpArr8, origWidht, origHeight, bound);
	//drawBound(bmpArr8, origWidht, origHeight, bound);
	
//	printf("%d \n", PLClr);
	if (res == 0)
	{  // 定位成功
		status = 1;
		tempArr = extractBmpByBounds(bmpArr24, origWidth, origHeight, &bound, 24);
		locateWidth = bound.right - bound.left + 1;
		locateHeight = bound.up - bound.down + 1;
		//tempArr = extractBmpByBounds(PLProBmpArr8, origWidth, origHeight, &bound, 8);
		
		//dilation(tempArr, locateWidth, locateHeight);
		//sobleSideEnhance(tempArr, locateWidth, locateHeight, 0);
		//iRotateAngle = hough(tempArr, locateWidth, locateHeight);
	//	printf("%d \n", iRotateAngle);
		//rotateBmpArr = rotateRGB(tempArr, 0, locateWidth, locateHeight, &rotateWidth, &rotateHeight);
		rotateBmpArr = correction(tempArr, locateWidth, locateHeight, &cWidth, &cHeight);

		/*bmpImage.infoHeader.biWidth = locateWidth;
		bmpImage.infoHeader.biHeight = locateHeight;*/
		bmpImage.infoHeader.biWidth = cWidth;
		bmpImage.infoHeader.biHeight = cHeight;
		//printf("%d %d \n", cWidth, cHeight);
		creatBmpByArr(destFile, &bmpImage, rotateBmpArr, 8);
		free(tempArr);
		//free(rotateBmpArr);
	}
	
	free(PLProEdgeBmpArr8);
	free(PLProBmpArr8);
	free(charProBmpArr8);
	free(myHSV);
	return status;
}

BYTE*	correction(BYTE* imageArr24, LONG width, LONG height, LONG* cWidth, LONG* cHeight) {
	BYTE* rotateBmpArr, *tempArr, *tempArr2;
	LONG	xOrigin, yOrigin, rFirst, rSize, i, j;
	int		numAngles;
	double	*thetaPtr, *radonTable, *maxArr;
	int		maxTheta;
	LONG*	RTable;
	LONG	rotatedWidth = 0, rotatedHeight = 0;
	xOrigin =0;
	yOrigin = 0;
	numAngles = 4;
	rFirst = 0;
	rSize = (LONG)sqrt(width*width + height*height) + 1;
	thetaPtr = (double*)calloc(numAngles, sizeof(double));
	for ( i = 0; i < numAngles; i++)
	{
		thetaPtr[i] = i;
	}
	radonTable = (double*)calloc(rSize*numAngles, sizeof(double));
	maxArr = (double*)calloc(numAngles, sizeof(double));
	tempArr  = rgbToGray(imageArr24, width, height);
	//histogramEqual(rotateBmpArr, locateWidth, locateHeight);
	binarization(tempArr, width, height);

	tempArr2 = (BYTE*)calloc(width*height, sizeof(BYTE));
	memcpy((BYTE*)tempArr2, (BYTE*)tempArr, width*height*sizeof(BYTE));
	sobleSideEnhance(tempArr2, width, height, 2);
	//radon(radonTable, tempArr, thetaPtr, width, height, xOrigin, yOrigin, numAngles, rFirst, rSize);
	maxTheta = KTTransform(tempArr2, width, height);
	printf("角度：%d \n", maxTheta);
	rotateBmpArr = rotate(tempArr, -maxTheta, width, height, cWidth, cHeight);
	//RTable = myRadon(tempArr, thetaPtr, width, height,numAngles);
	//printf("%d ", radonTable[0]);
	//getMaxValue
	/*for ( i = numAngles-1; i < numAngles; i++)
	{
		for ( j = 0; j < rSize; j++)
		{
			printf("%lf ", radonTable[i*rSize + j]);
		}
		printf("\n");
	}*/

	//for (i = 0; i < numAngles; i++)
	//{
	//	maxArr[i] = getMaxValue(radonTable + i*rSize, rSize);
	//	//printf("%lf ", maxArr[i]);
	//}
	
		//maxArr[i] = getMaxValue(radonTable + i*rSize, rSize);
	//maxTheta = getMaxIndex(maxArr, numAngles);
	//printf("\n %d \n", maxTheta);
	//printf("\n %d \n", getMaxIndex(radonTable+maxTheta*rSize, rSize));
	free(tempArr);
	free(tempArr2);
	return rotateBmpArr;
}
int		firstLocatePL(const BYTE* bmpArr24, LONG width, LONG height, PBOUNDS bound, int PLClr) {
	BYTE*	imageArr8;
	const	double  PLBoxAddRation = 0.35;		// 字符顶部到车牌套的距离
	int		PLBoxAdd = 0;		// 字符顶部到车牌套的距离
	LONG	i;
	int		res = 0;

	imageArr8 = getPLEdgeBmpArr(bmpArr24, width, height, PLClr);
	if (getHorBound_CLS(imageArr8, width, height, bound) == 0)
	{
		res = 1;	// 水平定位失败
	}
	else
	{
		if (getVerBound_CLS(imageArr8, width, height, bound, 4.0) == 0)
		{
			res = 2;	// 垂直定位失败
		}
		else
		{
			/*for (i = 0; i < height; i++)
			{
				radonTable[i*width + bound->left] = 255;
				radonTable[i*width + bound->right] = 255;
			}*/
			PLBoxAdd = (bound->up - bound->down) * PLBoxAddRation;
			bound->up += PLBoxAdd;			// 扩展车牌上下界，使之包含车牌框
			bound->down -= PLBoxAdd;
			if (bound->up > height)
			{
				bound->up = height;
			}
			if (bound->down < 0)
			{
				bound->down = 0;
			}
			/*for (i = 0; i < width; i++)
			{
				radonTable[bound->up*width + i] = 255;
				radonTable[bound->down*width + i] = 255;
			}*/
		}
	}
	//creatBmpByArr(dest, &bmpImage, PLProEdgeBmpArr8, 8);
	free(imageArr8);
	return res;
}

int		secLocatePL(BYTE* PLProBmpArr8w, LONG width, LONG height, PBOUNDS bound, int PLClr) {
	int		res = 1;
	LONG	i, j;
	const	double  PLBoxAddRation = 0.35;		// 字符顶部到车牌套的距离
	int		PLBoxAdd = 0;		// 字符顶部到车牌套的距离
	removeNoise_rect(PLProEdgeBmpArr8, width, height);

	if (getHorContiPix(PLProEdgeBmpArr8, width, height, bound) == 1) {
		
		res = 2;
		if (getVerBound_CLS(PLProEdgeBmpArr8, width, height, bound, 4.0) == 1) {
			PLBoxAdd = (bound->up - bound->down) * PLBoxAddRation;
			bound->up += PLBoxAdd;			// 扩展车牌上下界，使之包含车牌框
			bound->down -= PLBoxAdd;

			if (bound->up > height)
			{
				bound->up = height;
			}
			if (bound->down < 0)
			{
				bound->down = 0;
			}
			res = 0;
		}
	}
	return res;
}

int		judgePLClr(const PHSV myHSV, LONG width, LONG height) {
	double	ratio[5] = { 0 };		// 对应车牌颜色出现的概率，分别代表蓝，黄，白，黑，非车牌背景颜色
	int		cnt[5] = { 0 };			// 对应车牌颜色出现的次数，分别代表蓝，黄，白，黑，非车牌背景颜色
	int		i;
	LONG	totalPixls = width*height;
	const	double	RatioMin = 0.03;
	const	double	RatioMax = 0.1;
	int		res = -1;

	for (i = 0; i<totalPixls; ++i) 
	{
		++cnt[judgePixClr(myHSV[i].H, myHSV[i].S, myHSV[i].V)];
	}
	//printf("%d \n", i);
	for (i = 0; i<4; ++i) {
		ratio[i] = (double)cnt[i] / totalPixls;
		printf("%lf ", ratio[i]);
		if (ratio[i] >= RatioMin && ratio[i] <= RatioMax)
		{
			
			res = i;
			break;
		}
		//printf("%d %d %lf ", cnt[i], totalPixls, ratio[i]);
	}
	printf("\n");
	
	return res;
}

BYTE*	binarization_PLClr(BYTE* imageArr24, LONG width, LONG height, PHSV myHSV, int PLClr) {
	LONG	totalPixls = width*height;
	BYTE*	tempArr8 = (BYTE*)calloc(totalPixls, sizeof(BYTE));
	int		i;

	for (i = 0; i<totalPixls; ++i)
	{
		if (judgePixClr(myHSV[i].H, myHSV[i].S, myHSV[i].V) == PLClr) {
			tempArr8[i] = 255;
		}
	}

	return tempArr8;
}

int		judgePixClr(double H, double S, double V) {
	/*int		res;
	if ((H>=161 && H<=301) && (S>=0.25 && S<=1) && (V>=0.25&& V<=1))
		res = BLUE_PL;
	else if ((H >= 20 && H <= 60) && (S >= 0.25 && S <= 1) && (V >= 0.25&& V <= 1))
		res = YELLOW_PL;
	else if ((S <= 0.25) && (V >= 0.7&& V <= 1))
		res = WHITE_PL;
	else if ((V <= 0.25))
		res = BLACK_PL;
	else
		res = OTHERS_PL;
	return res;*/

	int		res;
	if ((H >= 200 && H <= 250) && (S >= 0.2 && S <= 1) && (V >= 0.15&& V <= 1))
		res = BLUE_PL;
	else if ((H >= 20 && H <= 50) && (S >= 0.3 && S <= 1) && (V >= 0.3&& V <= 1))
		res = YELLOW_PL;
	else if ((S <= 0.3) && (V >= 0.35&& V <= 1))
		res = WHITE_PL;
	else if ((V <= 0.7))
		res = BLACK_PL;
	else if ((H >= 350 && H <= 360) && (S >= 0.1 && S <= 1) && (V >= 0.35 && V <= 1))
		res = RED_PL;
	else
		res = OTHERS_PL;
	return res;
}

int		judgePLCharClr(int PLClr) {
	int		PLCharClr;
	switch (PLClr)
	{
	case BLUE_PL:		//蓝底白字
		PLCharClr = WHITE_PL;
		break;	
	case YELLOW_PL:		// 黄底黑字
		PLCharClr = BLACK_PL;
		break;
	case WHITE_PL:		// 白底黑字
		PLCharClr = BLACK_PL;
		break;
	case BLACK_PL:		// 黑底白字
		PLCharClr = WHITE_PL;
		break;
	default:
		PLCharClr = -1;
		break;
	}

	return PLCharClr;
}

int		getProBmp(BYTE* PLProEdgeBmpArr8, BYTE* charProBmpArr8, LONG width, LONG height, PHSV myHSV, int PLClr) {
	LONG	totalPixls = width*height;
	int		i;
	int		curClr;
	if (PLClr != WHITE_PL)
	{
		for (i = 0; i<totalPixls; ++i)
		{
			curClr = judgePixClr(myHSV[i].H, myHSV[i].S, myHSV[i].V);
			if (curClr == PLClr) {
				PLProEdgeBmpArr8[i] = 255;
			}
			else if (curClr == judgePLCharClr(PLClr))
			{
				charProBmpArr8[i] = 255;
			}
		}
	}
	else // 白底黑字的车牌前两个字符可能是红色
	{
		for (i = 0; i<totalPixls; ++i)
		{
			curClr = judgePixClr(myHSV[i].H, myHSV[i].S, myHSV[i].V);
			if (curClr == PLClr || curClr == RED_PL) {
				PLProEdgeBmpArr8[i] = 255;
			}
			else if (curClr == judgePLCharClr(PLClr))
			{
				charProBmpArr8[i] = 255;
			}
		}
	}
	
	//medianFilter(PLProEdgeBmpArr8, width, height);
	//medianFilter(PLProEdgeBmpArr8, width, height);
	return 0;
}

BYTE*	getPLEdgeBmpArr(const BYTE* imageArr24, LONG width, LONG height, int PLClr) {
	LONG	totalPixls = width*height;
	int		i , j;
	
	
	BYTE*	imageArr8;
	// RGB颜色空间转成HSV空间
	myHSV = RGB2HSV(imageArr24, width, height);
	PLProEdgeBmpArr8 = (BYTE*)calloc(totalPixls, sizeof(BYTE));
	PLProBmpArr8 = (BYTE*)calloc(totalPixls, sizeof(BYTE));
	charProBmpArr8 = (BYTE*)calloc(totalPixls, sizeof(BYTE));

	// 车牌定位第一步：获取蓝色和白色前景图
	getProBmp(PLProEdgeBmpArr8, charProBmpArr8, width, height, myHSV, PLClr);
	memcpy((BYTE*)PLProBmpArr8, (BYTE*)PLProEdgeBmpArr8, width*height*sizeof(BYTE));
	// 车牌定位第二步：将RGB彩色图像灰度化，进行边缘检测，并二值化，得到二值化边缘图
	imageArr8 = rgbToGray(imageArr24, width, height);
	sobleSideEnhance(imageArr8, width, height, 2);
	//histogramEqual(tempBmpArr, width, height);
	binarization(imageArr8, width, height);
	dilation(imageArr8, width, height);
	
	// 车牌定位第三步：确定图像蓝白色边缘点
	intersection(imageArr8, PLProEdgeBmpArr8, width, height); // 蓝色边缘点
	intersection(imageArr8, charProBmpArr8, width, height); // 白色边缘点
	// 判别蓝色边缘点左右是否存在有白色边缘点
	// 如有则为图像蓝白色边缘点，设为前景白色；如无则设
	// 为背景黑色；得到图像蓝白色边缘点图
	for (i = 0; i < totalPixls; i++)
	{
		if (PLProEdgeBmpArr8[i] == 255)
		{
			imageArr8[i] = (charProBmpArr8[i - 1] == 255 || charProBmpArr8[i + 1] == 255) ? 255 : 0;
		}
		else
		{
			imageArr8[i] = 0;
		}
	}
	removeHorAloneNoise(imageArr8, width, height);
	//dilation(radonTable, width, height);
	
	/*free(PLProEdgeBmpArr8);
	free(charProBmpArr8);
	free(myHSV);*/

	//printf("%d ", PLProEdgeBmpArr8[0]);
	return imageArr8;
}

int		histogramEqual(BYTE* imageArr8, LONG width, LONG height) {
	LONG		i, totalPixs = width*height;
	LONG	numPixel[256] = { 0 };
	double	probPixel[256] = { 0.0 }, cumuPixel[256] = { 0.0 };
	double	sum = 0;
	
	// 进行像素灰度统计
	for (i = 0; i < totalPixs; i++)
	{
		++numPixel[imageArr8[i]];
	}
	// 计算灰度分布密度  
	for ( i = 0; i < 256; i++)
	{
		probPixel[i] = numPixel[i] / (double)totalPixs;
		sum += probPixel[i];
		//printf("%d %lf ", numPixel[i], probPixel[i]);
	}
	
	// 计算累计直方图分布
	cumuPixel[0] = probPixel[0];
	for (i = 1; i < 256; i++)
	{
		cumuPixel[i] = cumuPixel[i - 1] + probPixel[i];
	}
	
	// 累计分布取整
	for (i = 0; i < 256; i++)
	{
		cumuPixel[i] = floor(255 * cumuPixel[i]);
	}
	
	// 对灰度值进行映射
	for (i = 0; i < totalPixs; i++)
	{
		imageArr8[i] = cumuPixel[imageArr8[i]];
	}

	return	0;
}

//膨胀运算
int		dilation(BYTE* imageArr8, int width, int height){
	LONG	i, j, m, n, index, sum, flag;
	BYTE*	tmpArr = (BYTE*)calloc(width*height, sizeof(BYTE));
	memcpy((BYTE*)tmpArr, (BYTE*)imageArr8, width*height*sizeof(BYTE));

	for (i = 1;i < height - 1;i++)
	{
		for (j = 1;j < width - 1;j++)
		{
			flag = 0;
			for (m = i - 1;m < i + 2;m++)
			{
				for (n = j - 1; n < j + 2;n++)
				{
					//自身及领域中若有一个为255
					//则将该点设为255
					if (tmpArr[m * width + n] == 255 || tmpArr[i * width + j] == 255)
					{
						flag = 1;
						break;
					}
				}
				if (flag == 1)
				{
					break;
				}
			}
			imageArr8[i * width + j] = (flag == 1) ? 255 : 0;
		}
	}

	free(tmpArr);
	return 0;
}

//膨胀运算
int		erosion(BYTE* imageArr8, int width, int height) {
	LONG	i, j, m, n, index, sum, flag;
	BYTE*	tmpArr = (BYTE*)calloc(width*height, sizeof(BYTE));
	memcpy((BYTE*)tmpArr, (BYTE*)imageArr8, width*height*sizeof(BYTE));

	for (i = 1;i < height - 1;i++)
	{
		for (j = 1;j < width - 1;j++)
		{
			flag = 0;
			for (m = i - 1;m < i + 2;m++)
			{
				for (n = j - 1; n < j + 2;n++)
				{
					//自身及领域中若有一个为0,则将该点设为0
					if (tmpArr[m * width + n] == 0 || tmpArr[i * width + j] == 0)
					{
						flag = 1;
						break;
					}
				}
				if (flag == 1)
				{
					break;
				}
			}
			imageArr8[i * width + j] = (flag == 1) ? 0 : 255;
		}
	}

	free(tmpArr);
	return 0;
}

int		intersection(const BYTE* sorce, BYTE* dest, LONG width, LONG height) {
	LONG	i, totalPixs = width*height;

	for (i = 0; i < totalPixs; i++)
	{
		if (dest[i] == 255)
		{
			dest[i] = (sorce[i] == 255) ? 255 : 0;
		}
	}
}

int		removeHorAloneNoise(BYTE* imageArr8, LONG width, LONG height) {
	LONG	i, j, curIndex;
	int		flag = 0, k;

	for ( i = 1; i < height-1; i++)
	{
		for ( j = 1; j < width-1; j++)
		{
			flag = 0;
			curIndex = i*width + j;
			if (imageArr8[curIndex] == 255)
			{
				for ( k = -1; k < 2; k++)
				{
					//flag = (radonTable[(i - 1)*width + j + k] == 255);
					if (imageArr8[(i - 1)*width + j + k] == 255)
					{
						flag = 1;
						break;
					}
					if (imageArr8[(i + 1)*width + j + k] == 255)
					{
						flag = 1;
						break;
					}
				}
				if (flag == 0)
				{
					imageArr8[curIndex] = 0;
				}
			}
		}
	}
}

int		getHorBound_CLS(const BYTE* imageArr8, LONG width, LONG height, PBOUNDS bound) {
	int		i, j, k, m, n, q = 0, tempCnt = 0;
	const BYTE	JUMP_SUM_MIN = 10;			//黑白跳变的个数
	const LONG	SPEC_LINE_MIN = 10;	//满足所有要求的短间隔连续行的上下界
	LONG*	iJumpSum = (LONG*)calloc(height, sizeof(LONG));	// 记录第j行的黑白跳变次数
	LONG*	tempPos = (LONG*)calloc(height, sizeof(LONG));
	LONG	iDown = -1, iUp = -1;
	
	for (j = 0, q=0; j<height; ++j) {
		for (i = 1, k = 0; i<width; ++i) {
			if (imageArr8[j*width + i] - imageArr8[j*width + i - 1] == 255) {
				++iJumpSum[j];
			}
		}
		
		if (iJumpSum[j] >= JUMP_SUM_MIN) {
			tempPos[q++] = j;
		}
	}
	tempCnt = 0;
	iDown = tempPos[0];
	for (i = 0; i<q - 1; i++) {
		if (tempPos[i + 1] - tempPos[i] <= 5) { // 放松标准，防止由于车牌区域中间某些行不满足要求而判错
			++tempCnt;
		}
		else 
		{
			if (tempCnt >= SPEC_LINE_MIN) {
				iUp = tempPos[i];
				break;
			}
			else {
				iDown = tempPos[i + 1];
				tempCnt = 0;
			}
		}

	}
	if (i <= q && tempCnt >= SPEC_LINE_MIN) {
		iUp = tempPos[i];
	}
	if (iUp > iDown)		// 使得车牌的边界扩展至包括车牌边框位置
	{
		/*bound->up = (iUp - iDown) * PLBoxAddRation + iUp;
		bound->down = iDown - (iUp - iDown) * PLBoxAddRation;*/
		bound->up =iUp;
		bound->down = iDown;
		return 1;
	}
	else
	{
		bound->up = iUp;
		bound->down = iDown;
	}
	
	free(iJumpSum);
	free(tempPos);

	return 0;
}

int		getVerBound_CLS(const BYTE* imageArr, LONG width, LONG height, PBOUNDS bound, double ratio) {
	//double	ratio = 3.0;
	LONG	iHeight = bound->up - bound->down + 1;
	LONG	iWidth = 0;
	LONG	iPerWidth = iWidth / 7;
	int		STEP = 5;
	LONG	i, j, k;
	int		whiteCnt[7] = { 0 };
	double		totalPerBoxPixs = 0.0;
	//const	double	WHITE_RATIO_MIN = 0;			// 白点所在的比例
	const	double	WHITE_CNT_MIN = 10;
	const	double	WHITE_RATIO_MAX = 1;
	int		flag = 1;
	int		tmpCnt = 0;
	bound->left = -1;
	bound->right = -1;
	//printf("\n%d %d \n", bound->up, bound->down);
	while (1) {
		iWidth = (LONG)iHeight*ratio;
		totalPerBoxPixs = iHeight*iWidth / 7;
		iPerWidth = iWidth / 7 + 1;
		for (k = 0; k<width - STEP; k += STEP) {// 每次向前移动5个像素
			for (j = bound->down; j <= bound->up; ++j) {//上下边界
				for (i = k; i<iWidth + k; ++i) {//车牌宽度范围内进行检查
					if (imageArr[j*width + i] == 255) { //统计7个小盒子中的白点个数
						++whiteCnt[(i - k) / iPerWidth];
					}
				}
			}
			for (j = 0; j<7; ++j) {
				if (whiteCnt[j] < WHITE_CNT_MIN) {
					//if (whiteCnt[j] / totalPerBoxPixs<WHITE_RATIO_MIN || whiteCnt[j] / totalPerBoxPixs>WHITE_RATIO_MAX) {
					flag = 0;
					break;
				}
			}
			if (flag == 1) {		// 找到垂直边界
				bound->left = k;
				bound->right = k + iWidth;
				// 扩展右边界的范围

				for (k = bound->right; k < width - iPerWidth; k += iPerWidth) {// 每次向前移动5个像素
					flag = 0;
					for (j = bound->down; j <= bound->up; ++j) {//上下边界
						for (i = k; i < iPerWidth + k; ++i) {//车牌宽度范围内进行检查
							if (imageArr[j*width + i] == 255) { //统计当前个小盒子中的白点个数
								tmpCnt++;
								if (tmpCnt >= WHITE_CNT_MIN)
								{
									flag = 1;
									break;
								}
							}
						}
					}
					if (flag == 1)
					{
						bound->right += iPerWidth;
						if (bound->right > width) // 防止越界
						{
							bound->right = width;
						}
					}
					else
					{
						break;
					}
				}
				//printf("======%d %d\n", bound->left, bound->right);
				return 1;
			}
			for (j = 0; j<7; ++j) {
				whiteCnt[j] = 0;
			}
			flag = 1;
		}
		ratio += 0.5;		// 调节宽度
		if (ratio >= 5) {
			break;
		}
	}
	
	
	/*for (i = 0; i < origHeight; i++)
	{
		tempArr[i*origWidth + bound.left] = 255;
		tempArr[i*origWidth + bound.right] = 255;
	}*/
	//printf("垂直定位失败");
	return 0;
}

int		checkBound(const PBOUNDS bound) {
	return (bound->up > bound->down && bound->right > bound->left);
}

BYTE*	extractBmpByBounds(const BYTE* bmpArr, LONG widht, LONG height, PBOUNDS bound, WORD wType) {
	int		flag = (wType == 8) ? 1 : 3;
	BYTE	*tempArr = (BYTE *)calloc((bound->up - bound->down + 1)*(bound->right - bound->left + 1)*flag, sizeof(BYTE));
	int		i, j, k, g;
	LONG	tmpIndex;

	if (bound->up>bound->down && bound->right>bound->left) { // 满足要求的定位边界
		for (i = bound->down, k = 0; i <= bound->up; ++i) {
			for (j = bound->left; j <= bound->right; ++j) {
				if (wType == 8) {
					tempArr[k++] = bmpArr[i*widht + j];
				}
				else if (wType == 24) {
					tmpIndex = (i*widht + j) * 3;
					tempArr[k] = bmpArr[tmpIndex];  // 注意此处的赋值
					tempArr[k + 1] = bmpArr[tmpIndex + 1];
					tempArr[k + 2] = bmpArr[tmpIndex + 2];
					k += 3;
				}
			}
		}
	}
	return tempArr;
}

int		removeNoise_rect(BYTE* imageArr8, LONG width, LONG heigh) {
	const	int	rectHeight = 3;
	const	int rectWidth = 10;
	int		i, j, m, n;
	int		reqTotalPixs = (int)rectWidth*rectHeight*0.5; 
	int		cnt = 0;
	for ( i = 0; i < heigh- rectHeight; i+= rectHeight)
	{
		for ( j = 0; j < width- rectWidth; j+= rectWidth)
		{
	/*for (i = rectHeight; i < heigh - rectHeight; i += rectHeight)
	{
		for (j = rectWidth; j < width - rectWidth; j += rectWidth)
		{*/
			cnt = 0;
			for ( m = i; m < i+ rectHeight; m++)
			{
				for ( n = j; n < j+rectWidth; n++)
				{
					if (imageArr8[m*width+n] == 255)
					{
						cnt++;
					}
				}
			}
			if (cnt < reqTotalPixs)
			{
				for (m = i; m < i + rectHeight; m++)
				{
					for (n = j; n < j + rectWidth; n++)
					{
						imageArr8[m*width + n] = 0;
					}
				}
			}
			else
			{
				/*for (m = i; m < i + rectHeight; m++)
				{
					for (n = j; n < j + rectWidth; n++)
					{
						radonTable[m*width + n] = 255;
					}
				}*/
			}
		}
	}
}

int		getHorContiPix(BYTE* imageArr8, LONG width, LONG heigh, PBOUNDS bound) {
	LONG	i, j, k, q,t;
	LONG	*pos = (LONG*)calloc(width, sizeof(LONG)); // 记录每行白点的位置
	LONG	*line = (LONG*)calloc(width, sizeof(LONG));	//记录每行连续白点数量超过一定阈值的行
	LONG	tempCnt = 0;
	const	double  PLBoxAddRation = 0.35;		// 字符顶部到车牌套的距离
	int		PLBoxAdd = 0;		// 字符顶部到车牌套的距离
	const	LONG	CNT_MIN = 10;			// 每行连续白点数量的阈值
	const	LONG	SPEC_LINE_MIN = 10;		// 连续SPEC_LINE_MIN行满足要求，则判定为车牌区域
	LONG	iDown = -1, iUp = -1;
	int		res = 0;
	for ( i = 0, q=0; i < heigh; i++)
	{
		tempCnt = 0;
		k = 0;
		for ( j = 0; j < width; j++)
		{
			if (imageArr8[i*width+j] == 255)
			{
				pos[k++] = j;
			}
		}
		//iDown = pos[0];
		for (j = 0; j<k - 1; j++) {
			//printf("%d ", pos[j]);
			if (pos[j + 1] - pos[j] <= 8) { // 放松标准，防止由于车牌区域中间某些行不满足要求而判错
				++tempCnt;
			}
			else
			{
				if (tempCnt >= CNT_MIN) {
					line[q++] = i;
					//printf("%d ", line[q-1]);
					/*for (t = 0; t < width/3; t++)
					{
						radonTable[i*width + t] = 255;
						radonTable[i*width + t] = 255;
					}*/
					break;
				}
				else {
					
					tempCnt = 0;
				}
			}

		}
		for ( t = 0; t < k; t++)
		{
			pos[i] = 0;
		}
		//printf("\n");
	}

	tempCnt = 0;
	iDown = line[0];
	for ( i = 0; i < q-1; i++)
	{
		//printf("%d ", line[i]);
		if (line[i + 1] - line[i] <= 5)
		{
			tempCnt++;
		}
		else
		{
			if (tempCnt >= SPEC_LINE_MIN)
			{
				iUp = line[i];
				break;
			}
			else
			{	
				iDown = line[i+1];
				tempCnt = 0;
			}
		}
	}
	if (i <= q && tempCnt >= SPEC_LINE_MIN) {
		iUp = line[i];
	}
	
	if (iUp > iDown)
	{
		bound->up = iUp;
		bound->down = iDown;
		
		//PLBoxAdd = (int)(iUp - iDown) * PLBoxAddRation;
		//bound->up += PLBoxAdd;			// 扩展车牌上下界，使之包含车牌框
		//bound->down -= PLBoxAdd;
		//printf("\n %d %d %d %d \n", iUp, iDown, bound->up, bound->down);
		/*for (j = 0; j < width; j++)
		{
			radonTable[bound->up*width + j] = 255;
			radonTable[bound->down*width + j] = 255;
		}*/
		res =  1;
	}
	
	free(pos);
	free(line);

	return res;
}

BYTE *rotateRGB(BYTE *image, float iRotateAngle, LONG width, LONG height, LONG *lwidth, LONG *lheight)
{

	int		i, j, k, m, n;
	long    lNewWidth;
	long    lNewHeight;
	float	gray;
	long    i0;
	long    j0;
	float   fRotateAngle;
	float   fSina, fCosa;
	float   fSrcX1, fSrcY1, fSrcX2, fSrcY2, fSrcX3, fSrcY3, fSrcX4, fSrcY4;
	float   fDstX1, fDstY1, fDstX2, fDstY2, fDstX3, fDstY3, fDstX4, fDstY4;
	BYTE* temp;
	float   f1, f2;
	float carCos[] = {
		1.000000, 0.999848, 0.999391, 0.998630, 0.997564, 0.996195, 0.994522, 0.992546, 0.990268, 0.987688, 0.984808, 0.981627, 0.978148, 0.974370, 0.970296, 0.965926,
		0.961262, 0.956305, 0.951057, 0.945519, 0.939693, 0.933580, 0.927184, 0.920505, 0.913545, 0.906308, 0.898794, 0.891007, 0.882948, 0.874620, 0.866025, 0.857167,
		0.848048, 0.838671, 0.829038, 0.819152, 0.809017, 0.798636, 0.788011, 0.777146, 0.766044, 0.754710, 0.743145, 0.731354, 0.719340, 0.707107, 0.694658, 0.681998,
		0.669131, 0.656059, 0.642788, 0.629320, 0.615662, 0.601815, 0.587785, 0.573576, 0.559193, 0.544639, 0.529919, 0.515038, 0.500000, 0.484810, 0.469472, 0.453991,
		0.438371, 0.422618, 0.406737, 0.390731, 0.374607, 0.358368, 0.342020, 0.325568, 0.309017, 0.292372, 0.275637, 0.258819, 0.241922, 0.224951, 0.207912, 0.190809,
		0.173648, 0.156434, 0.139173, 0.121869, 0.104528, 0.087156, 0.069757, 0.052336, 0.034900, 0.017452, 0.000000, -0.017452, -0.034899, -0.052336, -0.069756, -0.087156,
		-0.104528, -0.121869, -0.139173, -0.156434, -0.173648, -0.190809, -0.207912, -0.224951, -0.241922, -0.258819, -0.275637, -0.292372, -0.309017, -0.325568, -0.342020, -0.358368,
		-0.374607, -0.390731, -0.406737, -0.422618, -0.438371, -0.453990, -0.469472, -0.484810, -0.500000, -0.515038, -0.529919, -0.544639, -0.559193, -0.573576, -0.587785, -0.601815,
		-0.615661, -0.629320, -0.642788, -0.656059, -0.669131, -0.681998, -0.694658, -0.707107, -0.719340, -0.731354, -0.743145, -0.754710, -0.766044, -0.777146, -0.788011, -0.798635,
		-0.809017, -0.819152, -0.829038, -0.838671, -0.848048, -0.857167, -0.866025, -0.874620, -0.882948, -0.891007, -0.898794, -0.906308, -0.913545, -0.920505, -0.927184, -0.933580,
		-0.939693, -0.945519, -0.951056, -0.956305, -0.961262, -0.965926, -0.970296, -0.974370, -0.978148, -0.981627, -0.984808, -0.987688, -0.990268, -0.992546, -0.994522, -0.996195,
		-0.997564, -0.998630, -0.999391, -0.999848, -1.000000
	};

	float carSin[] = {
		0.000000, 0.017452, 0.034899, 0.052336, 0.069756, 0.087156, 0.104528, 0.121869, 0.139173, 0.156434, 0.173648, 0.190809, 0.207912, 0.224951, 0.241922, 0.258819,
		0.275637, 0.292372, 0.309017, 0.325568, 0.342020, 0.358368, 0.374607, 0.390731, 0.406737, 0.422618, 0.438371, 0.453990, 0.469472, 0.484810, 0.500000, 0.515038,
		0.529919, 0.544639, 0.559193, 0.573576, 0.587785, 0.601815, 0.615661, 0.629320, 0.642788, 0.656059, 0.669131, 0.681998, 0.694658, 0.707107, 0.719340, 0.731354,
		0.743145, 0.754710, 0.766044, 0.777146, 0.788011, 0.798635, 0.809017, 0.819152, 0.829038, 0.838671, 0.848048, 0.857167, 0.866025, 0.874620, 0.882948, 0.891007,
		0.898794, 0.906308, 0.913545, 0.920505, 0.927184, 0.933580, 0.939693, 0.945519, 0.951056, 0.956305, 0.961262, 0.965926, 0.970296, 0.974370, 0.978148, 0.981627,
		0.984808, 0.987688, 0.990268, 0.992546, 0.994522, 0.996195, 0.997564, 0.998630, 0.999391, 0.999848, 1.000000, 0.999848, 0.999391, 0.998630, 0.997564, 0.996195,
		0.994522, 0.992546, 0.990268, 0.987688, 0.984808, 0.981627, 0.978148, 0.974370, 0.970296, 0.965926, 0.961262, 0.956305, 0.951057, 0.945519, 0.939693, 0.933580,
		0.927184, 0.920505, 0.913545, 0.906308, 0.898794, 0.891007, 0.882948, 0.874620, 0.866025, 0.857167, 0.848048, 0.838671, 0.829038, 0.819152, 0.809017, 0.798636,
		0.788011, 0.777146, 0.766044, 0.754710, 0.743145, 0.731354, 0.719340, 0.707107, 0.694658, 0.681998, 0.669131, 0.656059, 0.642788, 0.629320, 0.615662, 0.601815,
		0.587785, 0.573576, 0.559193, 0.544639, 0.529919, 0.515038, 0.500000, 0.484810, 0.469472, 0.453991, 0.438371, 0.422618, 0.406737, 0.390731, 0.374607, 0.358368,
		0.342020, 0.325568, 0.309017, 0.292372, 0.275637, 0.258819, 0.241922, 0.224951, 0.207912, 0.190809, 0.173648, 0.156435, 0.139173, 0.121869, 0.104529, 0.087156,
		0.069757, 0.052336, 0.034900, 0.017452, 0.000000
	};

	if (iRotateAngle >= 0)
	{
		fSina = (float)carSin[(int)iRotateAngle];
		fCosa = (float)carCos[(int)iRotateAngle];
	}
	else
	{
		fSina = 0 - (float)carSin[0 - (int)iRotateAngle];
		fCosa = (float)carCos[0 - (int)iRotateAngle];
	}

	fSrcX1 = (float)(-(width - 1) / 2);
	fSrcY1 = (float)((height - 1) / 2);
	fSrcX2 = (float)((width - 1) / 2);
	fSrcY2 = (float)((height - 1) / 2);
	fSrcX3 = (float)(-(width - 1) / 2);
	fSrcY3 = (float)(-(height - 1) / 2);
	fSrcX4 = (float)((width - 1) / 2);
	fSrcY4 = (float)(-(height - 1) / 2);


	fDstX1 = fCosa * fSrcX1 + fSina * fSrcY1;
	fDstY1 = -fSina * fSrcX1 + fCosa * fSrcY1;
	fDstX2 = fCosa * fSrcX2 + fSina * fSrcY2;
	fDstY2 = -fSina * fSrcX2 + fCosa * fSrcY2;
	fDstX3 = fCosa * fSrcX3 + fSina * fSrcY3;
	fDstY3 = -fSina * fSrcX3 + fCosa * fSrcY3;
	fDstX4 = fCosa * fSrcX4 + fSina * fSrcY4;
	fDstY4 = -fSina * fSrcX4 + fCosa * fSrcY4;

	lNewWidth = (long)(fMax(fabs(fDstX4 - fDstX1), fabs(fDstX3 - fDstX2)) + 0.5);

	lNewHeight = (long)(fMax(fabs(fDstY4 - fDstY1), fabs(fDstY3 - fDstY2)) + 0.5);
	//unsigned char *temp=myMalloc(lNewHeight*lNewWidth*3,0,0);  
	temp = (BYTE*)calloc(lNewHeight*lNewWidth * 3, sizeof(BYTE));

	f1 = (float)(-0.5 * (lNewWidth - 1) * fCosa - 0.5 * (lNewHeight - 1) * fSina
		+ 0.5 * (width - 1));
	f2 = (float)(0.5 * (lNewWidth - 1) * fSina - 0.5 * (lNewHeight - 1) * fCosa
		+ 0.5 * (height - 1));

	for (i = 0; i < lNewHeight; i++)
	{
		for (m = 0, j = 0;j < lNewWidth, m<lNewWidth * 3;m += 3, j++)
		{
			i0 = (long)(-((float)j) * fSina + ((float)i) * fCosa + f2 + 0.5);
			j0 = (long)(((float)j) * fCosa + ((float)i) * fSina + f1 + 0.5);

			if ((j0 >= 0) && (j0 < width) && (i0 >= 0) && (i0 < height))
			{
				n = i0 * width * 3 + j0 * 3;
				*(temp + lNewWidth * i * 3 + m + 1) = *(image + n + 1);
				*(temp + lNewWidth * i * 3 + m + 2) = *(image + n + 2);
				*(temp + lNewWidth * i * 3 + m) = *(image + n);
			}
			else
			{
				*(temp + lNewWidth * i * 3 + m + 1) = 0;
				*(temp + lNewWidth * i * 3 + m + 2) = 0;
				*(temp + lNewWidth * i * 3 + m) = 0;
			}
		}
	}

	*lwidth = lNewWidth;
	*lheight = lNewHeight;

	return temp;

}

int		hough(BYTE* imageArr, int width, int height)
{
	int kmax = 0;
	int pmax = 0;
	int yuzhi = 0;
	int i, j, k, m, n, p;
	int mp = (int)(sqrt(width*width + height*height) + 1);
	int ma = 180;//180  
	int ap;
	int npp[180][1000] = { 0 };
	float carCos[] = {
		1.000000, 0.999848, 0.999391, 0.998630, 0.997564, 0.996195, 0.994522, 0.992546, 0.990268, 0.987688, 0.984808, 0.981627, 0.978148, 0.974370, 0.970296, 0.965926,
		0.961262, 0.956305, 0.951057, 0.945519, 0.939693, 0.933580, 0.927184, 0.920505, 0.913545, 0.906308, 0.898794, 0.891007, 0.882948, 0.874620, 0.866025, 0.857167,
		0.848048, 0.838671, 0.829038, 0.819152, 0.809017, 0.798636, 0.788011, 0.777146, 0.766044, 0.754710, 0.743145, 0.731354, 0.719340, 0.707107, 0.694658, 0.681998,
		0.669131, 0.656059, 0.642788, 0.629320, 0.615662, 0.601815, 0.587785, 0.573576, 0.559193, 0.544639, 0.529919, 0.515038, 0.500000, 0.484810, 0.469472, 0.453991,
		0.438371, 0.422618, 0.406737, 0.390731, 0.374607, 0.358368, 0.342020, 0.325568, 0.309017, 0.292372, 0.275637, 0.258819, 0.241922, 0.224951, 0.207912, 0.190809,
		0.173648, 0.156434, 0.139173, 0.121869, 0.104528, 0.087156, 0.069757, 0.052336, 0.034900, 0.017452, 0.000000, -0.017452, -0.034899, -0.052336, -0.069756, -0.087156,
		-0.104528, -0.121869, -0.139173, -0.156434, -0.173648, -0.190809, -0.207912, -0.224951, -0.241922, -0.258819, -0.275637, -0.292372, -0.309017, -0.325568, -0.342020, -0.358368,
		-0.374607, -0.390731, -0.406737, -0.422618, -0.438371, -0.453990, -0.469472, -0.484810, -0.500000, -0.515038, -0.529919, -0.544639, -0.559193, -0.573576, -0.587785, -0.601815,
		-0.615661, -0.629320, -0.642788, -0.656059, -0.669131, -0.681998, -0.694658, -0.707107, -0.719340, -0.731354, -0.743145, -0.754710, -0.766044, -0.777146, -0.788011, -0.798635,
		-0.809017, -0.819152, -0.829038, -0.838671, -0.848048, -0.857167, -0.866025, -0.874620, -0.882948, -0.891007, -0.898794, -0.906308, -0.913545, -0.920505, -0.927184, -0.933580,
		-0.939693, -0.945519, -0.951056, -0.956305, -0.961262, -0.965926, -0.970296, -0.974370, -0.978148, -0.981627, -0.984808, -0.987688, -0.990268, -0.992546, -0.994522, -0.996195,
		-0.997564, -0.998630, -0.999391, -0.999848, -1.000000
	};

	float carSin[] = {
		0.000000, 0.017452, 0.034899, 0.052336, 0.069756, 0.087156, 0.104528, 0.121869, 0.139173, 0.156434, 0.173648, 0.190809, 0.207912, 0.224951, 0.241922, 0.258819,
		0.275637, 0.292372, 0.309017, 0.325568, 0.342020, 0.358368, 0.374607, 0.390731, 0.406737, 0.422618, 0.438371, 0.453990, 0.469472, 0.484810, 0.500000, 0.515038,
		0.529919, 0.544639, 0.559193, 0.573576, 0.587785, 0.601815, 0.615661, 0.629320, 0.642788, 0.656059, 0.669131, 0.681998, 0.694658, 0.707107, 0.719340, 0.731354,
		0.743145, 0.754710, 0.766044, 0.777146, 0.788011, 0.798635, 0.809017, 0.819152, 0.829038, 0.838671, 0.848048, 0.857167, 0.866025, 0.874620, 0.882948, 0.891007,
		0.898794, 0.906308, 0.913545, 0.920505, 0.927184, 0.933580, 0.939693, 0.945519, 0.951056, 0.956305, 0.961262, 0.965926, 0.970296, 0.974370, 0.978148, 0.981627,
		0.984808, 0.987688, 0.990268, 0.992546, 0.994522, 0.996195, 0.997564, 0.998630, 0.999391, 0.999848, 1.000000, 0.999848, 0.999391, 0.998630, 0.997564, 0.996195,
		0.994522, 0.992546, 0.990268, 0.987688, 0.984808, 0.981627, 0.978148, 0.974370, 0.970296, 0.965926, 0.961262, 0.956305, 0.951057, 0.945519, 0.939693, 0.933580,
		0.927184, 0.920505, 0.913545, 0.906308, 0.898794, 0.891007, 0.882948, 0.874620, 0.866025, 0.857167, 0.848048, 0.838671, 0.829038, 0.819152, 0.809017, 0.798636,
		0.788011, 0.777146, 0.766044, 0.754710, 0.743145, 0.731354, 0.719340, 0.707107, 0.694658, 0.681998, 0.669131, 0.656059, 0.642788, 0.629320, 0.615662, 0.601815,
		0.587785, 0.573576, 0.559193, 0.544639, 0.529919, 0.515038, 0.500000, 0.484810, 0.469472, 0.453991, 0.438371, 0.422618, 0.406737, 0.390731, 0.374607, 0.358368,
		0.342020, 0.325568, 0.309017, 0.292372, 0.275637, 0.258819, 0.241922, 0.224951, 0.207912, 0.190809, 0.173648, 0.156435, 0.139173, 0.121869, 0.104529, 0.087156,
		0.069757, 0.052336, 0.034900, 0.017452, 0.000000
	};
	/*for (i = 0;i<180;i++)
		for (j = 0;j<1000;j++)
			npp[i][j] = 0;*/

	for (i = 0;i < height;i++)
		for (j = 0; j < width;j++)
		{
			if (imageArr[i * width + j] == 255)
			{
				for (k = 0; k < ma; k++)
				{
					p = (int)(i * carCos[k] + j * carSin[k]); //hough变换中距离参数
					p = (int)(p / 2 + mp / 2); //对p值优化防止为负
					npp[k][p] = npp[k][p]++;  //对变换域中对应重复出现的点累加
				}
			}
		}
	kmax = 0; // 最长直线的角度
	pmax = 0;  //最长直线的距离
	n = 0; //这一部分为寻找最长直线
	for (i = 0; i < ma; i++)
		for (j = 0; j < mp; j++) //mp为原图对角线距离
		{ 
			if (npp[i][j] > yuzhi)  //找出最长直线 yuzhi为中间变量用于比较
			{
				yuzhi = npp[i][j];
				kmax = i;
				pmax = j;
			}
		}
//	for (i = 0; i < height;i++)
//		for (j = 0; j < width;j++)
//		{
//			if (imageArr[i*width + j] == 255)
//			{
//				p = (int)(i*carCos[kmax] + j *carSin[kmax]);
//				p = (int)(p / 2 + mp / 2);
//#if defined(DISPLAYDEBUG)  
//				if (p == pmax)
//					putpixel(j, i, RGB(255, 0, 0));
//#endif  
//			}
//		}
	printf("%d \n", pmax);
	return kmax;
}

/**
**pr：进行radon变换后输出矩阵的对于一个特定theta角的列的首地址
**pixel：要进行radon变换的像素值乘以0.25以后的值（由于每一个像素点取了相邻四个点提高精度，故在计算时pixel也要相应乘以0.25，类似于一个点占0.25的比例，然后四个点刚好凑足1的份额）
**r：进行radon变换的该点与初始的r值——rFirst之间的差
**/
void incrementRadon(double *pr, BYTE pixel, double r)
{
	LONG r1;
	double delta;

	r1 = (LONG)r;   //对于每一个点，r值不同，所以，通过这种方式，可以把这一列中相应行的元素的值给赋上
	
	delta = r - r1;
	pr[r1] += pixel * (1.0 - delta); //radon变换本来就是通过记录目标平面上某一点的被映射后点的积累厚度来反推原平面的直线的存在性的，故为+=  
	//printf("%lf ", pr[r1]);
	pr[r1 + 1] += pixel * delta;  //两个点互相配合，提高精度
	//pr[r1]++;
	//pr[r1 + 1]++;
}
/***
**参数解释：
**radonTable：经过radon变换后输出的一维数组，该一维数组是其实要还原成一个rSize*numAngles的矩阵
**imageArr8：需要进行radon变换的矩阵的一维数组存储形势
**thetaPtr：指定进行radon变换的弧度的数组，该角度就是极坐标中偏离正方向的角度
**width：要进行radon变换的矩阵的行数
**height：要进行radon变换的矩阵的列数
**xOrigin：要进行radon变换的矩阵的的中心的横坐标
**yOrigin：要进行radon变换的矩阵的中心的纵坐标
**numAngles：thetaPtr数组中元素的个数
**rFist：极坐标中初始点与变换原点的距离
**rSize：整个radon变换中极坐标的点之间的最远距离
***/
void	radon(double *radonTable, BYTE *imageArr8, double *thetaPtr, LONG width, LONG height,
	LONG xOrigin, LONG yOrigin, int numAngles, LONG rFirst, LONG rSize)
{
	LONG k, m, n;              /* loop counters */
	double angle;             /* radian angle value */
	double cosine, sine;      /* cosine and sine of current angle */
	double *pr;               /* points inside output array */
	BYTE *pixelPtr;         /* points inside input array */
	double pixel;             /* current pixel value */
	double *ySinTable, *xCosTable;
	/* tables for x*cos(angle) and y*sin(angle) */
	LONG x, y;
	double r, delta;
	int r1;

	/* Allocate space for the lookup tables */
	xCosTable = (double *)calloc(2 * width, sizeof(double));  //MATLAB的内存申请函数，对应C语言可以换成calloc函数 
	ySinTable = (double *)calloc(2 * height, sizeof(double));
	printf("%d %d %d %d\n", xOrigin, yOrigin, rFirst, rSize);
	for (k = 0; k < numAngles; k++)
	{
		//每一个theta角，经过radon变化后，就会产生一列数据，这一列数据中，共有rSize个数据
		angle = thetaPtr[k];
		//printf("angle-> %lf \n", angle);
		pr = radonTable + k*rSize;  /* pointer to the top of the output column */
		cosine = cos(angle);
		sine = sin(angle);

		/* Radon impulse response locus:  R = X*cos(angle) + Y*sin(angle) */
		/* Fill the X*cos table and the Y*sin table.                      */
		/* x- and y-coordinates are offset from pixel locations by 0.25 */
		/* spaced by intervals of 0.5. */
		/*
		**radon 变换中，极坐标下，沿r轴的theta角和每一个像素点的分布都是非线性的，而此处采用的是线性radon变换，
		**为了提高精度，把每一个像素点分成其四周四个相邻的像素点来进行计算！x、y坐标的误差是正负0.25
		*/
		for (n = 0; n < width; n++)
		{
			x = n - xOrigin;
			xCosTable[2 * n] = (x - 0.25)*cosine;   //由极坐标的知识知道，相对于变换的原点，这个就是得到了该点的横坐标
			xCosTable[2 * n + 1] = (x + 0.25)*cosine;
			
		}
		/*for (n = 0; n < 2 * height; n++)
		{
			printf("%lf =%d ", xCosTable[n], n);
		}*/
		for (m = 0; m < height; m++)
		{
			y = yOrigin - m;
			ySinTable[2 * m] = (y - 0.25)*sine;   //同理，相对于变换的原点，得到了纵坐标
			ySinTable[2 * m + 1] = (y + 0.25)*sine;
		}
		
		pixelPtr = imageArr8;
		for (n = 0; n < width; n++)
		{
			for (m = 0; m < height; m++)   //便利原矩阵中的每一个像素点
			{
				pixel = *pixelPtr++;
				if (pixel != 0.0)   //如果该点像素值不为0，也即图像不连续
				{
					pixel *= 0.25;
					//printf("%lf ", pixel);
					//一个像素点分解成四个临近的像素点进行计算，提高精确度
					r = xCosTable[2 * n] + ySinTable[2 * m] - rFirst;
					incrementRadon(pr, pixel, r);

					r = xCosTable[2 * n + 1] + ySinTable[2 * m] - rFirst;
					incrementRadon(pr, pixel, r);

					r = xCosTable[2 * n] + ySinTable[2 * m + 1] - rFirst;
					incrementRadon(pr, pixel, r);

					r = xCosTable[2 * n + 1] + ySinTable[2 * m + 1] - rFirst;
					incrementRadon(pr, pixel, r);
				}
			}
		}
	}

	
	free(xCosTable); 
	free(ySinTable);
}



/***
**参数解释：
**radonTable：经过radon变换后输出的一维数组，该一维数组是其实要还原成一个rSize*numAngles的矩阵
**imageArr8：需要进行radon变换的矩阵的一维数组存储形势
**thetaPtr：指定进行radon变换的弧度的数组，该角度就是极坐标中偏离正方向的角度
**width：要进行radon变换的矩阵的行数
**height：要进行radon变换的矩阵的列数
**xOrigin：要进行radon变换的矩阵的的中心的横坐标
**yOrigin：要进行radon变换的矩阵的中心的纵坐标
**numAngles：thetaPtr数组中元素的个数
**rFist：极坐标中初始点与变换原点的距离
**rSize：整个radon变换中极坐标的点之间的最远距离
***/
void	myRadon(double *radonTable, BYTE *imageArr8, double *thetaPtr, LONG width, LONG height,
	LONG xOrigin, LONG yOrigin, int numAngles, LONG rFirst, LONG rSize)
{
	LONG k, m, n;              /* loop counters */
	double angle;             /* radian angle value */
	double cosine, sine;      /* cosine and sine of current angle */
	double *pr;               /* points inside output array */
	BYTE *pixelPtr;         /* points inside input array */
	double pixel;             /* current pixel value */
	double *ySinTable, *xCosTable;
	/* tables for x*cos(angle) and y*sin(angle) */
	LONG x, y;
	double r, delta;
	LONG r1;

	/* Allocate space for the lookup tables */
	xCosTable = (double *)calloc(width, sizeof(double));  //MATLAB的内存申请函数，对应C语言可以换成calloc函数 
	ySinTable = (double *)calloc(height, sizeof(double));
	printf("%d %d %d %d\n", xOrigin, yOrigin, rFirst, rSize);
	for (k = 0; k < numAngles; k++)
	{
		//每一个theta角，经过radon变化后，就会产生一列数据，这一列数据中，共有rSize个数据
		angle = thetaPtr[k];
		pr = radonTable + k*rSize;  /* pointer to the top of the output column */
		cosine = cos(angle);
		sine = sin(angle);

		/* Radon impulse response locus:  R = X*cos(angle) + Y*sin(angle) */
		/* Fill the X*cos table and the Y*sin table.                      */
		/* x- and y-coordinates are offset from pixel locations by 0.25 */
		/* spaced by intervals of 0.5. */
		/*
		**radon 变换中，极坐标下，沿r轴的theta角和每一个像素点的分布都是非线性的，而此处采用的是线性radon变换，
		**为了提高精度，把每一个像素点分成其四周四个相邻的像素点来进行计算！x、y坐标的误差是正负0.25
		*/
		for (n = 0; n < width; n++)
		{
			x = n - xOrigin;
			xCosTable[n] = x*cosine;   //由极坐标的知识知道，相对于变换的原点，这个就是得到了该点的横坐标
			//xCosTable[2 * n + 1] = (x + 0.25)*cosine;

		}
		/*for (n = 0; n < 2 * height; n++)
		{
		printf("%lf =%d ", xCosTable[n], n);
		}*/
		for (m = 0; m < height; m++)
		{
			y = yOrigin - m;
			ySinTable[ m] = y*sine;   //同理，相对于变换的原点，得到了纵坐标
			//ySinTable[2 * m + 1] = (y + 0.25)*sine;
		}

		pixelPtr = imageArr8;
		for (n = 0; n < width; n++)
		{
			for (m = 0; m < height; m++)   //便利原矩阵中的每一个像素点
			{
				pixel = *pixelPtr++;
				if (pixel != 0.0)   //如果该点像素值不为0，也即图像不连续
				{
					//pixel *= 0.25;
					//printf("%lf ", pixel);
					//一个像素点分解成四个临近的像素点进行计算，提高精确度
					r = ySinTable[n] + ySinTable[m] - rFirst;
					r1 = (LONG)r;   //对于每一个点，r值不同，所以，通过这种方式，可以把这一列中相应行的元素的值给赋上

					//delta = r - r1;
					pr[r1] = pr[r1]+1; //radon变换本来就是通过记录目标平面上某一点的被映射后点的积累厚度来反推原平面的直线的存在性的，故为+=  
													 //printf("%lf ", pr[r1]);
					//pr[r1 + 1] += pixel * delta;  //两个点互相配合，提高精度

					/*r = xCosTable[2 * n + 1] + ySinTable[2 * m] - rFirst;
					incrementRadon(pr, pixel, r);

					r = xCosTable[2 * n] + ySinTable[2 * m + 1] - rFirst;
					incrementRadon(pr, pixel, r);

					r = xCosTable[2 * n + 1] + ySinTable[2 * m + 1] - rFirst;
					incrementRadon(pr, pixel, r);*/
				}
			}
		}
	}


	free(xCosTable);
	free(ySinTable);
}


int		KTTransform(BYTE *imageArr8, LONG width, LONG height) {
	LONG	xSum = 0, ySum = 0, xSqSum = 0, ySqSum = 0, xySum = 0, cnt = 0;
	LONG	i, j;
	double	xMean, yMean, u20, u02, u11;
	double		theta = 0;

	for (i = 0; i < height; i++)
	{
		for (j = 0; j < width; j++)
		{
			if (imageArr8[i*width+j] == 255)
			{
				xSum += j;
				xSqSum += j*j;
				ySum += i;
				ySqSum += i*i;
				xySum += i*j;
				cnt++;
			}
		}
	}

	
	xMean = (double)xSum / cnt;
	yMean = (double)ySum / cnt;
	u20 = (double)xSqSum / cnt - xMean*xMean;
	u02 = (double)ySqSum / cnt - yMean*yMean;
	u11 = (double)xySum / cnt - xMean*yMean;
	//printf("%lf %lf %lf %lf %lf %lf\n", xMean, yMean, u20, u02, u11, 2 * u11 / (u20 - u02));
	//theta = atan(2 * u11 / (u20 - u02)) / 2;
	theta = atan(2 * u11 / (u20 - u02)) * 180.0 / PI;

	//printf("%lf \n", theta);
	return theta;
}

BYTE*	rotate(const BYTE* imageArr8, double fAngle, LONG oldWidth, LONG oldHeight, LONG* newWidth, LONG* newHeight) {
	BYTE*	pNewImg;
	LONG	i, j;
	double	fSin, fCos;		 // 旋转角度的正弦和余弦
							 //原图四个角的坐标
	double fSrcX1, fSrcY1, fSrcX2, fSrcY2, fSrcX3, fSrcY3, fSrcX4, fSrcY4;

	//旋转后四个角的坐标
	double fDstX1, fDstY1, fDstX2, fDstY2, fDstX3, fDstY3, fDstX4, fDstY4;

	//两个中间常量
	double f1, f2, i0, j0;

	fAngle = fAngle*PI /180.0 ;
	fSin = (double)sin(fAngle);
	fCos = (double)cos(fAngle);

	//计算原图的四个角的坐标
	fSrcX1 = (double)(-(oldWidth - 1) / 2);
	fSrcY1 = (double)((oldHeight - 1) / 2);
	fSrcX2 = (double)((oldWidth - 1) / 2);
	fSrcY2 = (double)((oldHeight - 1) / 2);
	fSrcX3 = (double)(-(oldWidth - 1) / 2);
	fSrcY3 = (double)(-(oldHeight - 1) / 2);
	fSrcX4 = (double)((oldWidth - 1) / 2);
	fSrcY4 = (double)(-(oldHeight - 1) / 2);
	
	//计算新图四个角的坐标
	fDstX1 = fCos*fSrcX1 + fSin*fSrcY1;
	fDstY1 = -fSin*fSrcX1 + fCos*fSrcY1;
	fDstX2 = fCos*fSrcX2 + fSin*fSrcY2;
	fDstY2 = -fSin*fSrcX2 + fCos*fSrcY2;
	fDstX3 = fCos*fSrcX3 + fSin*fSrcY3;
	fDstY3 = -fSin*fSrcX3 + fCos*fSrcY3;
	fDstX4 = fCos*fSrcX4 + fSin*fSrcY4;
	fDstY4 = -fSin*fSrcX4 + fCos*fSrcY4;
	//printf("%lf %lf %lf %lf \n", fDstX1, fDstY1, )
	//计算旋转后的图象实际宽度
	*newWidth = (LONG)(max(fabs(fDstX4 - fDstX1), fabs(fDstX3 - fDstX2)) + 0.5);
	//计算旋转后图象的高度
	*newHeight = (LONG)(max(fabs(fDstY4 - fDstY1), fabs(fDstY3 - fDstY2)) + 0.5);
	
	//printf("%d %d \n", *newWidth, *newHeight);
	//两个常数，这样不用以后每次都计算
	f1 = (double)(-0.5*(*newWidth - 1)*fCos - 0.5*(*newHeight - 1)*fSin + 0.5*(oldWidth - 1));
	f2 = (double)(0.5*(*newWidth - 1)*fSin - 0.5*(*newHeight - 1)*fCos + 0.5*(oldHeight - 1));
	pNewImg = (BYTE*)calloc((*newWidth)*(*newHeight), sizeof(BYTE));


	for ( i = 0; i < *newHeight; i++)
	{
		for ( j = 0; j < *newWidth; j++)
		{
			//计算该像数在原图中的坐标
			i0 = -((double)j)*fSin + ((double)i)*fCos + f2;
			j0 = ((double)j)*fCos + ((double)i)*fSin + f1;
			pNewImg[i*(*newWidth) +j] = interpolation(imageArr8, oldWidth, oldHeight, j0, i0);
			// pNewImg[i*(*newWidth) + j] = 255;
		}
	}

	return pNewImg;
}

BYTE	interpolation(const BYTE* imageArr8, LONG width, LONG height, double x, double y) {
	// 四个最临近象素的坐标(i1, j1), (i2, j1), (i1, j2), (i2, j2)  
	LONG    i1, i2;
	LONG    j1, j2;
	BYTE    f1, f2, f3, f4;    // 四个最临近象素值      
	BYTE    f12, f34;        // 二个插值中间值      
	BYTE	res;
	// 定义一个值，当象素坐标相差小于改值时认为坐标相同  
	double    EXP = 0.0001;
	//BYTE f12, f34;
	// 计算四个最临近象素的坐标  
	i1 = (LONG)x;
	i2 = i1 + 1;
	j1 = (LONG)y;
	j2 = j1 + 1;

	// 根据不同情况分别处理  
	if ((x < 0) || (x > width - 1) || (y < 0) || (y > height - 1))
	{
		return 0;        // 要计算的点不在源图范围内，直接返回255。  
	}
	else
	{
		if (fabs(x - width + 1) <= EXP)
		{
			// 要计算的点在图像右边缘上  
			if (fabs(y - height + 1) <= EXP)
			{
				// 要计算的点正好是图像最右下角那一个象素，直接返回该点象素值  
				//f1 = *((unsigned char *)lpDIBBits + lLineBytes *
				//	(lHeight - 1 - j1) + i1);
				f1 = imageArr8[j1*width + i1];
				//return f1;
				res = f1;
			}
			else
			{
				// 在图像右边缘上且不是最后一点，直接一次插值即可  
				//f1 = *((unsigned char *)lpDIBBits + lLineBytes *
				//	(lHeight - 1 - j1) + i1);
				//f3 = *((unsigned char *)lpDIBBits + lLineBytes *
				//	(lHeight - 1 - j1) + i2);
				f1 = imageArr8[j1*width + i1];
				f3 = imageArr8[j1*width + i2];

				// 返回插值结果  
				res = ((BYTE)(f1 + (y - j1) * (f3 - f1)));
				//return 
			}
		}
		else if (fabs(y - height + 1) <= EXP)
		{
			// 要计算的点在图像下边缘上且不是最后一点，直接一次插值即可  
			//f1 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j1) + i1);
			//f2 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j2) + i1);
			f1 = imageArr8[j1*width + i1];
			f2 = imageArr8[j2*width + i1];
			// 返回插值结果  
			res = ((BYTE)(f1 + (x - i1) * (f2 - f1)));
			//return ((BYTE)(f1 + (x - i1) * (f2 - f1)));
		}
		else
		{
			// 计算四个最临近象素值  
			/*f1 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j1) + i1);
			f2 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j2) + i1);
			f3 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j1) + i2);
			f4 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j2) + i2);*/

			f1 = imageArr8[j1*width + i1];
			f2 = imageArr8[j2*width + i1];
			f3 = imageArr8[j1*width + i2];
			f4 = imageArr8[j2*width + i2];

			// 插值1  
			f12 = (BYTE)(f1 + (x - i1) * (f2 - f1));
			// 插值2  
			f34 = (BYTE)(f3 + (x - i1) * (f4 - f3));
			// 插值3  
			res = ((BYTE)(f12 + (y - j1) * (f34 - f12)));
			//return ((BYTE)(f12 + (y - j1) * (f34 - f12)));
		}
	}
	return (res > 120) ? 255 : 0;
}

