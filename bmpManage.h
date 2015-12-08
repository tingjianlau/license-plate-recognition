#ifndef _FILE_MANAGE_
#define _FILE_MANAGE_

#include "bitmapStruct.h"

#define BI_THRESHOLD 120  // 二值化的阈值
#define NORMALIZED_WIDTH	24	//归一化后图片的宽
#define NORMALIZED_HEIGHT	48	//归一化后图片的高
#define SPLIT_SUCCEED 10
//云AU7526 京H99999 京GT7777.bmp


#define SRC_FILE_PATH "F:\\deep learning\\LPR\\sourceImg\\云AU7526.bmp" // 原位图地址
#define BIN_FILE_PATH "F:\\deep learning\\LPR\\binairzation img\\京GT7777.bmp" // 灰度化后产生的位图地址及文件名
#define FP_DEST_BMPFILE_SPLIT "F:/deep learning/H99999_3.bmp" // 灰度化后产生的位图地址及文件名
#define FP_DEST_BMPFILE_CHARS "F:/deep learning/chars/H99999_"
#define TRAIN_FEATRUES_PATH "F:\\deep learning\\BPNetwork\\trainNetwork\\features.txt"
#define WEIGHT_PATH "F:\\deep learning\\BPNetwork\\trainNetwork\\neuron.txt"
#define CAR_NUM_PATH "F:\\deep learning\\BPNetwork\\trainNetwork\\carNum.txt"
#define TEST_FEATRUES_PATH "F:\\deep learning\\BPNetwork\\testNetwork\\features.txt"

typedef	struct His
{
	double	H;
	double	S;
	double	I;
}HSI, *PHSI;

typedef	struct Hiv
{
	double	H;		// 色调hue
	double	S;		// 饱和度saturation
	double	V;		// 亮度value
}HSV, *PHSV;

//************************************
// Method:    GetBmpHeader 获取位图的头信息
// Parameter: FILE * fpBmp
// Parameter: PBITMAP_IMAGE bmpImage
// Returns:   void
//************************************
void getBmpHeader(FILE* fpBmp, PBITMAP_IMAGE bmpImage);

//************************************
// Method:    updateBmpHeader 更新位图的文件头
// Parameter: PBITMAP_IMAGE bmpImage 新的位图文件头信息
// Parameter: WORD wType	位图文件的类型
// Returns:   void
//************************************
void updateBmpHeader(PBITMAP_IMAGE bmpImage, WORD wType);

//************************************
// Method:    setBmpHeader		写入新的位图文件的头信息
// Parameter: FILE * fpBmp		新的位图文件的指针
// Parameter: const PBITMAP_IMAGE bmpImage	新的位图文件的头信息
// Returns:   void
//************************************
void setBmpHeader(FILE* fpBmp, const PBITMAP_IMAGE bmpImage);

//************************************
// Method:    setRGBQUAD	写入位图文件的颜色表
// Parameter: FILE * fpBmp	新的位图文件的指针
// Returns:   void
//************************************
void setRGBQUAD(FILE* fpBmp);

int realWidth(const PBITMAP_INFO_HEADER pbiheader);
//************************************
// Method:    loadImage		加载位图文件，获取文件头信息并将文件流定位到数据区
// Parameter: char * src	位图文件的地址
// Parameter: BITMAP_IMAGE bmpImage	用来存储文图的头信息
// Returns:   FILE*			返回打开的文件指针
//************************************
FILE* loadImage(char* src, PBITMAP_IMAGE bmpImage);

//************************************
// Method:    creatBmpArr 创建文图数据区的一维数组
// Parameter: FILE * pfBmp 
// Parameter: const PBITMAP_IMAGE bmpImage 位图头文件
// Returns:   BYTE* 返回创建的一维数组
//************************************
BYTE* creatImageArr(FILE* pfBmp, const PBITMAP_IMAGE bmpImage);

//************************************
// Method:    creatBmpByArr		通过给定的一维数组和头文件，创建新的位图,并关闭文件流
// Parameter: char * src
// Parameter: PBITMAP_IMAGE bmpImage
// Parameter: BYTE * imageArr
// Returns:   void
//************************************
void creatBmpByArr(char* src, PBITMAP_IMAGE bmpImage, BYTE* imageArr, WORD wType);

//************************************
// Method:    countingSort	计数排序
// Parameter: BYTE nums[]
// Parameter: int numsSize
// Parameter: int maxValue
// Returns:   BYTE*			返回排序好的数组
//************************************
BYTE*	countingSort(BYTE* nums, int numsSize, int maxValue);


//************************************
// Method:    getMedianValue 调用计数排序，返回中值
// Parameter: int nums
// Parameter: int numsSize
// Parameter: int maxValue
// Returns:   int
//************************************
BYTE	getMedianValue(BYTE* nums, int numsSize, int maxValue);


//************************************
// Method:    extractBmpByBound		根据四个边界，提取图片中相应位置的子图
// Parameter: BYTE * bmpArr
// Parameter: LONG widht
// Parameter: LONG height
// Parameter: LONG bound[]
// Parameter: WORD wType			8或24
// Returns:   BYTE*					返回提取到的图像数组
//************************************
BYTE* extractBmpByBound(const BYTE* bmpArr, LONG widht, LONG height, LONG bound[], WORD wType);

HSI*  RGB2HSI(const BYTE * imageArr24, LONG width, LONG height);

HSV* 	RGB2HSV(const BYTE* imageArr24, LONG width, LONG height);

// 膨胀运算:用一个结构元素(一般是3×3的大小)扫描图像中的每一个像素，
// 用结构元素中的每一个像素与其覆盖的像素做“与”操作，如果都为0，则该像素为0，否则为1
int		dilation(BYTE* imageArr8, int width, int height);
// 腐蚀
int		erosion(BYTE* imageArr8, int width, int height);

// 求在dest位图像素点为255的前提下，两个图像的交集，并更新至dest中
int		intersection(const BYTE* sorce, BYTE* dest, LONG width, LONG height);


//使用Rosenfeld算法的细化算法
void thinnerRosenfeld(BYTE *image, LONG width, LONG height);
void thinnerHilditch(void *image, unsigned long lx, unsigned long ly);
void thinnerPavlidis(void *image, unsigned long lx, unsigned long ly);

FILE*	openTextFile(char* filename, char* mode);

// 反转二值图
void	reverseBinImg(BYTE* biImg, LONG width, LONG heigh);
#endif