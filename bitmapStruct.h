/* 位图文件件结构声明 */

// Bitmap格式有4部分组成： 文件头、图像描述、颜色表（在真彩色(24或32位)模式无颜色表）和图像数据区
#ifndef _BMP_HEADER_
#define _BMP_HEADER_

#define BF_TYPE_8 8
#define BF_TYPE_24 24
#define COUNT_24(width)	((4-(width*3)%4)%4)	//由于系统读取时按4字节的倍数读取，因此需要计算每行补0的位数
#define COUNT_8(width)	((4-(width)%4)%4)

typedef unsigned short	WORD;	//2*8=16
typedef unsigned long	DWORD;	//4*8=32
typedef long			LONG;	//4*8=32
typedef unsigned char	BYTE;	//1*8=8

 /*14 bytes BMP文件头数据结构含有BMP文件的类型、文件大小和位图起始位置等信息*/
#pragma pack(1)		//另结构体按1的长度进行
typedef struct tagBITMAP_FILE_HEADER
{
    WORD	bfType;			/*2,位图文件的类型，必须为BM(0-1字节)*/
    DWORD	bfSize;			/*4,位图文件的大小，以字节为单位(2-5字节)*/
    WORD	bfReserved1;	/*2,位图文件保留字，必须为0(6-7字节)*/
    WORD	bfReserved2;	/*2,位图文件保留字，必须为0（6-7字节）*/
    DWORD	bfOffBits;		/*4,实际位图数据的偏移字节数，即前三个部分长度之和(10-13字节) */
}BITMAP_FILE_HEADER, *PBITMAP_FILE_HEADER;
#pragma pack()

#pragma pack(1)
/*40 bytes BMP位图信息头数据用于说明位图的尺寸等信息。*/
typedef struct tagBITMAP_INFO_HEADER
{ 
    DWORD	biSize;			/*4,图像描述信息块所占用字节数(14-17字节) */
    LONG	biWidth;		/*4,位图的宽度，以像素为单位(18-21字节)*/
    LONG	biHeight;		/*4,位图的高度，以像素为单位(22-25字节) */
    WORD	biPlanes;		/*2,目标设备的级别，必须为1(26-27字节) */
    WORD	biBitCount;		/*2每个像素所需的位数，必须是1(双色),(28-29字节)
							4(16色)，8(256色)或24(真彩色)之一 */
    DWORD	biCompression;	/*4,位图压缩类型，必须是 0(不压缩),(30-33字节)
							1(BI_RLE8压缩类型)或2(BI_RLE4压缩类型)之一 */
    DWORD	biSizeImage;		/*4,图像数据区的大小，以字节为单位(34-37字节) */
    LONG	biXPelsPerMeter;	/*4,水平每米有多少像素，在设备无关位图（.DIB）中，每字节以00H填写(38-41字节) */
    LONG	biYPelsPerMeter;	/*4,垂直每米有多少像素，在设备无关位图（.DIB）中，每字节以00H填写(42-45字节) */
    DWORD	biClrUsed;			/*4,位图实际使用的颜色表中的颜色数,如值为0，表示所有颜色一样重要(46-49字节) */
    DWORD	biClrImportant;		/*4,位图显示过程中重要的颜色数(50-53字节) */
} BITMAP_INFO_HEADER,*PBITMAP_INFO_HEADER;
#pragma pack()

/*颜色表用于说明位图中的颜色，它有若干个表项，每一个表项是一个RGBQUAD类型的结构，定义一种颜色。
每4字节表示一种颜色，并以B（蓝色）、G（绿色）、R（红色）、alpha（像素的透明度值，一般不需要）*/

#pragma pack(1)
typedef struct tagRGB_QUAD
{ 
    BYTE	rgbBlue;		/*蓝色的亮度(值范围为0-255)*/
    BYTE	rgbGreen;		/*绿色的亮度(值范围为0-255) */
    BYTE	rgbRed;			/*红色的亮度(值范围为0-255)*/
    BYTE	rgbReserved;	/*保留，必须为0 */
}RGB_QUAD;
#pragma pack()
/*颜色表中RGBQUAD结构数据的个数有biBitCount来确定:
当biBitCount=1,4,8时，分别有2,16,256个表项，即有2，16，256种不同的颜色
当biBitCount=24时，没有颜色表项。
每种颜色模式的行字节数要用数据“00”补齐为4的整数倍
如：256色图像，图像宽为19，每行也要补充4-19%4=1个字节*/

/*位图信息头和文件头组成位图信息*/
typedef struct tagBITMAP_IMAGE
{ 
    BITMAP_FILE_HEADER fileHeader;	/*位图信息头*/
	BITMAP_INFO_HEADER infoHeader;	//此处不能用指针类型
	//RGB_QUAD bmiColors[1];			/*颜色表*/
}BITMAP_IMAGE, *PBITMAP_IMAGE;

#endif