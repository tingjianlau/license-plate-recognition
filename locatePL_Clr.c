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
// ����Ե�ǰ��ɫ��Ϊ���ĵľŹ����е�һ�к����һ�ж��Ǻ�ɫ�㣬�򽫸õ���Ϊ�ڵ�
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
	const WORD	wTpye = BF_TYPE_24;		//ָ����ȡ�Ķ�����8λ�ҶȻ���24λ��ɫ��ͼ		
	LONG	bound[4] = { 0 };			// �ĸ��߽磬�ֱ������������
	
	int		PLClr = 0;

	fpSrc = loadImage(srcFile, &bmpImage);
	bmpArr24 = creatImageArr(fpSrc, &bmpImage);
	origWidth = bmpImage.infoHeader.biWidth;
	origHeight = bmpImage.infoHeader.biHeight;

	
	//PLClr = judgePLClr(myHSV, origWidht, origHeight);
	//if (PLClr == -1)
	//{
	//	printf("�жϳ��Ƶĵ�ɫʧ�� \n");
	//	//free(myHSV);
	//	//return 1;
	//}
	//else
	//{
	//	printf("��ǰ���ĵ�ɫ��: %d\n",PLClr);
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
LONG*	getHorBound_Clr(const BYTE* imageArr, LONG width, LONG height, LONG bound[]) {
	int		i, j, k, m, n, q = 0, tempCnt = 0;
	//const BYTE	JUMP_SUM_MIN = 12, JUMP_SUM_MAX = 30;	//�ڰ�����ĸ���
	const BYTE	JUMP_SUM_MIN = 10, JUMP_SUM_MAX = 30;	//�ڰ�����ĸ���
	//const BYTE	JUMP_COUNTER_MIN = 6, JUMP_COUNTER_MAX = 18;	//����涨�����ĸ���
	const BYTE	JUMP_COUNTER_MIN = 2, JUMP_COUNTER_MAX = 18;	//����涨�����ĸ���
	//const BYTE	JUMP_DIFF_MIN = 4, JUMP_DIFF_MAX = 25;	//�������κڰ����ľ����
	const BYTE	JUMP_DIFF_MIN = 0, JUMP_DIFF_MAX = 25;	//�������κڰ����ľ����
	const LONG	SPEC_LINE_MIN = 15, SPEC_LINE_MAX = 90;	//��������Ҫ��Ķ̼�������е����½�
	LONG*	iJumpSum = (LONG*)calloc(height, sizeof(LONG));	// ��¼��j�еĺڰ��������
	LONG*	iJumpLocation = (LONG*)calloc(width / 2 + 1, sizeof(LONG));	//��¼��ǰ�з����ڰ������λ��
	LONG*	iJumpDiff = (LONG*)calloc(width / 2 + 1, sizeof(LONG));		//��¼���κڰ�����ļ��
	LONG*	tempPos = (LONG*)calloc(height, sizeof(LONG));
	//LONG*	iJumpCouter = (LONG*)calloc(width/2+1, sizeof(LONG));		//��¼����涨�����ĸ���
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
			for (m = 0; m<k - 1; ++m) {//�����������κڰ�����ľ����
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
		printf("ˮƽ��λʧ��");
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
	// RGB��ɫ�ռ�ת��HSV�ռ�
	myHSV = RGB2HSV(imageArr24, width, height);
	
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

LONG*	getHorBound_CLS(BYTE* imageArr8, LONG width, LONG height, LONG bound[]) {
	int		i, j, k, m, n, q = 0, tempCnt = 0;
	const BYTE	JUMP_SUM_MIN = 10, JUMP_SUM_MAX = 3000;			//�ڰ�����ĸ���
	const BYTE	JUMP_COUNTER_MIN = 2, JUMP_COUNTER_MAX = 18;	//����涨�����ĸ���
	const BYTE	JUMP_DIFF_MIN = 2, JUMP_DIFF_MAX = 25;	//�������κڰ����ľ����
	const LONG	SPEC_LINE_MIN = 15, SPEC_LINE_MAX = 90;	//��������Ҫ��Ķ̼�������е����½�
	LONG*	iJumpSum = (LONG*)calloc(height, sizeof(LONG));	// ��¼��j�еĺڰ��������
	LONG*	iJumpLocation = (LONG*)calloc(width / 2 + 1, sizeof(LONG));	//��¼��ǰ�з����ڰ������λ��
	LONG*	iJumpDiff = (LONG*)calloc(width / 2 + 1, sizeof(LONG));		//��¼���κڰ�����ļ��
	LONG*	tempPos = (LONG*)calloc(height, sizeof(LONG));
	//LONG*	iJumpCouter = (LONG*)calloc(width/2+1, sizeof(LONG));		//��¼����涨�����ĸ���
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
	//		for (m = 0; m<k - 1; m += 2) {//�����������κڰ�����ľ����
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
			//for (m = 0; m<k - 1; m++) {//�����������κڰ�����ľ����
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
		printf("ˮƽ��λʧ��");
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