//************************************
// Function:   ���ƶ�λ֮ǰ��Ԥ����
// 1;	�ҶȻ�
// 2:	��ֵ�˲�
// 3:	��ֵ��������ostu(�����䷽��)�㷨������ֵ
//************************************
#ifndef _PREPROCESS_H_
#define _PREPROCESS_H_

#include "bitmapStruct.h"

#define CELL_THRESHOLD_DIFF 10 // �Ͻ����������ֵ
#define CELL_THRESHOLD_WHITE 40 // �Ͻ��ɫ�������ֵ
#define FLOOR_THRESHOLD_DIFF 8 // �½����������ֵ
#define FLOOR_THRESHOLD_WHITE 40 // �½��ɫ�������ֵ
#define FEATURES_CNT 22
#define TRAIN_MODE 0	//ѵ��ģʽ
#define TEST_MODE 1		//����ģʽ

//************************************
// Method:    locatePre			���ƶ�λ֮ǰ��Ԥ����:ԭͼ->�ҶȻ�->��ֵ�˲�->��ֵ��
// Parameter: BYTE * imageArr
// Parameter: LONG width
// Parameter: LONG height
// Returns:   BYTE*				����Ԥ������ͼ������
//************************************
BYTE* locatePre(const BYTE* imageArr, LONG width, LONG height);

 //************************************
 // Method:    rgbToGray �������ͼƬ�ĻҶȻ�������������飬Ҫ���������24λ�����ɫ��ͼƬ
 // Parameter: BYTE * imageArr 24ɫ�����ɫͼ��һά����
 // Parameter: LONG width
 // Parameter: LONG height
 // Returns:    BYTE*		�����޸ĺ�ĻҶȻ���һά����
 //************************************
 BYTE* rgbToGray(const BYTE* imageArr, LONG width, LONG height);


 //************************************
 // Method:    medianFilter		��ֵ�˲�
 // Parameter: BYTE * imageArr	�Դ����������д���
 // Parameter: int width
 // Parameter: int height
 // Returns:   void
 //************************************
 void	medianFilter(BYTE* imageArr, LONG width, LONG height);

 //************************************
 // Method:    soble	Soble��Ե������Ӷ�ͼ����б�Ե������㣬Ҫ��Ŀ��ͼ��Ϊ�Ҷ�ͼ��
 // Parameter: BYTE * imageArr
 // Parameter: LONG width
 // Parameter: LONG height
 // Returns:   void
 //************************************
 void	sobleSideEnhance(BYTE* imageArr, LONG width, LONG height);

 //************************************
 // Method:    binarization		���ֱ��ͼ����ѧ������ȫ�ֶ�ֵ��
 // Parameter: BYTE * imageArr
 // Parameter: LONG width
 // Parameter: LONG height
 // Returns:   void
 //************************************
 void	binarization(BYTE* imageArr, LONG width, LONG height);

  //************************************
 // Method:    ostuThresh		����ostu(�����䷽�)�����ֵ������ֵ
 // Parameter: BYTE * imageArr
 // Parameter: LONG width
 // Parameter: LONG height
 // Returns:   BYTE				��������õ���ֵ
 //************************************
 BYTE	ostuThresh(BYTE* imageArr, LONG width, LONG height);

 //************************************
 // Method:    removeNoise	  Background curve and noise removing
 // Parameter: BYTE * imageArr
 // Parameter: LONG width
 // Parameter: LONG height
 // Returns:   void
 //************************************
 void	removeNoise(BYTE* imageArr, LONG width, LONG height);

 //���ƶ�λ
int		locatePL(char* srcFile, char* destFile);

//int binarization(char* srcFile, char* destFile);//24λ��ɫͼת��Ϊ8λ�Ҷ�ͼ����Ҫ����
//
//int split();
//// ����˫���Բ�ֵ�㷨
//int normalized(char* source, char* dest);
//
//// ϸ��
//void thinner(char* src, char* dest);
//
////������ȡ
//void featuresExtract(char* source);
//
////BP������
//void trainNetworkMain();

#endif
