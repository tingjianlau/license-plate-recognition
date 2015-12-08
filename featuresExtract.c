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

// 提取四个侧面的特征
// side 当前侧面的统计特征点数组
// size 数组大小
// feature 特征数组
// 标记当前是哪个侧面 0，1，2，3 分别对应左右上下
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
	//扫描深度
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
		//如果斜率为零
		if (side[i] == side[i + 4])
		{
			feature[3 * flag]++;
			leftradio[n] = 0;
		}
		else
		{
			leftradio[n] = (side[i] - side[i + 4]) / 4.0;
			if (leftradio[n] <0) //如果斜率小于0
				feature[3 * flag + 1]++;
			else        //斜率大于0
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
	// 提取左侧面像素点
	for (i = 0; i<height; ++i) {
		left[i] = -1;
		for (j = 0; j<width; ++j) {
			if (imageArr[i*width + j]> 0) {
				left[i] = j;
				break;
			}
		}
	}
	// 提取右侧面边像素点
	for (i = 0; i<height; ++i) {
		right[i] = -1;
		for (j = width - 1; j >= 0; --j) {
			if (imageArr[i*width + j]> 0) {
				right[i] = j;
				break;
			}
		}
	}
	// 提取上侧面像素点
	for (i = 0; i<width; ++i) {
		up[i] = -1;
		for (j = 0; j<height; ++j) {
			if (imageArr[j*width + i]> 0) {
				up[i] = j;
				break;
			}
		}
	}
	// 提取下侧面像素点
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

	// 提取内部像素点
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
	// 提取内部像素点
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

// 提取特征，创建一个(6*N)*(D+1)的矩阵，其中N为车牌样本数量，6为除了汉字外的六个字符，D为特征数量，+1表示当前特征对应得车牌字符
void	writeFeatures(const char* fileName, FILE* fpFeatures, const BYTE* thinnerImg) {
	//FILE* fpFeatures;
	int		i, j, cur_num;
	int		aCurNum[6];
	char	cur_char;
	int	len = strlen(fileName);
	//char	*carNumbers;	// 当前车牌的七个字符
	double  *features;
	BYTE	*tempArr = (BYTE*)calloc(NORMALIZED_WIDTH*NORMALIZED_HEIGHT, sizeof(BYTE));
	LONG	lTmp = NORMALIZED_WIDTH*NORMALIZED_HEIGHT*sizeof(BYTE);
	// 提取当前车牌的七个字符
	///carNumbers = fileName + len - 12;
	for (i = 1; i < 7; i++)	// 将车牌字符进行编号：0-9表示字符0-9，10-33表示字符A-Z(除了I和O)
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
		else if ((cur_char >= 'J' && cur_char <= 'N')) // 忽略字符I
		{
			cur_num = (int)(cur_char - 'A') + 10;
		}
		else if ((cur_char >= 'P' && cur_char <= 'Z')) // 忽略字符O
		{
			cur_num = (int)(cur_char - 'A') + 9;
		}
		aCurNum[i - 1] = cur_num;
		//printf("%d ", cur_num);

	}
	// 提取特征并写入文本中
	//fpFeatures = openTextFile(srcFeatures, "w");
	for (i = 1; i < 7; i++)
	{
		//printf("%d \n", i);
		thinnerImg += lTmp;		// 跳过汉字
		memcpy((BYTE*)tempArr, (BYTE*)thinnerImg, lTmp);
		features = featuresExtract(tempArr, NORMALIZED_WIDTH, NORMALIZED_HEIGHT);

		for (j = 0; j<FEATURES_CNT; ++j) {
			//printf("%.2lf ", features[j]);
			fprintf(fpFeatures, "%.2lf ", features[j]);
		}
		// 在特征值向量后写入对应的车牌字符
		fprintf(fpFeatures, "%d \n", aCurNum[i - 1]);
		//printf("\n");
	}
}