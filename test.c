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
//#define SAMPLE_CNT  2 //用来表示已经知道的数据样本的数量,也就是训练样本的数量
//#define OUTPUT_CNT 34 //表示输出层的输出个数
//#define NEURON_CNT 20 // 表示隐藏层神经元的数量
//#define TRAIN_CNT 500 //来表示训练的次数
//#define A  0.2
//#define B  0.4
//#define a  0.2
//#define b  0.3
//
//double	sampleInput[SAMPLE_CNT][FEATURES_CNT],			//存储SAMPLE_CNT个样本的特征值
//sampleOutput[SAMPLE_CNT][OUTPUT_CNT] = { 0 },			//存储SAMPLE_CNT个样本车牌号的实际值的概率，只有一个元素为1，其余为0	
//inputLayerWeight[NEURON_CNT][FEATURES_CNT],		// 表示某个输入对某个神经元的权重
//outputLayerWeight[OUTPUT_CNT][NEURON_CNT],		// 来表示某个神经元对某个输出的权重
//activationValue[NEURON_CNT],					// 隐含层的激活值
//networkOutput[OUTPUT_CNT],						//存储BP神经网络的输出
//maxIn[FEATURES_CNT], minIn[FEATURES_CNT],
//maxOut[OUTPUT_CNT], minOut[OUTPUT_CNT],
//outputLayerWeightDiff[OUTPUT_CNT][NEURON_CNT],	//某个神经元对某个输出的权重的修正量
//inputLayerWeightDiff[NEURON_CNT][FEATURES_CNT];	//输入对某个神经元的权重的修正量
//double	e;	//误差
//int		realNumIndex[SAMPLE_CNT] = { 0 };		//指定sampleOutput元素为1时的下标
//
//
//												//读取所有样本的特征值和实际输出对应的编号
//void readData(char* fileName, int mode) {
//	FILE *fpFeatures, *fpCarNum, *fpWeight;
//	int i, j;
//	double	temp;
//
//	//fpFeatures = openTextFile(TRAIN_FEATRUES_PATH, "r");
//	fpFeatures = openTextFile(fileName, "r");
//	for (i = 0;i<SAMPLE_CNT;i++)	//读入所有样本的特征值
//		for (j = 0; j<FEATURES_CNT; j++)
//			fscanf(fpFeatures, "%lf", &sampleInput[i][j]);
//	fclose(fpFeatures);
//
//	if (mode == TRAIN_MODE) { //训练模式
//		fpCarNum = openTextFile(CAR_NUM_PATH, "r");
//		//for(i=0;i<SAMPLE_CNT;i++)	//读入所有样本的特征值
//		//	for(j=0; j<FEATURES_CNT; j++)
//		//		sampleOutput[i][j] = 0;
//		for (i = 0;i<SAMPLE_CNT;i++) {	//读入所有样本的实际值对应的编号
//			fscanf(fpCarNum, "%d", &realNumIndex[i]);
//			sampleOutput[i][realNumIndex[i]] = 1;
//		}
//		fclose(fpCarNum);
//	}
//	else if (mode == TEST_MODE) {
//		//权重
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
////初始化神经网络，包括输入层特征值的归一化和权重的随机初始化
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
//	// 归一化处理
//	for (i = 0; i < FEATURES_CNT; i++)
//		for (j = 0; j < SAMPLE_CNT; j++)
//			sampleInput[j][i] = (sampleInput[j][i] - minIn[i] + 1) / (maxIn[i] - minIn[i] + 1);
//
//	/*for(i = 0; i < SAMPLE_CNT; i++)
//	sampleOutput[i] /= OUTPUT_CNT;*/
//
//	if (mode == TRAIN_MODE) {
//		//权重随机初始化
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
//	//输入层到隐含层的输出   
//	for (i = 0; i < NEURON_CNT; ++i) {
//		sum = 0;
//		for (j = 0; j < FEATURES_CNT; ++j)
//			sum += inputLayerWeight[i][j] * sampleInput[var][j];
//		activationValue[i] = 1 / (1 + exp(-1 * sum));
//	}
//	/*  隐藏层到输出层输出 */
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
