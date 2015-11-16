//************************************
// Function:   车牌定位之前的预处理
// 1;	灰度化
// 2:	中值滤波
// 3:	二值化，采用ostu(最大类间方差)算法产出阈值
//************************************
#ifndef _PREPROCESS_H_
#define _PREPROCESS_H_

#include "bitmapStruct.h"

#define CELL_THRESHOLD_DIFF 10 // 上界相异个数阈值
#define CELL_THRESHOLD_WHITE 40 // 上界白色点个数阈值
#define FLOOR_THRESHOLD_DIFF 8 // 下界相异个数阈值
#define FLOOR_THRESHOLD_WHITE 40 // 下界白色点个数阈值
#define FEATURES_CNT 22
#define TRAIN_MODE 0	//训练模式
#define TEST_MODE 1		//测试模式

#define UP		0
#define DOWN	1
#define LEFT	2
#define RIGHT	3

#define BLUE_PL	0   // 蓝底白字车牌
#define YELLOW_PL	1	 // 黄底黑字车牌
#define WHITE_PL	2     // 白底黑字车牌
#define BLACK_PL	3      // 黑底白字车牌
#define OTHERS_PL	4   // 不是车牌区域

//************************************
// Method:    locatePre			车牌定位之前的预处理:原图->灰度化->中值滤波->二值化
// Parameter: BYTE * imageArr
// Parameter: LONG width
// Parameter: LONG height
// Returns:   BYTE*				返回预处理后的图像数组
//************************************
BYTE* locatePre(const BYTE* imageArr, LONG width, LONG height);

BYTE * rotatePre(const BYTE * imageArr, LONG width, LONG height);

 //************************************
 // Method:    rgbToGray 输出给定图片的灰度化后的数据区数组，要求输入的是24位的真彩色的图片
 // Parameter: BYTE * imageArr 24色的真彩色图的一维数组
 // Parameter: LONG width
 // Parameter: LONG height
 // Returns:    BYTE*		返回修改后的灰度化的一维数组
 //************************************
 BYTE* rgbToGray(const BYTE* imageArr, LONG width, LONG height);


 //************************************
 // Method:    medianFilter		中值滤波
 // Parameter: BYTE * imageArr	对传入的数组进行处理
 // Parameter: int width
 // Parameter: int height
 // Returns:   void
 //************************************
 void	medianFilter(BYTE* imageArr, LONG width, LONG height);

 //************************************
 // Method:    soble	Soble边缘检测算子对图像进行边缘检测运算，要求目标图像为灰度图像
 // Parameter: BYTE * imageArr
 // Parameter: LONG width
 // Parameter: LONG height
 // Returns:   void
 //************************************
 void	sobleSideEnhance(BYTE* imageArr, LONG width, LONG height);

 //************************************
 // Method:    binarization		结合直方图和数学期望的全局二值化
 // Parameter: BYTE * imageArr
 // Parameter: LONG width
 // Parameter: LONG height
 // Returns:   void
 //************************************
 void	binarization(BYTE* imageArr, LONG width, LONG height);

  //************************************
 // Method:    ostuThresh		采用ostu(最大类间方差法)求出二值化的阈值
 // Parameter: BYTE * imageArr
 // Parameter: LONG width
 // Parameter: LONG height
 // Returns:   BYTE				返回所求得的阈值
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

 //车牌定位
int		locatePL(char* srcFile, char* destFile);


int		locatePL_clr(char* srcFile, char* destFile);
//int binarization(char* srcFile, char* destFile);//24位真色图转化为8位灰度图的主要函数
//
//int split();
//// 采用双线性插值算法
//int normalized(char* source, char* dest);
//
//// 细化
//void thinner(char* src, char* dest);
//
////特征提取
//void featuresExtract(char* source);
//
////BP神经网络
//void trainNetworkMain();
LONG*	getHorBound(const BYTE* imageArr, LONG width, LONG height, LONG bound[]);
LONG*	getVerBound(const BYTE* imageArr, LONG width, LONG height, LONG bound[]);
void	drawBound(BYTE* bmpArr, LONG width, LONG heigh, LONG bound[]);
// 检查四个边界是否满足要求
int		checkBounds(const LONG bound[]);
#endif
