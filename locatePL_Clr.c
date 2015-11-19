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

int		judgePLClr(const PHSV myHSV, LONG width, LONG height);
int		judgePixClr(double H, double S, double V);
BYTE*	binarization_PLClr(BYTE* imageArr24, LONG width, LONG height, PHSV myHSV, int PLClr);
int		judgePLCharClr(int PLClr);
int		getProBmp(BYTE* PLProBmpArr8, BYTE* charProBmpArr8, LONG width, LONG height, PHSV myHSV, int PLClr);
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
int		secLocatePL(BYTE* PLProBmpArr8, LONG width, LONG height, PBOUNDS bound, int PLClr);
BYTE*	extractBmpByBounds(const BYTE* bmpArr, LONG widht, LONG height, PBOUNDS bound, WORD wType);

int		getHorContiPix(BYTE* imageArr8, LONG width, LONG heigh, PBOUNDS bound);
// 用一个3*12的矩形逐步探测蓝色前景图，如果白点个数超过阈值，则全部置为白点，否则置为黑点
int		removeNoise_rect(BYTE* imageArr8, LONG width, LONG heigh);
//********************************
// 全局变量
BITMAP_IMAGE bmpImage;
BYTE	*PLProBmpArr8;
BYTE	*charProBmpArr8;
HSV*	myHSV = NULL;

char*		dest;
int		locatePL_clr(char* srcFile, char* destFile) {
	
	FILE	*fpSrc = NULL,
			*fpDest = NULL;
	LONG	i, j, k, temp = 0;
	BYTE	*bmpArr8, *bmpArr24, *firstLocateBmpArr, *secLocateBmpArr, *tempArr, *rotatePreBmpArr;
	int		res, status = 0;
	
	LONG	origWidth, origHeight, locateWidth, locateHeight;
	//BITMAP_IMAGE bmpImage;
	LONG	rotateWidth, rotateHeight;
	const WORD	wTpye = BF_TYPE_24;		//指定提取的对象是8位灰度还是24位真色子图		
	BOUNDS	bound;			// 四个边界，分别代表上下左右
	
	int		PLClr = 0;

	dest = destFile;

	fpSrc = loadImage(srcFile, &bmpImage);
	//printBmpHeader(&bmpImage);
	bmpArr24 = creatImageArr(fpSrc, &bmpImage);
	origWidth = bmpImage.infoHeader.biWidth;
	origHeight = bmpImage.infoHeader.biHeight;
	for (PLClr = 0; PLClr < 2                                                                          ; PLClr++)
	{
		res = firstLocatePL(bmpArr24, origWidth, origHeight, &bound, PLClr);
		if (res == 0)
		{	// 第一次定位成功
			break;
			status = 1;
		}
		if (res != 0)
		{	// 第一次定位失败
			bound.up = -1;		// 初始边界，避免第一次定位失败对第二次定位的影响
			bound.down = -1;
			bound.left = -1;
			bound.right = -1;
			res = secLocatePL(PLProBmpArr8, origWidth, origHeight, &bound, PLClr);
			//creatBmpByArr(destFile, &bmpImage, PLProBmpArr8, 8);
			if (res == 0)
			{ // 第二次定位成功

			  /*for (i = 0; i < origHeight; i++)
			  {
			  PLProBmpArr8[i*origWidth + bound.left] = 255;
			  PLProBmpArr8[i*origWidth + bound.right] = 255;
			  }
			  for (i = 0; i < origWidth; i++)
			  {
			  PLProBmpArr8[bound.up*origWidth + i] = 255;
			  PLProBmpArr8[bound.down*origWidth + i] = 255;
			  }*/
			  //creatBmpByArr(destFile, &bmpImage, PLProBmpArr8, 8);
				
				status = 2;
				break;
			}
		}
	}
	printf("车牌颜色为: %d \n", PLClr);
	//printf("%d \n", PLProBmpArr8[0]);
	//bmpArr8 = binarization_PLClr(bmpArr24, origWidht, origHeight, myHSV, 0);
	//medianFilter(bmpArr8, origWidht, origHeight);
	
	//getHorBound_Clr(bmpArr8,origWidht, origHeight, bound);
	//getVerBound(bmpArr8, origWidht, origHeight, bound);
	//drawBound(bmpArr8, origWidht, origHeight, bound);
	
//	printf("%d \n", PLClr);
	if (res == 0)
	{  // 定位成功
		tempArr = extractBmpByBounds(bmpArr24, origWidth, origHeight, &bound, 24);
		bmpImage.infoHeader.biWidth = bound.right - bound.left + 1;
		bmpImage.infoHeader.biHeight = bound.up - bound.down + 1;
		creatBmpByArr(destFile, &bmpImage, tempArr, 24);
		free(tempArr);
	}
	
	free(PLProBmpArr8);
	free(charProBmpArr8);
	free(myHSV);
	return status;
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
				imageArr8[i*width + bound->left] = 255;
				imageArr8[i*width + bound->right] = 255;
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
				imageArr8[bound->up*width + i] = 255;
				imageArr8[bound->down*width + i] = 255;
			}*/
		}
	}
	//creatBmpByArr(dest, &bmpImage, PLProBmpArr8, 8);
	free(imageArr8);
	return res;
}

