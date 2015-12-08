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

// ��ȡ�ĸ����������
// side ��ǰ�����ͳ������������
// size �����С
// feature ��������
// ��ǵ�ǰ���ĸ����� 0��1��2��3 �ֱ��Ӧ��������
void getFeatures(int* side, int size, double feature[], int flag) {
	int i, begin, end, n;
	double *leftradio;
	//for (i=0; i<heigh; ++i){
	i = 0;
	if (side[i] == -1)
	{
		while (side[i] == -1) i++;
	}
	begin = i;
	i = size - 1;
	if (side[i] == -1)
	{
		while (side[i] == -1) i--;
	}
	end = i;
	for (i = begin;i <= end;i++)
	{
		if (side[i] == -1)
			side[i] = side[i + 1];
	}
	//ɨ�����
	for (i = begin;i <= end;i++)
	{
		feature[16 + flag] += side[i];
	}
	feature[16 + flag] = feature[16 + flag] / 10;
	leftradio = (double*)malloc(sizeof(double)*(end - begin));
	for (i = 0;i<end - begin;i++)
		leftradio[i] = 0;
	n = 0;
	for (i = begin;i <= (end - 4);i++)
	{
		//���б��Ϊ��
		if (side[i] == side[i + 4])
		{
			feature[3 * flag]++;
			leftradio[n] = 0;
		}
		else
		{
			leftradio[n] = (side[i] - side[i + 4]) / 4.0;
			if (leftradio[n] <0) //���б��С��0
				feature[3 * flag + 1]++;
			else        //б�ʴ���0
				feature[3 * flag + 2]++;
		}
		n++;
	}
	for (i = 1;i<n;i++)
	{
		feature[12 + flag] += fabs(leftradio[i] - leftradio[i - 1]);
	}
	//}
}

double* featuresExtract(const BYTE* imageArr, LONG width, LONG height) {
	int *left, *right, *up, *down;
	//double features[FEATURES_CNT] = { 0 };
	double* features = (double*)calloc(FEATURES_CNT, sizeof(double));
	int i, j, count8;
	int innerPointCnt1, innerPointCnt2;


	left = (int*)malloc(sizeof(int)*height);
	right = (int*)malloc(sizeof(int)*height);
	up = (int*)malloc(sizeof(int)*width);
	down = (int*)malloc(sizeof(int)*width);
	// ��ȡ��������ص�
	for (i = 0; i<height; ++i) {
		left[i] = -1;
		for (j = 0; j<width; ++j) {
			if (imageArr[i*width + j]> 0) {
				left[i] = j;
				break;
			}
		}
	}
	// ��ȡ�Ҳ�������ص�
	for (i = 0; i<height; ++i) {
		right[i] = -1;
		for (j = width - 1; j >= 0; --j) {
			if (imageArr[i*width + j]> 0) {
				right[i] = j;
				break;
			}
		}
	}
	// ��ȡ�ϲ������ص�
	for (i = 0; i<width; ++i) {
		up[i] = -1;
		for (j = 0; j<height; ++j) {
			if (imageArr[j*width + i]> 0) {
				up[i] = j;
				break;
			}
		}
	}
	// ��ȡ�²������ص�
	for (i = 0; i<width; ++i) {
		down[i] = -1;
		for (j = height - 1; j >= 0; --j) {
			if (imageArr[j*width + i]> 0) {
				down[i] = j;
				break;
			}
		}
	}
	getFeatures(left, height, features, 0);
	getFeatures(right, height, features, 1);
	getFeatures(up, width, features, 2);
	getFeatures(down, width, features, 3);

	// ��ȡ�ڲ����ص�
	innerPointCnt1 = 0;
	for (i = 5; i<height - 3; ++i) {
		//down[i] = -1;
		for (j = 5; j<width - 7; ++j) {
			if (imageArr[i*width + j]> 0) {
				++innerPointCnt1;
			}
		}
	}
	features[FEATURES_CNT - 2] = innerPointCnt1;
	// ��ȡ�ڲ����ص�
	innerPointCnt2 = 0;
	for (i = 3; i<10; ++i) {
		//down[i] = -1;
		for (j = 0; j<width; ++j) {
			if (imageArr[i*width + j]> 0) {
				++innerPointCnt2;
			}
		}
	}
	features[FEATURES_CNT - 1] = innerPointCnt2;

	return features;
}

// ��ȡ����������һ��(6*N)*(D+1)�ľ�������NΪ��������������6Ϊ���˺�����������ַ���DΪ����������+1��ʾ��ǰ������Ӧ�ó����ַ�
void	writeFeatures(const char* fileName, FILE* fpFeatures, const BYTE* thinnerImg) {
	//FILE* fpFeatures;
	int		i, j, cur_num;
	int		aCurNum[6];
	char	cur_char;
	int	len = strlen(fileName);
	//char	*carNumbers;	// ��ǰ���Ƶ��߸��ַ�
	double  *features;
	BYTE	*tempArr = (BYTE*)calloc(NORMALIZED_WIDTH*NORMALIZED_HEIGHT, sizeof(BYTE));
	LONG	lTmp = NORMALIZED_WIDTH*NORMALIZED_HEIGHT*sizeof(BYTE);
	// ��ȡ��ǰ���Ƶ��߸��ַ�
	///carNumbers = fileName + len - 12;
	for (i = 1; i < 7; i++)	// �������ַ����б�ţ�0-9��ʾ�ַ�0-9��10-33��ʾ�ַ�A-Z(����I��O)
	{
		cur_char = *(fileName + i + 1);
		//cur_num = (int)*(fileName + i + 1);
		if ((cur_char >= '0' && cur_char <= '9'))
		{
			cur_num = (int)(cur_char - '0');
		}
		else if ((cur_char >= 'A' && cur_char <= 'H'))
		{
			cur_num = (int)(cur_char - 'A') + 11;
		}
		else if ((cur_char >= 'J' && cur_char <= 'N')) // �����ַ�I
		{
			cur_num = (int)(cur_char - 'A') + 10;
		}
		else if ((cur_char >= 'P' && cur_char <= 'Z')) // �����ַ�O
		{
			cur_num = (int)(cur_char - 'A') + 9;
		}
		aCurNum[i - 1] = cur_num;
		//printf("%d ", cur_num);

	}
	// ��ȡ������д���ı���
	//fpFeatures = openTextFile(srcFeatures, "w");
	for (i = 1; i < 7; i++)
	{
		//printf("%d \n", i);
		thinnerImg += lTmp;		// ��������
		memcpy((BYTE*)tempArr, (BYTE*)thinnerImg, lTmp);
		features = featuresExtract(tempArr, NORMALIZED_WIDTH, NORMALIZED_HEIGHT);

		for (j = 0; j<FEATURES_CNT; ++j) {
			//printf("%.2lf ", features[j]);
			fprintf(fpFeatures, "%.2lf ", features[j]);
		}
		// ������ֵ������д���Ӧ�ĳ����ַ�
		fprintf(fpFeatures, "%d \n", aCurNum[i - 1]);
		//printf("\n");
	}
}