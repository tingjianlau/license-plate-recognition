///*******************************
//*函数名称
//*RotateDIB2()
//*
//*参数
//*  LPSTR lpDIB        -指向原DIB的指针
//* int iRotateAngle    -旋转的角度
//*
//*返回值
//
//*HGLOBAL
//*
//*说明：
//*   该函数用来以图象为中心旋转DIB图象，返回新生成DIB的句柄。
//*调用该函数会自动扩大图象以显示所有的像素。函数中采用双线形插值算法进行插值
//*
//***********************************/
//HGLOBAL WINAPI RotateDIB2(LPSTR lpDIB, int iRotateAngle)
//{
//	原图象的高度和宽度
//	LONG lWidth;
//	LONG lHeight;
//
//	旋转后的图象高度和宽度
//	LONG lNewWidth;
//	LONG lNewHeight;
//
//	旋转后图象的宽度（1NewWidth，必须是4的倍数）
//	LONG lNewLineBytes;
//	指向原图象的指针
//	LPSTR lpDIBBits;
//
//	指向原像数的指针
//	LPSTR lpSrc;
//
//	旋转后新DIB句柄
//	HDIB hDIB;
//
//	指向旋转图象对应像数的指针
//	LPSTR lpDst;
//
//	指向旋转图象的指针
//	LPSTR lpNewDIB;
//	LPSTR lpNewDIBBits;
//
//	指向BITMAPINFO结构的指针
//	LPBITMAPINFOHEADER lpbmi;
//
//	指向BITMAPCOREINFO结构的指针
//	LPBITMAPCOREHEADER lpbmc;
//
//	循环变量
//	LONG i;
//	LONG j;
//
//	像数在原DIB中的坐标
//	FLOAT i0;
//	FLOAT j0;
//
//	旋转角度
//	float fRotateAngle;
//
//	旋转角度的正弦和余弦
//	float fSina, fCosa;
//
//	原图四个角的坐标
//	float fSrcX1, fSrcY1, fSrcX2, fSrcY2, fSrcX3, fSrcY3, fSrcX4, fSrcY4;
//
//	旋转后四个角的坐标
//	float fDstX1, fDstY1, fDstX2, fDstY2, fDstX3, fDstY3, fDstX4, fDstY4;
//
//	两个中间常量
//	float f1, f2;
//
//	找到原DIB图象像数起始位置
//	lpDIBBits = ::FindDIBBits(lpDIB);
//
//	获取图象的宽度
//	lWidth = ::DIBWidth(lpDIB);
//
//	获取图象的高度
//	lHeight = ::DIBHeight(lpDIB);
//
//	将旋转角度从度转换到弧度
//	fRotateAngle = (float)RADIAN(iRotateAngle);
//
//	计算旋转角度的正弦
//	fSina = (float)sin((double)fRotateAngle);
//
//	计算旋转角度的余弦
//	fCosa = (float)cos((double)fRotateAngle);
//
//	计算原图的四个角的坐标
//	fSrcX1 = (float)(-(lWidth - 1) / 2);
//	fSrcY1 = (float)((lHeight - 1) / 2);
//	fSrcX2 = (float)((lWidth - 1) / 2);
//	fSrcY2 = (float)((lHeight - 1) / 2);
//	fSrcX3 = (float)(-(lWidth - 1) / 2);
//	fSrcY3 = (float)(-(lHeight - 1) / 2);
//	fSrcX4 = (float)((lWidth - 1) / 2);
//	fSrcY4 = (float)(-(lHeight - 1) / 2);
//
//	计算新图四个角的坐标
//	fDstX1 = fCosa*fSrcX1 + fSina*fSrcY1;
//	fDstY1 = -fsina*fSrcX1 + fCosa*fSrcY1;
//	fDstX2 = fCosa*fSrcX2 + fSina*fSrcY2;
//	fDstY2 = -fsina*fSrcX2 + fCosa*fSrcY2;
//	fDstX3 = fCosa*fSrcX3 + fSina*fSrcY3;
//	fDstY3 = -fsina*fSrcX3 + fCosa*fSrcY3;
//	fDstX4 = fCosa*fSrcX4 + fSina*fSrcY4;
//	fDstY4 = -fsina*fSrcX4 + fCosa*fSrcY4;
//
//	计算旋转后的图象实际宽度
//	lNewWidth = (LONG)(max(fabs(fDstX4 - fDstX1), fabs(fDstX3 - fDstX2)) + 0.5);
//	lNewLineBytes = WIDTHBYTES(lNewWidth * 8);
//
//	计算旋转后图象的高度
//	lNewHeight = (LONG)(max(fabs(fDstY4 - fDstY1), fabs(fDstY3 - fDstY2)) + 0.5);
//
//	两个常数，这样不用以后每次都计算
//	f1 = (float)(-0.5*(lNewWidth - 1)*fCosa - 0.5*(lNewHeight - 1)*fSina + 0.5*(lWidth - 1));
//	f2 = (float)(0.5*(lNewWidth - 1)*fSina - 0.5*(lNewHeight - 1)*fCosa + 0.5(lHeight - 1));
//
//	分配内存，以保存DIB
//	hDIB = (hDIB)::GlobalAlloc(GHND, lNewLineBytes*lNewHeight + *(LPDWORD)lpDIB + ::paletteSize(lpDIB));
//
//	判断是否为内存分配失败
//	if (hDIB == NULL)
//	{
//
//		分配内存失败
//		return NULL;
//	}
//
//	锁定内存
//	lpNewDIB = (char *)::GlobalLock((HGLOBAL)hDIB);
//
//	复制DIB信息头和调色板
//	memcpy(lpNewDIB, lpDIB, *(LPDWORD)lpDIB + ::PaletteSize(lpDIB));
//
//	找到新DIB像数起始位置
//	lpNewDIBBits = ::FindDIBBits(lpNewDIB);
//
//	获取指针
//	lpbmi = (LPBITMAPINFOHEADER)lpNewDIB;
//	lpbmc = (LPBITMAPCOREHEADER)lpNewDIB;
//
//	更新DIB中图象的高度和宽度
//	if (IS_WIN30_DIB(lpNewDIB))
//	{
//
//		对于WIN 3.0 DIB
//		lpbim->biWidth = lNewWidth;
//		lpbmi->biHeight = lNewHeight;
//	}
//	else
//	{
//		对于其他格式的DIB
//		lpbmc->bcWidth = (unsigned short)lNewWidth;
//		lpbmc->bcHeight = (unsigned short)lNewHeight;
//	}
//	针对图象每行进行操作
//	for (i = 0;i<lNewHeight;i++)
//	{
//		针对图象每列进行操作
//		for (j = 0;j<lNewWidth;j++)
//		{
//			注意新DIB第i行,第j列个像数的指针
//			注意此处宽度和高度是DIB的宽度和高度
//			lpDst = (char *)lpNewDIBBits + lNewLineBytes*(lNewHeight - 1 - i) + j;
//
//			计算该像数在原DIB中的坐标
//			i0 = -((float)j)*fSina + ((float)i)*fCosa + f2;
//			j0 = ((float)j)*fCosa + ((float)i)*fSina + f1;
//
//			利用双线形插值算法来估算像数值
//			*lpDst = Interpolation(lpDIBBits, lWidth, lHeight, j0, i0);
//
//		}
//	}
//
//	返回
//	return hDIB;
//
//}
//
///*************************************************************************
//* 函数名称：
//*   Interpolation()
//* 参数:
//*   LPSTR lpDIBBits    - 指向源DIB图像指针
//*   LONG  lWidth       - 源图像宽度（象素数）
//*   LONG  lHeight      - 源图像高度（象素数）
//*   FLOAT x            - 插值元素的x坐标
//*   FLOAT y            - 插值元素的y坐标
//* 返回值:
//*   unsigned char      - 返回插值计算结果。
//* 说明:
//*   该函数利用双线性插值算法来估算象素值。对于超出图像范围的象素，
//* 直接返回255。
//************************************************************************/
//unsigned char CDibImage::Interpolation(LPSTR lpDIBBits, LONG lWidth,
//	LONG lHeight, FLOAT x, FLOAT y)
//{
//	 四个最临近象素的坐标(i1, j1), (i2, j1), (i1, j2), (i2, j2)  
//	LONG    i1, i2;
//	LONG    j1, j2;
//
//	unsigned char    f1, f2, f3, f4;    // 四个最临近象素值      
//	unsigned char    f12, f34;        // 二个插值中间值      
//
//									   定义一个值，当象素坐标相差小于改值时认为坐标相同  
//	FLOAT            EXP;
//
//	LONG lLineBytes;                // 图像每行的字节数  
//	lLineBytes = WIDTHBYTES(lWidth * 8);
//
//	EXP = (FLOAT) 0.0001;
//
//	 计算四个最临近象素的坐标  
//	i1 = (LONG)x;
//	i2 = i1 + 1;
//	j1 = (LONG)y;
//	j2 = j1 + 1;
//
//	 根据不同情况分别处理  
//	if ((x < 0) || (x > lWidth - 1) || (y < 0) || (y > lHeight - 1))
//	{
//		return 255;        // 要计算的点不在源图范围内，直接返回255。  
//	}
//	else
//	{
//		if (fabs(x - lWidth + 1) <= EXP)
//		{
//			 要计算的点在图像右边缘上  
//			if (fabs(y - lHeight + 1) <= EXP)
//			{
//				 要计算的点正好是图像最右下角那一个象素，直接返回该点象素值  
//				f1 = *((unsigned char *)lpDIBBits + lLineBytes *
//					(lHeight - 1 - j1) + i1);
//				return f1;
//			}
//			else
//			{
//				 在图像右边缘上且不是最后一点，直接一次插值即可  
//				f1 = *((unsigned char *)lpDIBBits + lLineBytes *
//					(lHeight - 1 - j1) + i1);
//				f3 = *((unsigned char *)lpDIBBits + lLineBytes *
//					(lHeight - 1 - j1) + i2);
//
//				 返回插值结果  
//				return ((unsigned char)(f1 + (y - j1) * (f3 - f1)));
//			}
//		}
//		else if (fabs(y - lHeight + 1) <= EXP)
//		{
//			 要计算的点在图像下边缘上且不是最后一点，直接一次插值即可  
//			f1 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j1) + i1);
//			f2 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j2) + i1);
//
//			 返回插值结果  
//			return ((unsigned char)(f1 + (x - i1) * (f2 - f1)));
//		}
//		else
//		{
//			 计算四个最临近象素值  
//			f1 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j1) + i1);
//			f2 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j2) + i1);
//			f3 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j1) + i2);
//			f4 = *((unsigned char*)lpDIBBits + lLineBytes * (lHeight - 1 - j2) + i2);
//
//			 插值1  
//			f12 = (unsigned char)(f1 + (x - i1) * (f2 - f1));
//			 插值2  
//			f34 = (unsigned char)(f3 + (x - i1) * (f4 - f3));
//			 插值3  
//			return ((unsigned char)(f12 + (y - j1) * (f34 - f12)));
//		}
//	}
//}
