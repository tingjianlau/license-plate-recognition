#ifndef _FILE_MANAGE_
#define _FILE_MANAGE_

#include "bitmapStruct.h"

#define BI_THRESHOLD 120  // ��ֵ������ֵ
#define NORMALIZED_WIDTH	24	//��һ����ͼƬ�Ŀ�
#define NORMALIZED_HEIGHT	48	//��һ����ͼƬ�ĸ�
#define SPLIT_SUCCEED 10
//��AU7526 ��H99999 ��GT7777.bmp


#define SRC_FILE_PATH "F:\\deep learning\\LPR\\sourceImg\\��AU7526.bmp" // ԭλͼ��ַ
#define BIN_FILE_PATH "F:\\deep learning\\LPR\\binairzation img\\��GT7777.bmp" // �ҶȻ��������λͼ��ַ���ļ���
#define FP_DEST_BMPFILE_SPLIT "F:/deep learning/H99999_3.bmp" // �ҶȻ��������λͼ��ַ���ļ���
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
	double	H;		// ɫ��hue
	double	S;		// ���Ͷ�saturation
	double	V;		// ����value
}HSV, *PHSV;

//************************************
// Method:    GetBmpHeader ��ȡλͼ��ͷ��Ϣ
// Parameter: FILE * fpBmp
// Parameter: PBITMAP_IMAGE bmpImage
// Returns:   void
//************************************
void getBmpHeader(FILE* fpBmp, PBITMAP_IMAGE bmpImage);

//************************************
// Method:    updateBmpHeader ����λͼ���ļ�ͷ
// Parameter: PBITMAP_IMAGE bmpImage �µ�λͼ�ļ�ͷ��Ϣ
// Parameter: WORD wType	λͼ�ļ�������
// Returns:   void
//************************************
void updateBmpHeader(PBITMAP_IMAGE bmpImage, WORD wType);

//************************************
// Method:    setBmpHeader		д���µ�λͼ�ļ���ͷ��Ϣ
// Parameter: FILE * fpBmp		�µ�λͼ�ļ���ָ��
// Parameter: const PBITMAP_IMAGE bmpImage	�µ�λͼ�ļ���ͷ��Ϣ
// Returns:   void
//************************************
void setBmpHeader(FILE* fpBmp, const PBITMAP_IMAGE bmpImage);

//************************************
// Method:    setRGBQUAD	д��λͼ�ļ�����ɫ��
// Parameter: FILE * fpBmp	�µ�λͼ�ļ���ָ��
// Returns:   void
//************************************
void setRGBQUAD(FILE* fpBmp);

int realWidth(const PBITMAP_INFO_HEADER pbiheader);
//************************************
// Method:    loadImage		����λͼ�ļ�����ȡ�ļ�ͷ��Ϣ�����ļ�����λ��������
// Parameter: char * src	λͼ�ļ��ĵ�ַ
// Parameter: BITMAP_IMAGE bmpImage	�����洢��ͼ��ͷ��Ϣ
// Returns:   FILE*			���ش򿪵��ļ�ָ��
//************************************
FILE* loadImage(char* src, PBITMAP_IMAGE bmpImage);

//************************************
// Method:    creatBmpArr ������ͼ��������һά����
// Parameter: FILE * pfBmp 
// Parameter: const PBITMAP_IMAGE bmpImage λͼͷ�ļ�
// Returns:   BYTE* ���ش�����һά����
//************************************
BYTE* creatImageArr(FILE* pfBmp, const PBITMAP_IMAGE bmpImage);

//************************************
// Method:    creatBmpByArr		ͨ��������һά�����ͷ�ļ��������µ�λͼ,���ر��ļ���
// Parameter: char * src
// Parameter: PBITMAP_IMAGE bmpImage
// Parameter: BYTE * imageArr
// Returns:   void
//************************************
void creatBmpByArr(char* src, PBITMAP_IMAGE bmpImage, BYTE* imageArr, WORD wType);

//************************************
// Method:    countingSort	��������
// Parameter: BYTE nums[]
// Parameter: int numsSize
// Parameter: int maxValue
// Returns:   BYTE*			��������õ�����
//************************************
BYTE*	countingSort(BYTE* nums, int numsSize, int maxValue);


//************************************
// Method:    getMedianValue ���ü������򣬷�����ֵ
// Parameter: int nums
// Parameter: int numsSize
// Parameter: int maxValue
// Returns:   int
//************************************
BYTE	getMedianValue(BYTE* nums, int numsSize, int maxValue);


//************************************
// Method:    extractBmpByBound		�����ĸ��߽磬��ȡͼƬ����Ӧλ�õ���ͼ
// Parameter: BYTE * bmpArr
// Parameter: LONG widht
// Parameter: LONG height
// Parameter: LONG bound[]
// Parameter: WORD wType			8��24
// Returns:   BYTE*					������ȡ����ͼ������
//************************************
BYTE* extractBmpByBound(const BYTE* bmpArr, LONG widht, LONG height, LONG bound[], WORD wType);

HSI*  RGB2HSI(const BYTE * imageArr24, LONG width, LONG height);

HSV* 	RGB2HSV(const BYTE* imageArr24, LONG width, LONG height);

// ��������:��һ���ṹԪ��(һ����3��3�Ĵ�С)ɨ��ͼ���е�ÿһ�����أ�
// �ýṹԪ���е�ÿһ���������串�ǵ����������롱�����������Ϊ0���������Ϊ0������Ϊ1
int		dilation(BYTE* imageArr8, int width, int height);
// ��ʴ
int		erosion(BYTE* imageArr8, int width, int height);

// ����destλͼ���ص�Ϊ255��ǰ���£�����ͼ��Ľ�������������dest��
int		intersection(const BYTE* sorce, BYTE* dest, LONG width, LONG height);


//ʹ��Rosenfeld�㷨��ϸ���㷨
void thinnerRosenfeld(BYTE *image, LONG width, LONG height);
void thinnerHilditch(void *image, unsigned long lx, unsigned long ly);
void thinnerPavlidis(void *image, unsigned long lx, unsigned long ly);

FILE*	openTextFile(char* filename, char* mode);

// ��ת��ֵͼ
void	reverseBinImg(BYTE* biImg, LONG width, LONG heigh);
#endif