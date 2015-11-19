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
// ����Ե�ǰ��ɫ��Ϊ���ĵľŹ����е�һ�к����һ�ж��Ǻ�ɫ�㣬�򽫸õ���Ϊ�ڵ�
int		removeHorAloneNoise(BYTE* imageArr8, LONG width, LONG height);
// �ֶ�λˮƽ�߽磬�ɹ�����1��ʧ�ܷ���0
int		getHorBound_CLS(const BYTE* imageArr8, LONG width, LONG height, PBOUNDS bound);
// �ֶ�λ��ֱƽ�߽磬�ɹ�����1��ʧ�ܷ���0
int		getVerBound_CLS(const BYTE* imageArr8, LONG width, LONG height, PBOUNDS bound,double ratio);
int		checkBound(const PBOUNDS bound);
// ��һ�δֶ�λ��0���ɹ���1��ˮƽ��λʧ�ܣ�2:��ֱ��λʧ��
int		firstLocatePL(const BYTE* bmpArr24, LONG width, LONG height, PBOUNDS bound, int PLClr);
int		secLocatePL(BYTE* PLProBmpArr8, LONG width, LONG height, PBOUNDS bound, int PLClr);
BYTE*	extractBmpByBounds(const BYTE* bmpArr, LONG widht, LONG height, PBOUNDS bound, WORD wType);