int		secLocatePL(BYTE* PLProBmpArr8w, LONG width, LONG height, PBOUNDS bound, int PLClr) {
	int		res = 1;
	LONG	i, j;
	const	double  PLBoxAddRation = 0.35;		// 字符顶部到车牌套的距离
	int		PLBoxAdd = 0;		// 字符顶部到车牌套的距离
	removeNoise_rect(PLProBmpArr8, width, height);

	if (getHorContiPix(PLProBmpArr8, width, height, bound) == 1) {
		
		res = 2;
		if (getVerBound_CLS(PLProBmpArr8, width, height, bound, 4.0) == 1) {
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

int		getProBmp(BYTE* PLProBmpArr8, BYTE* charProBmpArr8, LONG width, LONG height, PHSV myHSV, int PLClr) {
	LONG	totalPixls = width*height;
	int		i;
	int		curClr;
	if (PLClr != WHITE_PL)
	{
		for (i = 0; i<totalPixls; ++i)
		{
			curClr = judgePixClr(myHSV[i].H, myHSV[i].S, myHSV[i].V);
			if (curClr == PLClr) {
				PLProBmpArr8[i] = 255;
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
				PLProBmpArr8[i] = 255;
			}
			else if (curClr == judgePLCharClr(PLClr))
			{
				charProBmpArr8[i] = 255;
			}
		}
	}
	
	//medianFilter(PLProBmpArr8, width, height);
	//medianFilter(PLProBmpArr8, width, height);
	return 0;
}

BYTE*	getPLEdgeBmpArr(const BYTE* imageArr24, LONG width, LONG height, int PLClr) {
	LONG	totalPixls = width*height;
	int		i , j;
	
	
	BYTE*	imageArr8;
	// RGB颜色空间转成HSV空间
	myHSV = RGB2HSV(imageArr24, width, height);
	PLProBmpArr8 = (BYTE*)calloc(totalPixls, sizeof(BYTE));
	charProBmpArr8 = (BYTE*)calloc(totalPixls, sizeof(BYTE));

	// 车牌定位第一步：获取蓝色和白色前景图
	getProBmp(PLProBmpArr8, charProBmpArr8, width, height, myHSV, PLClr);
	
	// 车牌定位第二步：将RGB彩色图像灰度化，进行边缘检测，并二值化，得到二值化边缘图
	imageArr8 = rgbToGray(imageArr24, width, height);
	sobleSideEnhance(imageArr8, width, height);
	//histogramEqual(tempBmpArr, width, height);
	binarization(imageArr8, width, height);
	dilation(imageArr8, width, height);
	
	// 车牌定位第三步：确定图像蓝白色边缘点
	intersection(imageArr8, PLProBmpArr8, width, height); // 蓝色边缘点
	intersection(imageArr8, charProBmpArr8, width, height); // 白色边缘点
	// 判别蓝色边缘点左右是否存在有白色边缘点
	// 如有则为图像蓝白色边缘点，设为前景白色；如无则设
	// 为背景黑色；得到图像蓝白色边缘点图
	for (i = 0; i < totalPixls; i++)
	{
		if (PLProBmpArr8[i] == 255)
		{
			imageArr8[i] = (charProBmpArr8[i - 1] == 255 || charProBmpArr8[i + 1] == 255) ? 255 : 0;
		}
		else
		{
			imageArr8[i] = 0;
		}
	}
	removeHorAloneNoise(imageArr8, width, height);
	//dilation(imageArr8, width, height);
	
	/*free(PLProBmpArr8);
	free(charProBmpArr8);
	free(myHSV);*/

	//printf("%d ", PLProBmpArr8[0]);
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
					//flag = (imageArr8[(i - 1)*width + j + k] == 255);
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
						imageArr8[m*width + n] = 255;
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
						imageArr8[i*width + t] = 255;
						imageArr8[i*width + t] = 255;
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
			imageArr8[bound->up*width + j] = 255;
			imageArr8[bound->down*width + j] = 255;
		}*/
		res =  1;
	}
	
	free(pos);
	free(line);

	return res;
}