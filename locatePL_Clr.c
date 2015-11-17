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
LONG*	getHorBound_Clr(const BYTE* imageArr, LONG width, LONG height, LONG bound[]);
int		judgePLClr(const PHSV myHSV, LONG width, LONG height);
int		judgePixClr(double H, double S, double V);
BYTE*	binarization_PLClr(BYTE* imageArr24, LONG width, LONG height, PHSV myHSV, int PLClr);
int		judgePLCharClr(int PLClr);
int		getProBmp(BYTE* PLProBmpArr8, BYTE* charProBmpArr8, LONG width, LONG height, PHSV myHSV, int PLClr);
BYTE*	getPLEdgeBmpArr(const BYTE* imageArr24, LONG width, LONG height, int PLClr);
// 如果以当前白色点为中心的九宫格中第一行和最后一行都是黑色点，则将该点置为黑点
int		removeHorAloneNoise(BYTE* imageArr8, LONG width, LONG height);
LONG*	getHorBound_CLS(BYTE* imageArr8, LONG width, LONG height, LONG bound[]);
int		locatePL_clr(char* srcFile, char* destFile) {
	FILE	*fpSrc = NULL,
		*fpDest = NULL;
	LONG	i, j, k, temp = 0;
	BYTE	*bmpArr8, *bmpArr24, *firstLocateBmpArr, *secLocateBmpArr, *tempArr, *rotatePreBmpArr;
	
	
	LONG	origWidth, origHeight, locateWidth, locateHeight;
	BITMAP_IMAGE bmpImage;
	LONG	rotateWidth, rotateHeight;
	const WORD	wTpye = BF_TYPE_24;		//指定提取的对象是8位灰度还是24位真色子图		
	LONG	bound[4] = { 0 };			// 四个边界，分别代表上下左右
	
	int		PLClr = 0;

	fpSrc = loadImage(srcFile, &bmpImage);
	bmpArr24 = creatImageArr(fpSrc, &bmpImage);
	origWidth = bmpImage.infoHeader.biWidth;
	origHeight = bmpImage.infoHeader.biHeight;

	
	//PLClr = judgePLClr(myHSV, origWidht, origHeight);
	//if (PLClr == -1)
	//{
	//	printf("判断车牌的底色失败 \n");
	//	//free(myHSV);
	//	//return 1;
	//}
	//else
	//{
	//	printf("当前车的底色是: %d\n",PLClr);
	//}

	tempArr = getPLEdgeBmpArr(bmpArr24, origWidth, origHeight, PLClr);
	getHorBound_CLS(tempArr, origWidth, origHeight, bound);
	//printf("%d \n", PLProBmpArr8[0]);
	//bmpArr8 = binarization_PLClr(bmpArr24, origWidht, origHeight, myHSV, 0);
	//medianFilter(bmpArr8, origWidht, origHeight);

	//getHorBound_Clr(bmpArr8,origWidht, origHeight, bound);
	//getVerBound(bmpArr8, origWidht, origHeight, bound);
	//drawBound(bmpArr8, origWidht, origHeight, bound);
	creatBmpByArr(destFile, &bmpImage, tempArr, 8);
//	printf("%d \n", PLClr);

	free(tempArr);

	return 0;
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
LONG*	getHorBound_Clr(const BYTE* imageArr, LONG width, LONG height, LONG bound[]) {
	int		i, j, k, m, n, q = 0, tempCnt = 0;
	//const BYTE	JUMP_SUM_MIN = 12, JUMP_SUM_MAX = 30;	//黑白跳变的个数
	const BYTE	JUMP_SUM_MIN = 10, JUMP_SUM_MAX = 30;	//黑白跳变的个数
	//const BYTE	JUMP_COUNTER_MIN = 6, JUMP_COUNTER_MAX = 18;	//满足规定距离差的个数
	const BYTE	JUMP_COUNTER_MIN = 2, JUMP_COUNTER_MAX = 18;	//满足规定距离差的个数
	//const BYTE	JUMP_DIFF_MIN = 4, JUMP_DIFF_MAX = 25;	//相邻两次黑白跳的距离差
	const BYTE	JUMP_DIFF_MIN = 0, JUMP_DIFF_MAX = 25;	//相邻两次黑白跳的距离差
	const LONG	SPEC_LINE_MIN = 15, SPEC_LINE_MAX = 90;	//满足所有要求的短间隔连续行的上下界
	LONG*	iJumpSum = (LONG*)calloc(height, sizeof(LONG));	// 记录第j行的黑白跳变次数
	LONG*	iJumpLocation = (LONG*)calloc(width / 2 + 1, sizeof(LONG));	//记录当前行发生黑白跳变的位置
	LONG*	iJumpDiff = (LONG*)calloc(width / 2 + 1, sizeof(LONG));		//记录两次黑白跳变的间隔
	LONG*	tempPos = (LONG*)calloc(height, sizeof(LONG));
	//LONG*	iJumpCouter = (LONG*)calloc(width/2+1, sizeof(LONG));		//记录满足规定距离差的个数
	LONG	iJumpCouter = 0;
	LONG	iDown = -1, iUp = -1;
	//LONG*	iJumpLocation = (LONG*)calloc(width)*height, sizeof(LONG));
	int flag = 1;
	for (j = 0; j<height; ++j) {
		for (i = 1, k = 0; i<width; ++i) {
			if (imageArr[j*width + i] - imageArr[j*width + i - 1] == -255) {
				++iJumpSum[j];
				iJumpLocation[k++] = i - 1;
			}
		}
		if (iJumpSum[j] >= JUMP_SUM_MIN && iJumpSum[j] <= JUMP_SUM_MAX) {
			iJumpCouter = 0;
			++tempCnt;
			for (m = 0; m<k - 1; ++m) {//计算相邻两次黑白跳变的距离差
				iJumpDiff[m] = iJumpLocation[m + 1] - iJumpLocation[m];
			}

			for (n = 0; n<k - 1; ++n) {
				if (iJumpDiff[n]>JUMP_DIFF_MIN && iJumpDiff[n]<JUMP_DIFF_MAX) {
					++iJumpCouter;
				}
				else {
					if (iJumpCouter >= JUMP_COUNTER_MIN && iJumpCouter <= JUMP_COUNTER_MAX) {
						//if (checkRunLenRatio(imageArr, height, width, j) == 1) {
							tempPos[q++] = j;
							
						//}
						break;
					}
					else {
						iJumpCouter = 0;
					}
				}
			}
		}
	}
	tempCnt = 0;
	iDown = tempPos[0];
	for (i = 0; i<q - 1; i++) {
		if (tempPos[i + 1] - tempPos[i] <= 3) {
			++tempCnt;
		}
		else {
			if (tempCnt >= SPEC_LINE_MIN && tempCnt <= SPEC_LINE_MAX) {
				iUp = tempPos[i];
				break;
			}
			else {
				iDown = tempPos[i + 1];
				tempCnt = 0;
			}
		}

	}
	if (i <= q && tempCnt >= SPEC_LINE_MIN && tempCnt <= SPEC_LINE_MAX) {
		iUp = tempPos[i];
	}
	bound[0] = iUp;
	bound[1] = iDown;
	if (iUp <= iDown) {
		printf("水平定位失败");
	}
	else {
		printSuccess("getHorPos");
	}
	free(iJumpSum);
	free(iJumpLocation);
	free(iJumpDiff);
	free(tempPos);

	return NULL;
}

int		getProBmp(BYTE* PLProBmpArr8, BYTE* charProBmpArr8, LONG width, LONG height, PHSV myHSV, int PLClr) {
	LONG	totalPixls = width*height;
	int		i;
	
	for (i = 0; i<totalPixls; ++i)
	{
		if (judgePixClr(myHSV[i].H, myHSV[i].S, myHSV[i].V) == PLClr) {
			PLProBmpArr8[i] = 255;
		}
		else if (judgePixClr(myHSV[i].H, myHSV[i].S, myHSV[i].V) == judgePLCharClr(PLClr))
		{
			charProBmpArr8[i] = 255;
		}
	}
	//medianFilter(PLProBmpArr8, width, height);
	//medianFilter(PLProBmpArr8, width, height);
	return 0;
}

BYTE*	getPLEdgeBmpArr(const BYTE* imageArr24, LONG width, LONG height, int PLClr) {
	LONG	totalPixls = width*height;
	int		i , j;
	BYTE	*PLProBmpArr8 = (BYTE*)calloc(totalPixls, sizeof(BYTE));
	BYTE	*charProBmpArr8 = (BYTE*)calloc(totalPixls, sizeof(BYTE));
	HSV*	myHSV = NULL;
	BYTE*	imageArr8;
	// RGB颜色空间转成HSV空间
	myHSV = RGB2HSV(imageArr24, width, height);
	
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
	
	//free(PLProBmpArr8);
	//free(charProBmpArr8);
	free(myHSV);

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

LONG*	getHorBound_CLS(BYTE* imageArr8, LONG width, LONG height, LONG bound[]) {
	int		i, j, k, m, n, q = 0, tempCnt = 0;
	const BYTE	JUMP_SUM_MIN = 10, JUMP_SUM_MAX = 3000;			//黑白跳变的个数
	const BYTE	JUMP_COUNTER_MIN = 2, JUMP_COUNTER_MAX = 18;	//满足规定距离差的个数
	const BYTE	JUMP_DIFF_MIN = 2, JUMP_DIFF_MAX = 25;	//相邻两次黑白跳的距离差
	const LONG	SPEC_LINE_MIN = 15, SPEC_LINE_MAX = 90;	//满足所有要求的短间隔连续行的上下界
	LONG*	iJumpSum = (LONG*)calloc(height, sizeof(LONG));	// 记录第j行的黑白跳变次数
	LONG*	iJumpLocation = (LONG*)calloc(width / 2 + 1, sizeof(LONG));	//记录当前行发生黑白跳变的位置
	LONG*	iJumpDiff = (LONG*)calloc(width / 2 + 1, sizeof(LONG));		//记录两次黑白跳变的间隔
	LONG*	tempPos = (LONG*)calloc(height, sizeof(LONG));
	//LONG*	iJumpCouter = (LONG*)calloc(width/2+1, sizeof(LONG));		//记录满足规定距离差的个数
	LONG	iJumpCouter = 0;
	LONG	iDown = -1, iUp = -1;
	//LONG*	iJumpLocation = (LONG*)calloc(width)*height, sizeof(LONG));
	int flag = 1;
	//for (j = 0; j<height; ++j) {
	//	for (i = 1, k = 0; i<width; ++i) {
	//		if (imageArr8[j*width + i] - imageArr8[j*width + i - 1] == 255) {
	//			++iJumpSum[j];
	//			iJumpLocation[k++] = i - 1;
	//		}
	//	}
	//	if (iJumpSum[j] >= JUMP_SUM_MIN) {
	//		iJumpCouter = 0;
	//		++tempCnt;
	//		for (m = 0; m<k - 1; m += 2) {//计算相邻两次黑白跳变的距离差
	//			iJumpDiff[m/2] = iJumpLocation[m + 1] - iJumpLocation[m];
	//		}

	//		for (n = 0; n<k/2; ++n) {
	//			if (iJumpDiff[n]>JUMP_DIFF_MIN && iJumpDiff[n]<JUMP_DIFF_MAX) {
	//				++iJumpCouter;
	//			}
	//			else {
	//				if (iJumpCouter >= JUMP_COUNTER_MIN && iJumpCouter <= JUMP_COUNTER_MAX) {
	//					//if (checkRunLenRatio(imageArr, height, width, j) == 1) {
	//						tempPos[q++] = j;
	//						for ( n = 0; n < width; n++)
	//						{
	//							imageArr8[j*width + n] = 255;
	//						}
	//						
	//					//}
	//					break;
	//				}
	//				else {
	//					iJumpCouter = 0;
	//				}
	//			}
	//		}
	//	}
	//}
	for (j = 0; j<height; ++j) {
		for (i = 1, k = 0; i<width; ++i) {
			if (imageArr8[j*width + i] - imageArr8[j*width + i - 1] == 255) {
				++iJumpSum[j];
				iJumpLocation[k++] = i - 1;
			}
		}
		if (iJumpSum[j] >= JUMP_SUM_MIN) {
			iJumpCouter = 0;
			++tempCnt;
			for (n = 0; n < width; n++)
			{
				imageArr8[j*width + n] = 255;
			}
			//for (m = 0; m<k - 1; m++) {//计算相邻两次黑白跳变的距离差
			//	iJumpDiff[m] = iJumpLocation[m + 1] - iJumpLocation[m];
			//}

			//for (n = 0; n<k-1; ++n) {
			//	if (iJumpDiff[n]>JUMP_DIFF_MIN && iJumpDiff[n]<JUMP_DIFF_MAX) {
			//		++iJumpCouter;
			//	}
			//	
				//else {
				//	if (iJumpCouter >= JUMP_COUNTER_MIN && iJumpCouter <= JUMP_COUNTER_MAX) {
				//		//if (checkRunLenRatio(imageArr, height, width, j) == 1) {
				//		tempPos[q++] = j;
				//		for (n = 0; n < width; n++)
				//		{
				//			imageArr8[j*width + n] = 255;
				//		}

				//		//}
				//		break;
				//	}
				//	else {
				//		iJumpCouter = 0;
				//	}
				//}
		}
		
	}
	/*tempCnt = 0;
	iDown = tempPos[0];
	for (i = 0; i<q - 1; i++) {
		if (tempPos[i + 1] - tempPos[i] <= 3) {
			++tempCnt;
		}
		else {
			if (tempCnt >= SPEC_LINE_MIN && tempCnt <= SPEC_LINE_MAX) {
				iUp = tempPos[i];
				break;
			}
			else {
				iDown = tempPos[i + 1];
				tempCnt = 0;
			}
		}

	}
	if (i <= q && tempCnt >= SPEC_LINE_MIN && tempCnt <= SPEC_LINE_MAX) {
		iUp = tempPos[i];
	}
	bound[0] = iUp;
	bound[1] = iDown;
	if (iUp <= iDown) {
		printf("水平定位失败");
	}
	else {
		printSuccess("getHorPos");
	}*/
	free(iJumpSum);
	free(iJumpLocation);
	free(iJumpDiff);
	free(tempPos);

	return NULL;
}

//LONG*	getHorBound_CLS(BYTE* imageArr8, LONG width, LONG height, LONG bound[]) {
//	int		i, j, k, m, n, q = 0, tempCnt = 0;
//	LONG*	iJumpSum = (LONG*)calloc(height, sizeof(LONG));
//	const BYTE	JUMP_SUM_MIN = 10;
//
//	for (j = 0; j<height; ++j) {
//		for (i = 1, k = 0; i<width; ++i) {
//			if (imageArr8[j*width + i] - imageArr8[j*width + i - 1] == 255) {
//				++iJumpSum[j];
//				//iJumpLocation[k++] = i - 1;
//			}
//		}
//		if (iJumpSum[j] >= JUMP_SUM_MIN) {
//			//iJumpCouter = 0;
//			//++tempCnt;
//			for (n = 0; n < width; n++)
//			{
//				imageArr8[j*width + n] = 255;
//			}
//		}
//
//	}
//}