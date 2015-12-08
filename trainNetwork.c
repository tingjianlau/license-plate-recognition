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

#define SAMPLE_CNT  2 //������ʾ�Ѿ�֪������������������,Ҳ����ѵ������������
#define OUTPUT_CNT 34 //��ʾ�������������
#define NEURON_CNT 20 // ��ʾ���ز���Ԫ������
#define TRAIN_CNT 500 //����ʾѵ���Ĵ���
#define A  0.2
#define B  0.4
#define a  0.2
#define b  0.3
double	*sampleInput; //�洢SAMPLE_CNT������������ֵ
int	*sampleOutput;	//�洢SAMPLE_CNT���������ƺŵ�ʵ��ֵ�ĸ��ʣ�ֻ��һ��Ԫ��Ϊ1������Ϊ0	
double	inputLayerWeight[NEURON_CNT][FEATURES_CNT],		// ��ʾĳ�������ĳ����Ԫ��Ȩ��
outputLayerWeight[OUTPUT_CNT][NEURON_CNT],		// ����ʾĳ����Ԫ��ĳ�������Ȩ��
activationValue[NEURON_CNT],					// ������ļ���ֵ
networkOutput[OUTPUT_CNT],						//�洢BP����������
maxIn[FEATURES_CNT], minIn[FEATURES_CNT],
maxOut[OUTPUT_CNT], minOut[OUTPUT_CNT],
outputLayerWeightDiff[OUTPUT_CNT][NEURON_CNT],	//ĳ����Ԫ��ĳ�������Ȩ�ص�������
inputLayerWeightDiff[NEURON_CNT][FEATURES_CNT];	//�����ĳ����Ԫ��Ȩ�ص�������
double	e;	//���

int		realNumIndex[SAMPLE_CNT] = { 0 };		//ָ��sampleOutputԪ��Ϊ1ʱ���±�

// ��ȡ����ֵ����ʵֵ
void	readData(const char* src, int mode, const int sampleCnt);
void initBPNework(int mode, const int sampleCnt);
void  trainNetwork(int sampleCnt);
void writeNeuron(const char* weightSrc);

int  trainNetworkMain(const char* featrueSrc, const char* weightSrc, int sampleCnt)
{
	int mode = 0, i, j;
	
	//printf("sampleCnt -> %d \n", sampleCnt);
	readData(featrueSrc, mode, sampleCnt);
	/*for (i = 0; i < sampleCnt*FEATURES_CNT; i++)
	{
		printf("%.2lf ", sampleInput[i]);
	}
	printf("\n");
	for (i = 0; i < sampleCnt*OUTPUT_CNT; i++)
	{
		printf("%d ", sampleOutput[i]);
	}
	printf("\n");*/
	initBPNework(mode, sampleCnt);

	if (mode == TRAIN_MODE) {
		trainNetwork(sampleCnt);
		writeNeuron(weightSrc);
	}
	else if (mode == TEST_MODE) {
		/*for (i = 0; i<SAMPLE_CNT; ++i) {
			computO(i);
			printf("%d \n", getExtremumIndex(networkOutput, OUTPUT_CNT, 1));
		}*/
	}
}
void readData(const char* src, int mode, const int sampleCnt) {
	FILE *fpFeatures, *fpCarNum, *fpWeight;
	int i, j;
	//int	realPLCharIndex;
	double	tmpFea;
	double*		temp = NULL;

	sampleInput = (double*)calloc(sampleCnt*FEATURES_CNT, sizeof(double));
	sampleOutput = (int*)calloc(sampleCnt*OUTPUT_CNT, sizeof(int));
	temp = sampleInput;

	fpFeatures = openTextFile(src, "r");

	for (i = 0;i < sampleCnt;i++) {
		for (j = 0; j < FEATURES_CNT; j++) {
			fscanf(fpFeatures, "%lf", temp++);
			//*(temp++) = tmpFea;

			//printf("%lf ", *(temp-1));
			//sampleInput = sampleInput + 1;
		}
		//printf("\n");
		fscanf(fpFeatures, "%d", &realNumIndex[i]);
		//printf("%d \n", realPLCharIndex);
		*(sampleOutput + i*OUTPUT_CNT + realNumIndex[i]) = 1;
		//sampleOutput[i][realPLCharIndex] = 1;
	}
	
	//fclose(fpFeatures);

	//if (mode == TRAIN_MODE) { //ѵ��ģʽ
	//	fpCarNum = openTextFile(CAR_NUM_PATH, "r");
	//	
	//	for (i = 0;i<sampleCnt;i++) {	//��������������ʵ��ֵ��Ӧ�ı��
	//		fscanf(fpCarNum, "%d", &realNumIndex[i]);
	//		sampleOutput[i][realNumIndex[i]] = 1;
	//	}
	//	fclose(fpCarNum);
	//}
	//else if (mode == TEST_MODE) {
	//	//Ȩ��
	//	fpWeight = openTextFile(WEIGHT_PATH, "r");
	//	for (i = 0; i < NEURON_CNT; ++i) {
	//		for (j = 0; j < FEATURES_CNT; ++j) {
	//			fscanf(fpWeight, "%lf", &temp);
	//			inputLayerWeight[i][j] = temp;
	//		}
	//	}

	//	for (i = 0; i < NEURON_CNT; ++i) {
	//		for (j = 0; j < OUTPUT_CNT; ++j) {
	//			fscanf(fpWeight, "%lf", &temp);
	//			outputLayerWeight[j][i] = temp;
	//		}
	//	}
	//}
	fclose(fpFeatures);
}

