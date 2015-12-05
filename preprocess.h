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

#define UP		0
#define DOWN	1
#define LEFT	2
#define RIGHT	3

#define BLUE_PL	0   // ���װ��ֳ���
#define YELLOW_PL	1	 // �Ƶ׺��ֳ���
#define WHITE_PL	2     // �׵׺��ֳ���
#define BLACK_PL	3      // �ڵװ��ֳ���
#define RED_PL	4      // �ڵװ��ֳ����е�ǰ�����ַ������Ǻ�ɫ
#define OTHERS_PL	5   // ���ǳ�������

//************************************
// Method:    locatePre			���ƶ�λ֮ǰ��Ԥ����:ԭͼ->�ҶȻ�->��ֵ�˲�->��ֵ��
// Parameter: BYTE * imageArr
// Parameter: LONG width
// Parameter: LONG height
// Returns:   BYTE*				����Ԥ������ͼ������
//************************************
BYTE* locatePre(const BYTE* imageArr, LONG width, LONG height);

BYTE * rotatePre(const BYTE * imageArr, LONG width, LONG height);

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
 // Parameter: int mode			��Ե����ģʽ��0��ˮƽ��ֱ��Ե��⣬1��ˮƽ��2����ֱ
 // Returns:   void
 //************************************
 void	sobleSideEnhance(BYTE* imageArr, LONG width, LONG height, int mode);

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


int		locatePL_clr(char* srcFile, char* destFile);
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
LONG*	getHorBound(const BYTE* imageArr, LONG width, LONG height, LONG bound[]);
LONG*	getVerBound(const BYTE* imageArr, LONG width, LONG height, LONG bound[]);
void	drawBound(BYTE* bmpArr, LONG width, LONG heigh, LONG bound[]);
// ����ĸ��߽��Ƿ�����Ҫ��
int		checkBounds(const LONG bound[]);


// ֱ��ͼ���⻯
int		histogramEqual(BYTE* imageArr8, LONG width, LONG height);

/*��תRGBͼ�����Ҫ������
image�� ͼ������
iRotateAngle : Ҫ��ת�ĽǶ�
width�� height��ԭʼͼ��Ŀ�ȣ��߶�
lwidth��lheight����ת��ͼ��Ŀ�ȣ��߶�
*/
BYTE *rotateRGB(BYTE *image, float iRotateAngle, LONG width, LONG height, LONG *lwidth, LONG *lheight);

int		hough(BYTE* imageArr, int width, int height);

BYTE*	correction(BYTE* imageArr24, LONG width, LONG height, LONG* cWidth, LONG* cWeight);
void incrementRadon(double *pr, double pixel, double r);
void	radon(double *radonTable, BYTE *imageArr8, double *thetaPtr, LONG width, LONG height,
	LONG xOrigin, LONG yOrigin, int numAngles, LONG rFirst, LONG rSize);
void	myRadon(double *radonTable, BYTE *imageArr8, double *thetaPtr, LONG width, LONG height,
	LONG xOrigin, LONG yOrigin, int numAngles, LONG rFirst, LONG rSize);


int		KTTransform(BYTE *imageArr8, LONG width, LONG height);

BYTE	interpolation(const BYTE* imageArr8, LONG width, LONG height, double x, double y);
BYTE*	rotate(const BYTE* imageArr8, double fAngle, LONG oldWidth, LONG oldHeight, LONG* newWidth, LONG* newHeight);
#endif
