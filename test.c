///*******************************
//*��������
//*RotateDIB2()
//*
//*����
//*  LPSTR lpDIB        -ָ��ԭDIB��ָ��
//* int iRotateAngle    -��ת�ĽǶ�
//*
//*����ֵ
//
//*HGLOBAL
//*
//*˵����
//*   �ú���������ͼ��Ϊ������תDIBͼ�󣬷���������DIB�ľ����
//*���øú������Զ�����ͼ������ʾ���е����ء������в���˫���β�ֵ�㷨���в�ֵ
//*
//***********************************/
//HGLOBAL WINAPI RotateDIB2(LPSTR lpDIB, int iRotateAngle)
//{
//	ԭͼ��ĸ߶ȺͿ��
//	LONG lWidth;
//	LONG lHeight;
//
//	��ת���ͼ��߶ȺͿ��
//	LONG lNewWidth;
//	LONG lNewHeight;
//
//	��ת��ͼ��Ŀ�ȣ�1NewWidth��������4�ı�����
//	LONG lNewLineBytes;
//	ָ��ԭͼ���ָ��
//	LPSTR lpDIBBits;
//
//	ָ��ԭ������ָ��
//	LPSTR lpSrc;
//
//	��ת����DIB���
//	HDIB hDIB;
//
//	ָ����תͼ���Ӧ������ָ��
//	LPSTR lpDst;
//
//	ָ����תͼ���ָ��
//	LPSTR lpNewDIB;
//	LPSTR lpNewDIBBits;
//
//	ָ��BITMAPINFO�ṹ��ָ��
//	LPBITMAPINFOHEADER lpbmi;
//
//	ָ��BITMAPCOREINFO�ṹ��ָ��
//	LPBITMAPCOREHEADER lpbmc;
//
//	ѭ������
//	LONG i;
//	LONG j;
//
//	������ԭDIB�е�����
//	FLOAT i0;
//	FLOAT j0;
//
//	��ת�Ƕ�
//	float fRotateAngle;
//
//	��ת�Ƕȵ����Һ�����
//	float fSina, fCosa;
//
//	ԭͼ�ĸ��ǵ�����
//	float fSrcX1, fSrcY1, fSrcX2, fSrcY2, fSrcX3, fSrcY3, fSrcX4, fSrcY4;
//
//	��ת���ĸ��ǵ�����
//	float fDstX1, fDstY1, fDstX2, fDstY2, fDstX3, fDstY3, fDstX4, fDstY4;
//
//	�����м䳣��
//	float f1, f2;
//
//	�ҵ�ԭDIBͼ��������ʼλ��
//	lpDIBBits = ::FindDIBBits(lpDIB);
//
//	��ȡͼ��Ŀ��
//	lWidth = ::DIBWidth(lpDIB);
//
//	��ȡͼ��ĸ߶�
//	lHeight = ::DIBHeight(lpDIB);
//
//	����ת�ǶȴӶ�ת��������
//	fRotateAngle = (float)RADIAN(iRotateAngle);
//
//	������ת�Ƕȵ�����
//	fSina = (float)sin((double)fRotateAngle);
//
//	������ת�Ƕȵ�����
//	fCosa = (float)cos((double)fRotateAngle);
//
//	����ԭͼ���ĸ��ǵ�����
//	fSrcX1 = (float)(-(lWidth - 1) / 2);
//	fSrcY1 = (float)((lHeight - 1) / 2);
//	fSrcX2 = (float)((lWidth - 1) / 2);
//	fSrcY2 = (float)((lHeight - 1) / 2);
//	fSrcX3 = (float)(-(lWidth - 1) / 2);
//	fSrcY3 = (float)(-(lHeight - 1) / 2);
//	fSrcX4 = (float)((lWidth - 1) / 2);
//	fSrcY4 = (float)(-(lHeight - 1) / 2);
//
//	������ͼ�ĸ��ǵ�����
//	fDstX1 = fCosa*fSrcX1 + fSina*fSrcY1;
//	fDstY1 = -fsina*fSrcX1 + fCosa*fSrcY1;
//	fDstX2 = fCosa*fSrcX2 + fSina*fSrcY2;
//	fDstY2 = -fsina*fSrcX2 + fCosa*fSrcY2;
//	fDstX3 = fCosa*fSrcX3 + fSina*fSrcY3;
//	fDstY3 = -fsina*fSrcX3 + fCosa*fSrcY3;
//	fDstX4 = fCosa*fSrcX4 + fSina*fSrcY4;
//	fDstY4 = -fsina*fSrcX4 + fCosa*fSrcY4;
//
//	������ת���ͼ��ʵ�ʿ��
//	lNewWidth = (LONG)(max(fabs(fDstX4 - fDstX1), fabs(fDstX3 - fDstX2)) + 0.5);
//	lNewLineBytes = WIDTHBYTES(lNewWidth * 8);
//
//	������ת��ͼ��ĸ߶�
//	lNewHeight = (LONG)(max(fabs(fDstY4 - fDstY1), fabs(fDstY3 - fDstY2)) + 0.5);
//
//	�������������������Ժ�ÿ�ζ�����
//	f1 = (float)(-0.5*(lNewWidth - 1)*fCosa - 0.5*(lNewHeight - 1)*fSina + 0.5*(lWidth - 1));
//	f2 = (float)(0.5*(lNewWidth - 1)*fSina - 0.5*(lNewHeight - 1)*fCosa + 0.5(lHeight - 1));
//
//	�����ڴ棬�Ա���DIB
//	hDIB = (hDIB)::GlobalAlloc(GHND, lNewLineBytes*lNewHeight + *(LPDWORD)lpDIB + ::paletteSize(lpDIB));
//
//	�ж��Ƿ�Ϊ�ڴ����ʧ��
//	if (hDIB == NULL)
//	{
//
//		�����ڴ�ʧ��
//		return NULL;
//	}
//
//	�����ڴ�
//	lpNewDIB = (char *)::GlobalLock((HGLOBAL)hDIB);
//
//	����DIB��Ϣͷ�͵�ɫ��
//	memcpy(lpNewDIB, lpDIB, *(LPDWORD)lpDIB + ::PaletteSize(lpDIB));
//
//	�ҵ���DIB������ʼλ��
//	lpNewDIBBits = ::FindDIBBits(lpNewDIB);
//
//	��ȡָ��
//	lpbmi = (LPBITMAPINFOHEADER)lpNewDIB;
//	lpbmc = (LPBITMAPCOREHEADER)lpNewDIB;
//
//	����DIB��ͼ��ĸ߶ȺͿ��
//	if (IS_WIN30_DIB(lpNewDIB))
//	{
//
//		����WIN 3.0 DIB
//		lpbim->biWidth = lNewWidth;
//		lpbmi->biHeight = lNewHeight;
//	}
//	else
//	{
//		����������ʽ��DIB
//		lpbmc->bcWidth = (unsigned short)lNewWidth;
//		lpbmc->bcHeight = (unsigned short)lNewHeight;
//	}
//	���ͼ��ÿ�н��в���
//	for (i = 0;i<lNewHeight;i++)
//	{
//		���ͼ��ÿ�н��в���
//		for (j = 0;j<lNewWidth;j++)
//		{
//			ע����DIB��i��,��j�и�������ָ��
//			ע��˴���Ⱥ͸߶���DIB�Ŀ�Ⱥ͸߶�
//			lpDst = (char *)lpNewDIBBits + lNewLineBytes*(lNewHeight - 1 - i) + j;
//
//			�����������ԭDIB�е�����
//			i0 = -((float)j)*fSina + ((float)i)*fCosa + f2;
//			j0 = ((float)j)*fCosa + ((float)i)*fSina + f1;
//
//			����˫���β�ֵ�㷨����������ֵ
//			*lpDst = Interpolation(lpDIBBits, lWidth, lHeight, j0, i0);
//
//		}
//	}
//
//	����
//	return hDIB;
//
//}
//
///*************************************************************************
//* �������ƣ�
//*   Interpolation()
//* ����:
//*   LPSTR lpDIBBits    - ָ��ԴDIBͼ��ָ��
//*   LONG  lWidth       - Դͼ���ȣ���������
//*   LONG  lHeight      - Դͼ��߶ȣ���������
//*   FLOAT x            - ��ֵԪ�ص�x����
//*   FLOAT y            - ��ֵԪ�ص�y����
//* ����ֵ:
//*   unsigned char      - ���ز�ֵ��������
//* ˵��:
//*   �ú�������˫���Բ�ֵ�㷨����������ֵ�����ڳ���ͼ��Χ�����أ�
//* ֱ�ӷ���255��
//************************************************************************/
//unsigned char CDibImage::Interpolation(LPSTR lpDIBBits, LONG lWidth,
//	LONG lHeight, FLOAT x, FLOAT y)
//{
//	 �ĸ����ٽ����ص�����(i1, j1), (i2, j1), (i1, j2), (i2, j2)  
//	LONG    i1, i2;
//	LONG    j1, j2;
//
//	unsigned char    f1, f2, f3, f4;    // �ĸ����ٽ�����ֵ      
//	unsigned char    f12, f34;        // ������ֵ�м�ֵ      
//
//									   ����һ��ֵ���������������С�ڸ�ֵʱ��Ϊ������ͬ  
//	FLOAT            EXP;
//
//	LONG lLineBytes;                // ͼ��ÿ�е��ֽ���  
//	lLineBytes = WIDTHBYTES(lWidth * 8);
//
//	EXP = (FLOAT) 0.0001;
//
//	 �����ĸ����ٽ����ص�����  
//	i1 = (LONG)x;
//	i2 = i1 + 1;
//	j1 = (LONG)y;
//	j2 = j1 + 1;
//
//	 ���ݲ�ͬ����ֱ���  
//	if ((x < 0) || (x > lWidth - 1) || (y < 0) || (y > lHeight - 1))
//	{
//		return 255;        // Ҫ����ĵ㲻��Դͼ��Χ�ڣ�ֱ�ӷ���255��  
//	}
//	else
//	{
//		if (fabs(x - lWidth + 1) <= EXP)
//		{
//			 Ҫ����ĵ���ͼ���ұ�Ե��  
//			if (fabs(y - lHeight + 1) <= EXP)
//			{
//				 Ҫ����ĵ�������ͼ�������½���һ�����أ�ֱ�ӷ��ظõ�����ֵ  
//				f1 = *((unsigned char *)lpDIBBits + lLineBytes *
//					(lHeight - 1 - j1) + i1);
//				return f1;
//			}
//			else
//			{
//				 ��ͼ���ұ�Ե���Ҳ������һ�㣬ֱ��һ�β�ֵ����  
//				f1 = *((unsigned char *)lpDIBBits + lLineBytes *
//					(lHeight - 1 - j1) + i1);
//				f3 = *((unsigned char *)lpDIBBits + lLineBytes *
//					(lHeight - 1 - j1) + i2);
//
//				 ���ز�ֵ���  
//				return ((unsigned char)(f1 + (y - j1) * (f3 - f1)));
//			}
//		}
//		else if (fabs(y - lHeight + 1) <= EXP)
//		{
//			 Ҫ����ĵ���ͼ���±�Ե���Ҳ������һ�㣬ֱ��һ�β�ֵ����  
//			f1 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j1) + i1);
//			f2 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j2) + i1);
//
//			 ���ز�ֵ���  
//			return ((unsigned char)(f1 + (x - i1) * (f2 - f1)));
//		}
//		else
//		{
//			 �����ĸ����ٽ�����ֵ  
//			f1 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j1) + i1);
//			f2 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j2) + i1);
//			f3 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j1) + i2);
//			f4 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j2) + i2);
//
//			 ��ֵ1  
//			f12 = (unsigned char)(f1 + (x - i1) * (f2 - f1));
//			 ��ֵ2  
//			f34 = (unsigned char)(f3 + (x - i1) * (f4 - f3));
//			 ��ֵ3  
//			return ((unsigned char)(f12 + (y - j1) * (f34 - f12)));
//		}
//	}
//}