//��ʼ�������磬�������������ֵ�Ĺ�һ����Ȩ�ص������ʼ��
void initBPNework(int mode, const int sampleCnt) {
	int i, j;
	int	curIndex;
	for (i = 0; i<FEATURES_CNT; i++) {
		//minIn[i] = maxIn[i] = sampleInput[0][i];
		minIn[i] = maxIn[i] = sampleInput[i];
		for (j = 0; j<sampleCnt; j++)
		{
			curIndex = j*FEATURES_CNT + i;
			maxIn[i] = maxIn[i]>sampleInput[curIndex] ? maxIn[i] : sampleInput[curIndex];
			minIn[i] = minIn[i]<sampleInput[curIndex] ? minIn[i] : sampleInput[curIndex];
		}
	}
	// ��һ������
	for (i = 0; i < FEATURES_CNT; i++)
		for (j = 0; j < sampleCnt; j++) {
			curIndex = j*FEATURES_CNT + i;
			sampleInput[curIndex] = (sampleInput[curIndex] - minIn[i] + 1) / (maxIn[i] - minIn[i] + 1);
		}
			

	/*for(i = 0; i < sampleCnt; i++)
	sampleOutput[i] /= OUTPUT_CNT;*/

	if (mode == TRAIN_MODE) {
		//Ȩ�������ʼ��
		for (i = 0; i < NEURON_CNT; ++i) {
			for (j = 0; j < FEATURES_CNT; ++j) {
				inputLayerWeight[i][j] = (rand()*2.0 / RAND_MAX - 1) / 2;
				inputLayerWeightDiff[i][j] = 0;
			}
		}

		for (i = 0; i < NEURON_CNT; ++i) {
			for (j = 0; j < OUTPUT_CNT; ++j) {
				outputLayerWeight[j][i] = (rand()*2.0 / RAND_MAX - 1) / 2;
				outputLayerWeightDiff[j][i] = 0;
			}
		}
	}

}

void computO(int var) {
	int i, j;
	double sum;
	//����㵽����������   
	for (i = 0; i < NEURON_CNT; ++i) {
		sum = 0;
		for (j = 0; j < FEATURES_CNT; ++j)
			//sum += inputLayerWeight[i][j] * sampleInput[var][j];
			sum += inputLayerWeight[i][j] * sampleInput[var*FEATURES_CNT + j];
		activationValue[i] = 1 / (1 + exp(-1 * sum));
	}
	/*  ���ز㵽�������� */
	for (i = 0; i < OUTPUT_CNT; ++i) {
		sum = 0;
		for (j = 0; j < NEURON_CNT; ++j)
			sum += outputLayerWeight[i][j] * activationValue[j];

		networkOutput[i] = sum;
	}
	//printArrDbe(networkOutput, OUTPUT_CNT);
}

void backUpdate(int var)
{
	int i, j;
	int	curIndex;
	double t;
	for (i = 0; i < NEURON_CNT; ++i)
	{
		t = 0;
		for (j = 0; j < OUTPUT_CNT; ++j) {
			curIndex = var*OUTPUT_CNT + j;
			t += (networkOutput[j] - sampleOutput[curIndex])*outputLayerWeight[j][i];

			outputLayerWeightDiff[j][i] = A*outputLayerWeightDiff[j][i] + B*(networkOutput[j] - sampleOutput[curIndex])*activationValue[i];
			outputLayerWeight[j][i] -= outputLayerWeightDiff[j][i];
		}

		for (j = 0; j < FEATURES_CNT; ++j) {
			curIndex = var*FEATURES_CNT + j;
			inputLayerWeightDiff[i][j] = a*inputLayerWeightDiff[i][j] + b*t*activationValue[i] * (1 - activationValue[i])*sampleInput[curIndex];
			inputLayerWeight[i][j] -= inputLayerWeightDiff[i][j];
		}
	}
}

void  trainNetwork(int sampleCnt) {
	int i, c = 0;
	do {
		e = 0;
		for (i = 0; i < sampleCnt; ++i) {
			computO(i);
			//for (j = 0; j < OUTPUT_CNT; ++j)
			e += fabs(networkOutput[realNumIndex[i]] - sampleOutput[i*OUTPUT_CNT + realNumIndex[i]]);
			backUpdate(i);
		}
		//printf("%d  %lf\n", c, e / sampleCnt);
		c++;
	} while (c<TRAIN_CNT && e / sampleCnt>0.01);
	//printArrDbe(networkOutput, FEATURES_CNT);
}


void writeNeuron(const char* weightSrc)
{
	FILE *fpNeuron;
	int i, j;
	//printf("%s \n", weightSrc);
	fpNeuron = openTextFile(weightSrc, "w");
	for (i = 0; i < NEURON_CNT; ++i)
		for (j = 0; j < FEATURES_CNT; ++j)
			fprintf(fpNeuron, "%lf ", inputLayerWeight[i][j]);

	fprintf(fpNeuron, "\n\n\n\n");

	for (i = 0; i < NEURON_CNT; ++i)
		for (j = 0; j < OUTPUT_CNT; ++j)
			fprintf(fpNeuron, "%lf ", outputLayerWeight[j][i]);

	fclose(fpNeuron);
}