int		getHorContiPix(BYTE* imageArr8, LONG width, LONG heigh, PBOUNDS bound);
// ��һ��3*12�ľ�����̽����ɫǰ��ͼ������׵����������ֵ����ȫ����Ϊ�׵㣬������Ϊ�ڵ�
int		removeNoise_rect(BYTE* imageArr8, LONG width, LONG heigh);
//********************************
// ȫ�ֱ���
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
	const WORD	wTpye = BF_TYPE_24;		//ָ����ȡ�Ķ�����8λ�ҶȻ���24λ��ɫ��ͼ		
	BOUNDS	bound;			// �ĸ��߽磬�ֱ������������
	
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
		{	// ��һ�ζ�λ�ɹ�
			break;
			status = 1;
		}
		if (res != 0)
		{	// ��һ�ζ�λʧ��
			bound.up = -1;		// ��ʼ�߽磬�����һ�ζ�λʧ�ܶԵڶ��ζ�λ��Ӱ��
			bound.down = -1;
			bound.left = -1;
			bound.right = -1;
			res = secLocatePL(PLProBmpArr8, origWidth, origHeight, &bound, PLClr);
			//creatBmpByArr(destFile, &bmpImage, PLProBmpArr8, 8);
			if (res == 0)
			{ // �ڶ��ζ�λ�ɹ�

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
	printf("������ɫΪ: %d \n", PLClr);
	//printf("%d \n", PLProBmpArr8[0]);
	//bmpArr8 = binarization_PLClr(bmpArr24, origWidht, origHeight, myHSV, 0);
	//medianFilter(bmpArr8, origWidht, origHeight);
	
	//getHorBound_Clr(bmpArr8,origWidht, origHeight, bound);
	//getVerBound(bmpArr8, origWidht, origHeight, bound);
	//drawBound(bmpArr8, origWidht, origHeight, bound);
	
//	printf("%d \n", PLClr);
	if (res == 0)
	{  // ��λ�ɹ�
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
	const	double  PLBoxAddRation = 0.35;		// �ַ������������׵ľ���
	int		PLBoxAdd = 0;		// �ַ������������׵ľ���
	LONG	i;
	int		res = 0;

	imageArr8 = getPLEdgeBmpArr(bmpArr24, width, height, PLClr);
	if (getHorBound_CLS(imageArr8, width, height, bound) == 0)
	{
		res = 1;	// ˮƽ��λʧ��
	}
	else
	{
		if (getVerBound_CLS(imageArr8, width, height, bound, 4.0) == 0)
		{
			res = 2;	// ��ֱ��λʧ��
		}
		else
		{
			/*for (i = 0; i < height; i++)
			{
				imageArr8[i*width + bound->left] = 255;
				imageArr8[i*width + bound->right] = 255;
			}*/
			PLBoxAdd = (bound->up - bound->down) * PLBoxAddRation;
			bound->up += PLBoxAdd;			// ��չ�������½磬ʹ֮�������ƿ�
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
	const	double  PLBoxAddRation = 0.35;		// �ַ������������׵ľ���
	int		PLBoxAdd = 0;		// �ַ������������׵ľ���
	removeNoise_rect(PLProBmpArr8, width, height);

	if (getHorContiPix(PLProBmpArr8, width, height, bound) == 1) {
		
		res = 2;
		if (getVerBound_CLS(PLProBmpArr8, width, height, bound, 4.0) == 1) {
			PLBoxAdd = (bound->up - bound->down) * PLBoxAddRation;
			bound->up += PLBoxAdd;			// ��չ�������½磬ʹ֮�������ƿ�
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
	double	ratio[5] = { 0 };		// ��Ӧ������ɫ���ֵĸ��ʣ��ֱ���������ƣ��ף��ڣ��ǳ��Ʊ�����ɫ
	int		cnt[5] = { 0 };			// ��Ӧ������ɫ���ֵĴ������ֱ���������ƣ��ף��ڣ��ǳ��Ʊ�����ɫ
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
	case BLUE_PL:		//���װ���
		PLCharClr = WHITE_PL;
		break;	
	case YELLOW_PL:		// �Ƶ׺���
		PLCharClr = BLACK_PL;
		break;
	case WHITE_PL:		// �׵׺���
		PLCharClr = BLACK_PL;
		break;
	case BLACK_PL:		// �ڵװ���
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
	else // �׵׺��ֵĳ���ǰ�����ַ������Ǻ�ɫ
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
	// RGB��ɫ�ռ�ת��HSV�ռ�
	myHSV = RGB2HSV(imageArr24, width, height);
	PLProBmpArr8 = (BYTE*)calloc(totalPixls, sizeof(BYTE));
	charProBmpArr8 = (BYTE*)calloc(totalPixls, sizeof(BYTE));

	// ���ƶ�λ��һ������ȡ��ɫ�Ͱ�ɫǰ��ͼ
	getProBmp(PLProBmpArr8, charProBmpArr8, width, height, myHSV, PLClr);
	
	// ���ƶ�λ�ڶ�������RGB��ɫͼ��ҶȻ������б�Ե��⣬����ֵ�����õ���ֵ����Եͼ
	imageArr8 = rgbToGray(imageArr24, width, height);
	sobleSideEnhance(imageArr8, width, height);
	//histogramEqual(tempBmpArr, width, height);
	binarization(imageArr8, width, height);
	dilation(imageArr8, width, height);
	
	// ���ƶ�λ��������ȷ��ͼ������ɫ��Ե��
	intersection(imageArr8, PLProBmpArr8, width, height); // ��ɫ��Ե��
	intersection(imageArr8, charProBmpArr8, width, height); // ��ɫ��Ե��
	// �б���ɫ��Ե�������Ƿ�����а�ɫ��Ե��
	// ������Ϊͼ������ɫ��Ե�㣬��Ϊǰ����ɫ����������
	// Ϊ������ɫ���õ�ͼ������ɫ��Ե��ͼ
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
	
	// �������ػҶ�ͳ��
	for (i = 0; i < totalPixs; i++)
	{
		++numPixel[imageArr8[i]];
	}
	// ����Ҷȷֲ��ܶ�  
	for ( i = 0; i < 256; i++)
	{
		probPixel[i] = numPixel[i] / (double)totalPixs;
		sum += probPixel[i];
		//printf("%d %lf ", numPixel[i], probPixel[i]);
	}
	
	// �����ۼ�ֱ��ͼ�ֲ�
	cumuPixel[0] = probPixel[0];
	for (i = 1; i < 256; i++)
	{
		cumuPixel[i] = cumuPixel[i - 1] + probPixel[i];
	}
	
	// �ۼƷֲ�ȡ��
	for (i = 0; i < 256; i++)
	{
		cumuPixel[i] = floor(255 * cumuPixel[i]);
	}
	
	// �ԻҶ�ֵ����ӳ��
	for (i = 0; i < totalPixs; i++)
	{
		imageArr8[i] = cumuPixel[imageArr8[i]];
	}

	return	0;
}

//��������
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
					//��������������һ��Ϊ255
					//�򽫸õ���Ϊ255
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

//��������
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
					//��������������һ��Ϊ0,�򽫸õ���Ϊ0
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
	const BYTE	JUMP_SUM_MIN = 10;			//�ڰ�����ĸ���
	const LONG	SPEC_LINE_MIN = 10;	//��������Ҫ��Ķ̼�������е����½�
	LONG*	iJumpSum = (LONG*)calloc(height, sizeof(LONG));	// ��¼��j�еĺڰ��������
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
		if (tempPos[i + 1] - tempPos[i] <= 5) { // ���ɱ�׼����ֹ���ڳ��������м�ĳЩ�в�����Ҫ����д�
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
	if (iUp > iDown)		// ʹ�ó��Ƶı߽���չ���������Ʊ߿�λ��
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
	//const	double	WHITE_RATIO_MIN = 0;			// �׵����ڵı���
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
		for (k = 0; k<width - STEP; k += STEP) {// ÿ����ǰ�ƶ�5������
			for (j = bound->down; j <= bound->up; ++j) {//���±߽�
				for (i = k; i<iWidth + k; ++i) {//���ƿ�ȷ�Χ�ڽ��м��
					if (imageArr[j*width + i] == 255) { //ͳ��7��С�����еİ׵����
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
			if (flag == 1) {		// �ҵ���ֱ�߽�
				bound->left = k;
				bound->right = k + iWidth;
				// ��չ�ұ߽�ķ�Χ

				for (k = bound->right; k < width - iPerWidth; k += iPerWidth) {// ÿ����ǰ�ƶ�5������
					flag = 0;
					for (j = bound->down; j <= bound->up; ++j) {//���±߽�
						for (i = k; i < iPerWidth + k; ++i) {//���ƿ�ȷ�Χ�ڽ��м��
							if (imageArr[j*width + i] == 255) { //ͳ�Ƶ�ǰ��С�����еİ׵����
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
						if (bound->right > width) // ��ֹԽ��
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
		ratio += 0.5;		// ���ڿ��
		if (ratio >= 5) {
			break;
		}
	}
	
	
	/*for (i = 0; i < origHeight; i++)
	{
		tempArr[i*origWidth + bound.left] = 255;
		tempArr[i*origWidth + bound.right] = 255;
	}*/
	//printf("��ֱ��λʧ��");
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

	if (bound->up>bound->down && bound->right>bound->left) { // ����Ҫ��Ķ�λ�߽�
		for (i = bound->down, k = 0; i <= bound->up; ++i) {
			for (j = bound->left; j <= bound->right; ++j) {
				if (wType == 8) {
					tempArr[k++] = bmpArr[i*widht + j];
				}
				else if (wType == 24) {
					tmpIndex = (i*widht + j) * 3;
					tempArr[k] = bmpArr[tmpIndex];  // ע��˴��ĸ�ֵ
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
	LONG	*pos = (LONG*)calloc(width, sizeof(LONG)); // ��¼ÿ�а׵��λ��
	LONG	*line = (LONG*)calloc(width, sizeof(LONG));	//��¼ÿ�������׵���������һ����ֵ����
	LONG	tempCnt = 0;
	const	double  PLBoxAddRation = 0.35;		// �ַ������������׵ľ���
	int		PLBoxAdd = 0;		// �ַ������������׵ľ���
	const	LONG	CNT_MIN = 10;			// ÿ�������׵���������ֵ
	const	LONG	SPEC_LINE_MIN = 10;		// ����SPEC_LINE_MIN������Ҫ�����ж�Ϊ��������
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
			if (pos[j + 1] - pos[j] <= 8) { // ���ɱ�׼����ֹ���ڳ��������м�ĳЩ�в�����Ҫ����д�
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
		//bound->up += PLBoxAdd;			// ��չ�������½磬ʹ֮�������ƿ�
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