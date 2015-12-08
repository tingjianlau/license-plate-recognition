//#define _CRT_SECURE_NO_WARNINGS
//#include <stdio.h>
//#include <time.h>
//#include <math.h>
//#include <stdlib.h>
//
//#include "util.h"
//#include "bitmapStruct.h"
//#include "fileManage.h"
//#include "preprocess.h"
//
//#define SAMPLE_CNT  2 //������ʾ�Ѿ�֪������������������,Ҳ����ѵ������������
//#define OUTPUT_CNT 34 //��ʾ�������������
//#define NEURON_CNT 20 // ��ʾ���ز���Ԫ������
//#define TRAIN_CNT 500 //����ʾѵ���Ĵ���
//#define A  0.2
//#define B  0.4
//#define a  0.2
//#define b  0.3
//
//double	sampleInput[SAMPLE_CNT][FEATURES_CNT],			//�洢SAMPLE_CNT������������ֵ
//sampleOutput[SAMPLE_CNT][OUTPUT_CNT] = { 0 },			//�洢SAMPLE_CNT���������ƺŵ�ʵ��ֵ�ĸ��ʣ�ֻ��һ��Ԫ��Ϊ1������Ϊ0	
//inputLayerWeight[NEURON_CNT][FEATURES_CNT],		// ��ʾĳ�������ĳ����Ԫ��Ȩ��
//outputLayerWeight[OUTPUT_CNT][NEURON_CNT],		// ����ʾĳ����Ԫ��ĳ�������Ȩ��
//activationValue[NEURON_CNT],					// ������ļ���ֵ
//networkOutput[OUTPUT_CNT],						//�洢BP����������
//maxIn[FEATURES_CNT], minIn[FEATURES_CNT],
//maxOut[OUTPUT_CNT], minOut[OUTPUT_CNT],
//outputLayerWeightDiff[OUTPUT_CNT][NEURON_CNT],	//ĳ����Ԫ��ĳ�������Ȩ�ص�������
//inputLayerWeightDiff[NEURON_CNT][FEATURES_CNT];	//�����ĳ����Ԫ��Ȩ�ص�������
//double	e;	//���
//int		realNumIndex[SAMPLE_CNT] = { 0 };		//ָ��sampleOutputԪ��Ϊ1ʱ���±�
//
//
//												//��ȡ��������������ֵ��ʵ�������Ӧ�ı��
//void readData(char* fileName, int mode) {
//	FILE *fpFeatures, *fpCarNum, *fpWeight;
//	int i, j;
//	double	temp;
//
//	//fpFeatures = openTextFile(TRAIN_FEATRUES_PATH, "r");
//	fpFeatures = openTextFile(fileName, "r");
//	for (i = 0;i<SAMPLE_CNT;i++)	//������������������ֵ
//		for (j = 0; j<FEATURES_CNT; j++)
//			fscanf(fpFeatures, "%lf", &sampleInput[i][j]);
//	fclose(fpFeatures);
//
//	if (mode == TRAIN_MODE) { //ѵ��ģʽ
//		fpCarNum = openTextFile(CAR_NUM_PATH, "r");
//		//for(i=0;i<SAMPLE_CNT;i++)	//������������������ֵ
//		//	for(j=0; j<FEATURES_CNT; j++)
//		//		sampleOutput[i][j] = 0;
//		for (i = 0;i<SAMPLE_CNT;i++) {	//��������������ʵ��ֵ��Ӧ�ı��
//			fscanf(fpCarNum, "%d", &realNumIndex[i]);
//			sampleOutput[i][realNumIndex[i]] = 1;
//		}
//		fclose(fpCarNum);
//	}
//	else if (mode == TEST_MODE) {
//		//Ȩ��
//		fpWeight = openTextFile(WEIGHT_PATH, "r");
//		for (i = 0; i < NEURON_CNT; ++i) {
//			for (j = 0; j < FEATURES_CNT; ++j) {
//				fscanf(fpWeight, "%lf", &temp);
//				inputLayerWeight[i][j] = temp;
//			}
//		}
//
//		for (i = 0; i < NEURON_CNT; ++i) {
//			for (j = 0; j < OUTPUT_CNT; ++j) {
//				fscanf(fpWeight, "%lf", &temp);
//				outputLayerWeight[j][i] = temp;
//			}
//		}
//	}
//}
//
////��ʼ�������磬�������������ֵ�Ĺ�һ����Ȩ�ص������ʼ��
//void initBPNework(int mode) {
//	int i, j;
//	for (i = 0; i<FEATURES_CNT; i++) {
//		minIn[i] = maxIn[i] = sampleInput[0][i];
//		for (j = 0; j<SAMPLE_CNT; j++)
//		{
//			maxIn[i] = maxIn[i]>sampleInput[j][i] ? maxIn[i] : sampleInput[j][i];
//			minIn[i] = minIn[i]<sampleInput[j][i] ? minIn[i] : sampleInput[j][i];
//		}
//	}
//	// ��һ������
//	for (i = 0; i < FEATURES_CNT; i++)
//		for (j = 0; j < SAMPLE_CNT; j++)
//			sampleInput[j][i] = (sampleInput[j][i] - minIn[i] + 1) / (maxIn[i] - minIn[i] + 1);
//
//	/*for(i = 0; i < SAMPLE_CNT; i++)
//	sampleOutput[i] /= OUTPUT_CNT;*/
//
//	if (mode == TRAIN_MODE) {
//		//Ȩ�������ʼ��
//		for (i = 0; i < NEURON_CNT; ++i) {
//			for (j = 0; j < FEATURES_CNT; ++j) {
//				inputLayerWeight[i][j] = (rand()*2.0 / RAND_MAX - 1) / 2;
//				inputLayerWeightDiff[i][j] = 0;
//			}
//		}
//
//		for (i = 0; i < NEURON_CNT; ++i) {
//			for (j = 0; j < OUTPUT_CNT; ++j) {
//				outputLayerWeight[j][i] = (rand()*2.0 / RAND_MAX - 1) / 2;
//				outputLayerWeightDiff[j][i] = 0;
//			}
//		}
//	}
//
//}
//
//void computO(int var) {
//	int i, j;
//	double sum;
//	//����㵽����������   
//	for (i = 0; i < NEURON_CNT; ++i) {
//		sum = 0;
//		for (j = 0; j < FEATURES_CNT; ++j)
//			sum += inputLayerWeight[i][j] * sampleInput[var][j];
//		activationValue[i] = 1 / (1 + exp(-1 * sum));
//	}
//	/*  ���ز㵽�������� */
//	for (i = 0; i < OUTPUT_CNT; ++i) {
//		sum = 0;
//		for (j = 0; j < NEURON_CNT; ++j)
//			sum += outputLayerWeight[i][j] * activationValue[j];
//
//		networkOutput[i] = sum;
//	}
//	printArrDbe(networkOutput, OUTPUT_CNT);
//}
//
//void backUpdate(int var)
//{
//	int i, j;
//	double t;
//	for (i = 0; i < NEURON_CNT; ++i)
//	{
//		t = 0;
//		for (j = 0; j < OUTPUT_CNT; ++j) {
//			t += (networkOutput[j] - sampleOutput[var][j])*outputLayerWeight[j][i];
//
//			outputLayerWeightDiff[j][i] = A*outputLayerWeightDiff[j][i] + B*(networkOutput[j] - sampleOutput[var][j])*activationValue[i];
//			outputLayerWeight[j][i] -= outputLayerWeightDiff[j][i];
//		}
//
//		for (j = 0; j < FEATURES_CNT; ++j) {
//			inputLayerWeightDiff[i][j] = a*inputLayerWeightDiff[i][j] + b*t*activationValue[i] * (1 - activationValue[i])*sampleInput[var][j];
//			inputLayerWeight[i][j] -= inputLayerWeightDiff[i][j];
//		}
//	}
//}
//
//void writeNeuron()
//{
//	FILE *fpNeuron;
//	int i, j;
//	fpNeuron = openTextFile(WEIGHT_PATH, "w");
//	for (i = 0; i < NEURON_CNT; ++i)
//		for (j = 0; j < FEATURES_CNT; ++j)
//			fprintf(fpNeuron, "%lf ", inputLayerWeight[i][j]);
//
//	fprintf(fpNeuron, "\n\n\n\n");
//
//	for (i = 0; i < NEURON_CNT; ++i)
//		for (j = 0; j < OUTPUT_CNT; ++j)
//			fprintf(fpNeuron, "%lf ", outputLayerWeight[j][i]);
//
//	fclose(fpNeuron);
//}
//
//void  trainNetwork() {
//	int i, c = 0;
//	do {
//		e = 0;
//		for (i = 0; i < SAMPLE_CNT; ++i) {
//			computO(i);
//			//for (j = 0; j < OUTPUT_CNT; ++j)
//			e += fabs(networkOutput[realNumIndex[i]] - sampleOutput[i][realNumIndex[i]]);
//			backUpdate(i);
//		}
//		printf("%d  %lf\n", c, e / SAMPLE_CNT);
//		c++;
//	} while (c<TRAIN_CNT && e / SAMPLE_CNT>0.01);
//	printArrDbe(networkOutput, FEATURES_CNT);
//}
//
////void  trainNetworkMain()
////{
////	int mode = 1, i;
////	readData(TRAIN_FEATRUES_PATH, mode);
////	initBPNework(mode);
////	if (mode == TRAIN_MODE) {
////		trainNetwork();
////		writeNeuron();
////	}
////	else if (mode == TEST_MODE) {
////		for (i = 0; i<SAMPLE_CNT; ++i) {
////			computO(i);
////			printf("%d \n", getExtremumIndex(networkOutput, OUTPUT_CNT, 1));
////		}
////	}
////}
//
//
