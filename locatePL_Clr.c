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

int		locatePL_clr(char* srcFile, char* destFile) {
	FILE	*fpSrc = NULL,
		*fpDest = NULL;
	LONG	i, j, k, temp = 0;
	BYTE	*bmpArr8, *bmpArr24, *firstLocateBmpArr, *secLocateBmpArr, *tempArr, *rotatePreBmpArr;
	LONG	origWidht, origHeight, locateWidth, locateHeight;
	BITMAP_IMAGE bmpImage;
	LONG	rotateWidth, rotateHeight;
	const WORD	wTpye = BF_TYPE_24;		//ָ����ȡ�Ķ�����8λ�ҶȻ���24λ��ɫ��ͼ		
	LONG	bound[4] = { 0 };			// �ĸ��߽磬�ֱ������������
	HSV*	myHSV = NULL;
	int		PLClr;

	fpSrc = loadImage(srcFile, &bmpImage);
	bmpArr24 = creatImageArr(fpSrc, &bmpImage);
	origWidht = bmpImage.infoHeader.biWidth;
	origHeight = bmpImage.infoHeader.biHeight;

	myHSV = RGB2HSV(bmpArr24, origWidht, origHeight);
	PLClr = judgePLClr(myHSV, origWidht, origHeight);
	if (PLClr == -1)
	{
		printf("�жϳ��Ƶĵ�ɫʧ�� \n");
		//free(myHSV);
		//return 1;
	}
	else
	{
		printf("��ǰ���ĵ�ɫ��: %d\n",PLClr);
	}
	bmpArr8 = binarization_PLClr(bmpArr24, origWidht, origHeight, myHSV, 0);
	medianFilter(bmpArr8, origWidht, origHeight);

	getHorBound_Clr(bmpArr8,origWidht, origHeight, bound);
	getVerBound(bmpArr8, origWidht, origHeight, bound);
	drawBound(bmpArr8, origWidht, origHeight, bound);
	creatBmpByArr(destFile, &bmpImage, bmpArr8, 8);
//	printf("%d \n", PLClr);

	free(myHSV);

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
	int		res;
	if ((H>=161 && H<=301) && (S>=0.25 && S<=1) && (V>=0.25&& V<=1))
		res = BLUE_PL;
	else if ((H >= 20 && H <= 60) && (S >= 0.25 && S <= 1) && (V >= 0.25&& V <= 1))
		res = YELLOW_PL;
	else if ((S >= 0.0 && S <= 0.25) && (V >= 0.7&& V <= 1))
		res = WHITE_PL;
	else if ((V >= 0&& V <= 0.25))
		res = BLACK_PL;
	else
		res = OTHERS_PL;
	return res;
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
