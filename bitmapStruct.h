/* λͼ�ļ����ṹ���� */

// Bitmap��ʽ��4������ɣ� �ļ�ͷ��ͼ����������ɫ�������ɫ(24��32λ)ģʽ����ɫ����ͼ��������
#ifndef _BMP_HEADER_
#define _BMP_HEADER_

#define BF_TYPE_8 8
#define BF_TYPE_24 24
#define COUNT_24(width)	((4-(width*3)%4)%4)	//����ϵͳ��ȡʱ��4�ֽڵı�����ȡ�������Ҫ����ÿ�в�0��λ��
#define COUNT_8(width)	((4-(width)%4)%4)

typedef unsigned short	WORD;	//2*8=16
typedef unsigned long	DWORD;	//4*8=32
typedef long			LONG;	//4*8=32
typedef unsigned char	BYTE;	//1*8=8

 /*14 bytes BMP�ļ�ͷ���ݽṹ����BMP�ļ������͡��ļ���С��λͼ��ʼλ�õ���Ϣ*/
#pragma pack(1)		//��ṹ�尴1�ĳ��Ƚ���
typedef struct tagBITMAP_FILE_HEADER
{
    WORD	bfType;			/*2,λͼ�ļ������ͣ�����ΪBM(0-1�ֽ�)*/
    DWORD	bfSize;			/*4,λͼ�ļ��Ĵ�С�����ֽ�Ϊ��λ(2-5�ֽ�)*/
    WORD	bfReserved1;	/*2,λͼ�ļ������֣�����Ϊ0(6-7�ֽ�)*/
    WORD	bfReserved2;	/*2,λͼ�ļ������֣�����Ϊ0��6-7�ֽڣ�*/
    DWORD	bfOffBits;		/*4,ʵ��λͼ���ݵ�ƫ���ֽ�������ǰ�������ֳ���֮��(10-13�ֽ�) */
}BITMAP_FILE_HEADER, *PBITMAP_FILE_HEADER;
#pragma pack()

#pragma pack(1)
/*40 bytes BMPλͼ��Ϣͷ��������˵��λͼ�ĳߴ����Ϣ��*/
typedef struct tagBITMAP_INFO_HEADER
{ 
    DWORD	biSize;			/*4,ͼ��������Ϣ����ռ���ֽ���(14-17�ֽ�) */
    LONG	biWidth;		/*4,λͼ�Ŀ�ȣ�������Ϊ��λ(18-21�ֽ�)*/
    LONG	biHeight;		/*4,λͼ�ĸ߶ȣ�������Ϊ��λ(22-25�ֽ�) */
    WORD	biPlanes;		/*2,Ŀ���豸�ļ��𣬱���Ϊ1(26-27�ֽ�) */
    WORD	biBitCount;		/*2ÿ�����������λ����������1(˫ɫ),(28-29�ֽ�)
							4(16ɫ)��8(256ɫ)��24(���ɫ)֮һ */
    DWORD	biCompression;	/*4,λͼѹ�����ͣ������� 0(��ѹ��),(30-33�ֽ�)
							1(BI_RLE8ѹ������)��2(BI_RLE4ѹ������)֮һ */
    DWORD	biSizeImage;		/*4,ͼ���������Ĵ�С�����ֽ�Ϊ��λ(34-37�ֽ�) */
    LONG	biXPelsPerMeter;	/*4,ˮƽÿ���ж������أ����豸�޹�λͼ��.DIB���У�ÿ�ֽ���00H��д(38-41�ֽ�) */
    LONG	biYPelsPerMeter;	/*4,��ֱÿ���ж������أ����豸�޹�λͼ��.DIB���У�ÿ�ֽ���00H��д(42-45�ֽ�) */
    DWORD	biClrUsed;			/*4,λͼʵ��ʹ�õ���ɫ���е���ɫ��,��ֵΪ0����ʾ������ɫһ����Ҫ(46-49�ֽ�) */
    DWORD	biClrImportant;		/*4,λͼ��ʾ��������Ҫ����ɫ��(50-53�ֽ�) */
} BITMAP_INFO_HEADER,*PBITMAP_INFO_HEADER;
#pragma pack()

/*��ɫ������˵��λͼ�е���ɫ���������ɸ����ÿһ��������һ��RGBQUAD���͵Ľṹ������һ����ɫ��
ÿ4�ֽڱ�ʾһ����ɫ������B����ɫ����G����ɫ����R����ɫ����alpha�����ص�͸����ֵ��һ�㲻��Ҫ��*/

#pragma pack(1)
typedef struct tagRGB_QUAD
{ 
    BYTE	rgbBlue;		/*��ɫ������(ֵ��ΧΪ0-255)*/
    BYTE	rgbGreen;		/*��ɫ������(ֵ��ΧΪ0-255) */
    BYTE	rgbRed;			/*��ɫ������(ֵ��ΧΪ0-255)*/
    BYTE	rgbReserved;	/*����������Ϊ0 */
}RGB_QUAD;
#pragma pack()
/*��ɫ����RGBQUAD�ṹ���ݵĸ�����biBitCount��ȷ��:
��biBitCount=1,4,8ʱ���ֱ���2,16,256���������2��16��256�ֲ�ͬ����ɫ
��biBitCount=24ʱ��û����ɫ���
ÿ����ɫģʽ�����ֽ���Ҫ�����ݡ�00������Ϊ4��������
�磺256ɫͼ��ͼ���Ϊ19��ÿ��ҲҪ����4-19%4=1���ֽ�*/

/*λͼ��Ϣͷ���ļ�ͷ���λͼ��Ϣ*/
typedef struct tagBITMAP_IMAGE
{ 
    BITMAP_FILE_HEADER fileHeader;	/*λͼ��Ϣͷ*/
	BITMAP_INFO_HEADER infoHeader;	//�˴�������ָ������
	//RGB_QUAD bmiColors[1];			/*��ɫ��*/
}BITMAP_IMAGE, *PBITMAP_IMAGE;

#endif