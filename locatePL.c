#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <memory.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"
#include "bitmapStruct.h"
#include "bmpManage.h"
#include "preprocess.h"



typedef struct bound{
	int	up;
	int down;
	int left;
	int right;
}BOUNDS;



int updateHorBounds(const BYTE* imageArr24, BYTE* locateImageArr,LONG width, LONG height, LONG bound[], int PLColor);
int isMatchPLHorClr(int PLColor, BYTE curClr);
int isMatchPLVerClr(int PLColor, BYTE preClr, BYTE curClr, LONG* bound, LONG PLClrCnt);
int updateVerBounds(const BYTE* imageArr24, BYTE* locateImageArr,LONG width, LONG height, LONG bound[], int PLColor);
//************************************
// Method:    secondLocate			�ڶ��ξ�ϸ��λ,����wType��ֵ������Ӧ����ͼ
// Parameter: BYTE * imageArr24		24λ��ɫλͼ����
// Parameter: BYTE * imageArr8		8λ�Ҷ�λͼ����
// Parameter: BYTE * locateImageArr	������һ�δֶ�λ��õ���24λ��ɫλͼ����
// Parameter: LONG width
// Parameter: LONG height
// Parameter: LONG bound[]			������һ�δֶ�λ��õ��߽磬������
// Returns:   BYTE*					��ȡ�Ĵֶ�λ��ͼ����
//************************************
BYTE* secondLocate(const BYTE* imageArr24, const BYTE* imageArr8, BYTE* locateImageArr, LONG width, LONG height, LONG bound[], WORD wType);

//************************************
// Method:    firstLocate		��һ�δֶ�λ
// Parameter: BYTE * imageArr8	8ɫ�Ҷ�λͼ����
// Parameter: const BYTE * imageArr24  24λ��ɫλͼ����
// Parameter: LONG width		
// Parameter: LONG height
// Parameter: LONG bound[]		�ֶ�λ��õ����ĸ��߽磬�ֱ�����������
// Parameter: WORD wType		ָ����ȡ�Ĵֶ�λ��ͼ��8λ�Ҷ�ͼor24λ��ɫλͼ
// Returns:   BYTE*				��ȡ�Ĵֶ�λ��ͼ����
//************************************
BYTE*	firstLocate(BYTE* imageArr8, const BYTE* imageArr24,LONG width, LONG height, LONG	bound[], WORD wType);
//************************************
// Method:    checkColor   �Դ������ͨ����ɫֵ���ж�����һ����ɫ
// Parameter: BYTE b
// Parameter: BYTE g
// Parameter: BYTE r
// Returns:   int
//************************************
int checkColor(double b, double g, double r);
//************************************
// Method:    getHorBound		ˮƽ��λ
// Parameter: BYTE * imageArr
// Parameter: LONG width
// Parameter: LONG height
// Parameter: LONG bound[]		�ĸ��߽磬�ֱ������������
// Returns:   LONG*	
//************************************
int		getPLColor(const BYTE* imageArr, LONG width, LONG heigh);


int		locatePL(char* srcFile, char* destFile){
	FILE	*fpSrc = NULL,
			*fpDest = NULL;
	LONG	i, j, k, temp=0;
	BYTE	*bmpArr8, *bmpArr24, *firstLocateBmpArr, *secLocateBmpArr,*tempArr, *rotatePreBmpArr;
	LONG	origWidht, origHeight, locateWidth, locateHeight;
	BITMAP_IMAGE bmpImage;
	LONG	rotateWidth, rotateHeight;
	const WORD	wTpye = BF_TYPE_24;		//ָ����ȡ�Ķ�����8λ�ҶȻ���24λ��ɫ��ͼ		
	LONG	bound[4] = {0};			// �ĸ��߽磬�ֱ������������
	HSV*	myHSV = NULL;

	memset(&bmpImage, 0, sizeof(BITMAP_IMAGE));

	fpSrc = loadImage(srcFile, &bmpImage);
	bmpArr24 = creatImageArr(fpSrc, &bmpImage);
	origWidht = bmpImage.infoHeader.biWidth;
	origHeight = bmpImage.infoHeader.biHeight;

	//Ԥ����
	bmpArr8 = locatePre(bmpArr24, origWidht, origHeight);

	//medianFilter(bmpArr, curWidht, curHeight);

	//�ֶ�λ
	firstLocateBmpArr = firstLocate(bmpArr8, bmpArr24, origWidht, origHeight, bound, BF_TYPE_24);

	if(checkBounds(bound) == 0){
		//printErrors("�ֶ�λʧ��");
		return 1;
	}

	//����λ
	secLocateBmpArr = secondLocate(bmpArr24, bmpArr8, firstLocateBmpArr, origWidht, origHeight, bound, wTpye);
	if(checkBounds(bound) == 0 || secLocateBmpArr==NULL){
		//printErrors("��λʧ��");
		return 1;
	}
	//printf("update success\n");
	//printf("%d %d \n", bound[UP], bound[DOWN]);
	//secLocateBmpArr = rotateRGB(secLocateBmpArr, -2, bound[RIGHT]-bound[LEFT]+1,bound[UP]-bound[DOWN]+1,&rotateWidth,&rotateHeight);

	bmpImage.infoHeader.biHeight = bound[UP]-bound[DOWN]+1;
	bmpImage.infoHeader.biWidth = bound[RIGHT]-bound[LEFT]+1;
	locateWidth = bmpImage.infoHeader.biWidth;
	locateHeight = bmpImage.infoHeader.biHeight;
	/*bmpImage.infoHeader.biHeight= rotateHeight;
	bmpImage.infoHeader.biWidth = rotateWidth;*/

	//rotatePreBmpArr = rotatePre(secLocateBmpArr, bmpImage.infoHeader.biWidth, bmpImage.infoHeader.biHeight);
	//myHSI = RGB2HSI(secLocateBmpArr, bmpImage.infoHeader.biWidth, bmpImage.infoHeader.biHeight, myHSI);
	
	
	
	creatBmpByArr(destFile, &bmpImage, secLocateBmpArr, 24);
	
	printSuccess("locatePL");
	free(bmpArr8);
	free(bmpArr24);
	free(firstLocateBmpArr);
	return 1;
}

int		checkRunLenRatio(const BYTE* imageArr, LONG height, LONG width, LONG curLine){
	LONG*	runLength = (LONG*)calloc(width/2+1, sizeof(LONG));	//��¼run length���룬���δ���255��0��Ӧ��length
	/*const float WHITE_RATIO_MIN = 0.1;
	const float WHITE_RATIO_MAX = 3.0;
	const float BLACK_RATIO_MIN = 0.1;
	const float BLACK_RATIO_MAX = 4.0;*/
	const double WHITE_RATIO_MIN = 0.3;
	const double WHITE_RATIO_MAX =3;
	const double BLACK_RATIO_MIN = 0.4;
	const double BLACK_RATIO_MAX = 4;
	
	const int WHITE_RUN_MIN = 12;
	const int WHITE_RUN_MAX = 30;
	const int BLACK_RUN_MIN = 12;
	const int BLACK_RUN_MAX = 30;
	int		iWCnt=0, iBCnt=0;		// ����ڰ����Ͱ׺���run length�ĸ���
	double curRatio = 0.0;
	int	i=1, cnt=0;
	int	flag = 1;
	//printf("%d ", curLine);
	/*for (i=0; i<width; ++i){
	printf("%d ", imageArr[curLine*width+i]);
	}*/
	for (i=0; i<width;){
		if (flag && imageArr[curLine*width] == 0){//����¼�տ�ʼ�����ɸ�0���ص�
			while(imageArr[curLine*width+(i++)+1]==0);
			flag = 0;
		}
		while(imageArr[curLine*width+i] == 255 && i<width){
			++runLength[cnt];
			++i;
		}
		++cnt;
		while(imageArr[curLine*width+i] == 0 && i<width){
			++runLength[cnt];
			++i;
		}
		++cnt;
	}
	/*printf("\n %d \n", cnt);
	for (i=0; i<cnt; ++i){
		printf("%d ", runLength[i]);
	}*/
	for (i=2; i<cnt; i=i+2){// �жϺ��г̱��Ƿ�����Ҫ��
		curRatio = (float)runLength[i]/runLength[i-2];	// 
		
		if (curRatio>=BLACK_RATIO_MIN && curRatio<=BLACK_RATIO_MAX){
			
			++iBCnt;
			//printf("/n=%d= ", iBCnt);
			//return 0;
		}else{
			if (iBCnt>=BLACK_RUN_MIN && iBCnt<=BLACK_RUN_MAX){
				return 1;
			}else{
				//printf("�� %lf %d %d %d \n",curRatio, i, runLength[i], runLength[i-2]);
				iBCnt=0;
			}
		}
	}
	if (iBCnt>=BLACK_RUN_MIN && iBCnt<=BLACK_RUN_MAX){
		return 1;
	}
	for (i=3; i<cnt; i=i+2){// �жϰ��г̱��Ƿ�����Ҫ��
		curRatio = (float)runLength[i]/runLength[i-2];	// 
		//printf("%lf ", curRatio);
		if (curRatio>=WHITE_RATIO_MIN && curRatio<=WHITE_RATIO_MAX){
			//printf("�� %lf %d %d %d \n",curRatio, i, runLength[i], runLength[i-2]);
			++iWCnt;
			//printf("=%d= ", iWCnt);
			//return 0;
		}else{
			if (iWCnt>=WHITE_RUN_MIN && iWCnt<=WHITE_RUN_MAX){
				return 1;
			}else{
				iWCnt=0;
			}
		}
	}
	if (iWCnt>=WHITE_RUN_MIN && iWCnt<=WHITE_RUN_MAX){
		return 1;
	}
	
	return 0;
}

LONG*	getHorBound(const BYTE* imageArr, LONG width, LONG height, LONG bound[]){
	int		i, j, k, m, n,q=0, tempCnt=0;
	//const BYTE	JUMP_SUM_MIN = 12, JUMP_SUM_MAX = 30;	//�ڰ�����ĸ���
	const BYTE	JUMP_SUM_MIN = 12, JUMP_SUM_MAX = 3000;	//�ڰ�����ĸ���
	//const BYTE	JUMP_COUNTER_MIN = 6, JUMP_COUNTER_MAX = 18;	//����涨�����ĸ���
	const BYTE	JUMP_COUNTER_MIN = 2, JUMP_COUNTER_MAX = 18;	//����涨�����ĸ���
	//const BYTE	JUMP_DIFF_MIN = 4, JUMP_DIFF_MAX = 25;	//�������κڰ����ľ����
	const BYTE	JUMP_DIFF_MIN = 4, JUMP_DIFF_MAX = 25;	//�������κڰ����ľ����
	const LONG	SPEC_LINE_MIN = 15, SPEC_LINE_MAX = 90;	//��������Ҫ��Ķ̼�������е����½�
	LONG*	iJumpSum = (LONG*)calloc(height, sizeof(LONG));	// ��¼��j�еĺڰ��������
	LONG*	iJumpLocation = (LONG*)calloc(width/2+1, sizeof(LONG));	//��¼��ǰ�з����ڰ������λ��
	LONG*	iJumpDiff = (LONG*)calloc(width/2+1, sizeof(LONG));		//��¼���κڰ�����ļ��
	LONG*	tempPos = (LONG*)calloc(height, sizeof(LONG));
	//LONG*	iJumpCouter = (LONG*)calloc(width/2+1, sizeof(LONG));		//��¼����涨�����ĸ���
	LONG	iJumpCouter = 0;
	LONG	iDown = -1, iUp=-1;
	//LONG*	iJumpLocation = (LONG*)calloc(width)*height, sizeof(LONG));
	int flag = 1;
	for (j=0; j<height; ++j){
		for (i=1, k=0; i<width; ++i){
			if (imageArr[j*width+i]-imageArr[j*width+i-1]==255){
				++iJumpSum[j];
				iJumpLocation[k++] = i-1;
			}
		}
		if (iJumpSum[j]>=JUMP_SUM_MIN && iJumpSum[j]<=JUMP_SUM_MAX){
			iJumpCouter = 0;
			++tempCnt;
			for (m=0; m<k-1; ++m){//�����������κڰ�����ľ����
				iJumpDiff[m] = iJumpLocation[m+1] - iJumpLocation[m];
			}
			
			for (n=0; n<k-1; ++n){
				if (iJumpDiff[n]>JUMP_DIFF_MIN && iJumpDiff[n]<JUMP_DIFF_MAX){
					++iJumpCouter;
				}else{
					if (iJumpCouter>=JUMP_COUNTER_MIN && iJumpCouter<=JUMP_COUNTER_MAX){
						if(checkRunLenRatio(imageArr, height, width, j)==1){
							tempPos[q++]=j;
						}
						break;
					} else{
						iJumpCouter = 0;
					}
				}
			}
		} 
	}
	tempCnt = 0;
	iDown = tempPos[0];
	for (i=0; i<q-1; i++){
		if (tempPos[i+1]-tempPos[i]<=3){
			++tempCnt;
		}else{
			if (tempCnt>=SPEC_LINE_MIN && tempCnt<=SPEC_LINE_MAX){
				iUp = tempPos[i];
				break;
			}else{
				iDown = tempPos[i+1];
				tempCnt=0;
			}
		}
		
	}
	if (i<=q && tempCnt>=SPEC_LINE_MIN && tempCnt<=SPEC_LINE_MAX){
		iUp = tempPos[i];
	}
	bound[0] = iUp;
	bound[1] = iDown;
	if (iUp <= iDown){
		printf("ˮƽ��λʧ��");
	}else{
		printSuccess("getHorPos");
	}
	free(iJumpSum);
	free(iJumpLocation);
	free(iJumpDiff);
	free(tempPos);
	
	return NULL;
}

//LONG*	getVerPos(BYTE* imageArr, LONG width, LONG height, LONG down, LONG up, LONG* left, LONG* right){
LONG*	getVerBound(const BYTE* imageArr, LONG width, LONG height, LONG bound[]){
	double	RATION = 3.5;
	LONG	iHeight = bound[0]-bound[1]+1;
	LONG	iWidth	= 0;
	LONG	iPerWidth = iWidth/7;
	int	STEP = 5;
	LONG	i, j, k;
	int		whiteCnt[7] = {0};
	double		totalPerBoxPixs = 0.0;
	const	double	WHITE_RATIO_MIN = 0.2;
	const	double	WHITE_RATIO_MAX =0.8;
	int		flag = 1;

	while(1){
		iWidth	= (LONG)iHeight*RATION;
		totalPerBoxPixs = iHeight*iWidth/7;
		iPerWidth = iWidth/7+1;
		for (k=0; k<width-5; k+=STEP){// ÿ����ǰ�ƶ�5������
			for (j=bound[1]; j<=bound[0]; ++j){//���±߽�
				for (i=k; i<iWidth+k; ++i){//���ƿ�ȷ�Χ�ڽ��м��
					if (imageArr[j*width+i]==255){ //ͳ��7��С�����еİ׵����
						++whiteCnt[(i-k)/iPerWidth];
					}
				}
			}
			for (j=0; j<7; ++j){
				if (whiteCnt[j]/totalPerBoxPixs<WHITE_RATIO_MIN || whiteCnt[j]/totalPerBoxPixs>WHITE_RATIO_MAX){
					flag = 0;
					break;
				}
			}
			if (flag == 1){		// �ҵ���ֱ�߽�
				bound[2] = k;
				bound[3] = k+iWidth;
				/*for (j=0; j<7; ++j){
					printf("%d %lf ", whiteCnt[j], whiteCnt[j]/totalPerBoxPixs);
				}*/
				printSuccess("getVerPos");
				return 0;
			}
			for (j=0; j<7; ++j){
				whiteCnt[j] = 0;
			}
			flag = 1;
		}
		RATION += 0.5;		// ���ڿ��
		if (RATION>=5){
			break;
		}
	}
	printf("��ֱ��λʧ��");
	return 0;
}

BYTE*	locatePre(const BYTE* imageArr, LONG width, LONG height){
	BYTE*	bmpArr;
	bmpArr = rgbToGray(imageArr, width, height);
	medianFilter(bmpArr, width, height);

	sobleSideEnhance(bmpArr, width, height, 2);

	binarization(bmpArr, width, height);

	//removeNoise(bmpArr, width, height);

	return bmpArr;
}

BYTE*	rotatePre(const BYTE* imageArr, LONG width, LONG height) {
	BYTE*	bmpArr;
	bmpArr = rgbToGray(imageArr, width, height);
	medianFilter(bmpArr, width, height);

	//sobleSideEnhance(bmpArr, width, height, 2);

	binarization(bmpArr, width, height);

	//removeNoise(bmpArr, width, height);

	return bmpArr;
}

BYTE*	firstLocate(BYTE* imageArr8, const BYTE* imageArr24, LONG width, LONG height, LONG	bound[], WORD wType) {
	BYTE*	locateBmpArr;
	getHorBound(imageArr8, width, height, bound);
	getVerBound(imageArr8, width, height, bound);

	if(checkBounds(bound) == 0){
		printErrors("�ֶ�λʧ��");
		return NULL;
	}

	//drawBound(bmpArr, curWidht, curHeight, bound);

	// �ӻҶ�ͼ����ȡ��ͼ
	return (wType==8) ? extractBmpByBound(imageArr8, width, height, bound,BF_TYPE_8) 
					   : extractBmpByBound(imageArr24, width, height, bound,BF_TYPE_24);
	// ��24ɫλͼ����ȡ��ͼ

}

BYTE*	secondLocate(const BYTE* imageArr24, const BYTE* imageArr8, BYTE* locateImageArr, LONG width, LONG height, LONG bound[], WORD wType){
	int		PLColor;
	LONG locateWidth = bound[UP]-bound[DOWN]+1;
	LONG locateHeight = bound[RIGHT]-bound[LEFT]+1;
	PLColor = getPLColor(locateImageArr, locateWidth, locateHeight);
	printf("\n��ǰ������ɫΪ: %d \n",PLColor);

	if(PLColor == 4){
		printErrors("��λ�ɹ��������ǳ�����������");
		return NULL;
	}

	
	updateVerBounds(imageArr24, locateImageArr, width, height, bound, PLColor);
	updateHorBounds(imageArr24, locateImageArr, width, height, bound, PLColor);
	updateVerBounds(imageArr24, locateImageArr, width, height, bound, PLColor); // ��һ�θ������±߽磬��Ҫ��Ϊ�˳�������б�����

	// �ӻҶ�ͼ����ȡ��ͼ
	return (wType == 8) ? extractBmpByBound(imageArr8, width, height, bound, BF_TYPE_8)
							: extractBmpByBound(imageArr24, width, height, bound, BF_TYPE_24);
	// ��24ɫλͼ����ȡ��ͼ
	
}

int		updateHorBounds(const BYTE* imageArr24, BYTE* locateImageArr,LONG width, LONG height, LONG	bound[], int PLColor){
	int		i, j, k, flag=0, flag1=0, flag2=0;
	LONG	newLeft, newRight;
	BYTE	curClr, curClr2,midClr;
	LONG	tmpIdx, tmpIdx2, PLClrCnt=0; // ���Ʊ�����ɫ���ص�ĸ���
	LONG	maxScaleBound1, maxScaleBound2; // ���Ŀ���չˮƽ�߽��С����ֹ���ڱ��ո˵�Ӱ�쵼��ˮƽ�߽����
	const	int BoxWidth = (bound[UP]-bound[DOWN]+1)*3/7; // ������̽������ַ��Ŀ��
	newLeft = (bound[LEFT]+bound[RIGHT])/2;
	newRight = newLeft;
	//maxScaleBound1 = (bound[UP]-bound[DOWN]+1)*3 + newRight;
	//maxScaleBound2 = newLeft-(bound[UP]-bound[DOWN]+1)*3;
	//for (j=newRight; j<width; ++j){
	//	for (i=bound[DOWN]; i<=bound[UP]; ++i){
	//		tmpIdx = 3*(i*width+j);
	//		curClr = checkColor(imageArr24[tmpIdx], imageArr24[tmpIdx+1],imageArr24[tmpIdx+2]);
	//		flag = isMatchPLHorClr(PLColor, curClr);
	//		if (flag == 1){//ֻҪ�ڴ�ֱ��������Ӧ����ɫ���㣬����Ϊ�Ǳ߽���
	//			++newRight;
	//			break;
	//		}
	//	}
	//	if (flag==1){
	//		flag = 0;
	//	}else{
	//		break;
	//	}
	//}

	for (j=newRight+BoxWidth; j<width-BoxWidth; j+=BoxWidth){
		for (i=bound[DOWN]; i<=bound[UP]; ++i){
			for (k=j; k>j-BoxWidth; --k){
				tmpIdx = 3*(i*width+k);
				curClr = checkColor(imageArr24[tmpIdx], imageArr24[tmpIdx+1],imageArr24[tmpIdx+2]);
				flag = isMatchPLHorClr(PLColor, curClr);
				if (flag1==0 && flag ==1){
					flag1 = 1;
				}else if(flag2==0 && flag == 2){
					flag2 = 1;
				}
			}
			/*tmpIdx = 3*(i*width+j);
			curClr = checkColor(imageArr24[tmpIdx], imageArr24[tmpIdx+1],imageArr24[tmpIdx+2]);
			flag = isMatchPLHorClr(PLColor, curClr);*/
			if (flag1 == 1 && flag2==1){//ֻҪ�ڴ�ֱ��������Ӧ����ɫ���㣬����Ϊ�Ǳ߽���
				newRight += BoxWidth;
				break;
			}
		}
		if (flag1 == 1 && flag2==1){
			flag = 0;
			flag1 = 0;
			flag2 = 0;
		}else{
			break;
		}
	}
	for (j=newLeft-BoxWidth; j>=0; j-=BoxWidth){
		for (i=bound[DOWN]; i<=bound[UP]; ++i){
			for (k=j; k<j+BoxWidth; ++k){
				tmpIdx = 3*(i*width+k);
				curClr = checkColor(imageArr24[tmpIdx], imageArr24[tmpIdx+1],imageArr24[tmpIdx+2]);
				flag = isMatchPLHorClr(PLColor, curClr);
				if (flag1==0 && flag ==1){
					flag1 = 1;
				}else if(flag2==0 && flag == 2){
					flag2 = 1;
				}
			}
			if (flag1 == 1 && flag2==1){//ֻҪ�ڴ�ֱ��������Ӧ����ɫ���㣬����Ϊ�Ǳ߽���
				newLeft -= BoxWidth;
				break;
			}
		}
		if (flag1 == 1 && flag2==1){
			flag = 0;
			flag1 = 0;
			flag2 = 0;
		}else{
			break;
		}
	}

	//for (j=newLeft-1; j>=0; --j){
	//	for (i=bound[DOWN]; i<=bound[UP]; ++i){
	//		tmpIdx = 3*(i*width+j);
	//		curClr = checkColor(imageArr24[tmpIdx], imageArr24[tmpIdx+1],imageArr24[tmpIdx+2]);
	//		flag = isMatchPLHorClr(PLColor, curClr);
	//		if (flag == 1){//ֻҪ�ڴ�ֱ��������Ӧ����ɫ���㣬����Ϊ�Ǳ߽���
	//			--newLeft;
	//			break;
	//		}
	//	}
	//	if (flag==1){
	//		flag = 0;
	//	}else{
	//		break;
	//	}
	//}
	bound[LEFT] = newLeft;
	bound[RIGHT] = newRight;
	return 0;
}

int		updateVerBounds(const BYTE* imageArr24, BYTE* locateImageArr,LONG width, LONG height, LONG	bound[], int PLColor){
	int		i, j, flag=0;
	LONG	newUp, newDown;
	BYTE	curClr, preClr,midClr;
	LONG	tmpIdx, tmpIdx2, PLClrCnt=0; // ���Ʊ�����ɫ���ص�ĸ���
	
	newUp = (bound[UP]+bound[DOWN])/2;
	newDown = newUp;

	//printf("%d %d %d \n", bound[UP], bound[DOWN], newUp);
	// �����ϱ߽�
	for (i=newUp; i<height; ++i){
		tmpIdx2 = 3*(i*width+bound[LEFT]);
		preClr = checkColor(imageArr24[tmpIdx2], imageArr24[tmpIdx2+1],imageArr24[tmpIdx2+2]);
		midClr = checkColor(imageArr24[tmpIdx2+3], imageArr24[tmpIdx2+4],imageArr24[tmpIdx2+5]);
		for (j=bound[LEFT]+2; j<=bound[RIGHT]; ++j){
			tmpIdx = 3*(i*width+j);
			curClr = checkColor(imageArr24[tmpIdx], imageArr24[tmpIdx+1],imageArr24[tmpIdx+2]);
			if (curClr == PLColor){ // ͳ�Ƴ��Ʊ�����ɫ���ص�ĸ���������ȥ��������˿��Ӱ��
				++PLClrCnt;
			}
			//preClr = checkColor(imageArr24[tmpIdx+3], imageArr24[tmpIdx+4],imageArr24[tmpIdx+5]);
			//printf("%d ", curClr);
			flag = isMatchPLVerClr(PLColor, preClr, curClr, bound, PLClrCnt);
			if (flag == 1){//������Ӧ��ɫ�����䣬������ǰ���ǳ�������
				++newUp;
				//printf("\n%d %d %d %d %d \n", i, newUp, preClr, curClr, j-bound[LEFT]);
				break;
			}else{
				preClr = midClr;		// ʹ���������1����ֹ���ڱ�Եģ�������¼�����
				midClr = curClr;
			}
		}
		PLClrCnt = 0;
		if (flag == 1){ // ������Ӧ��ɫ�����䣬���³�ʼ����־
			flag = 0;
		}else{	// �����ڴ�����Ӧ��ɫ�����䣬�����ڸ��±߽磬�˳�
			break;
		}
		//printf("\n");
	}
	// �����±߽�
	for (i=newDown-1; i>=0; --i){
		tmpIdx2 = 3*(i*width+bound[LEFT]);
		preClr = checkColor(imageArr24[tmpIdx2], imageArr24[tmpIdx2+1],imageArr24[tmpIdx2+2]);
		midClr = checkColor(imageArr24[tmpIdx2+3], imageArr24[tmpIdx2+4],imageArr24[tmpIdx2+5]);
		for (j=bound[LEFT]+2; j<=bound[RIGHT]; ++j){
			tmpIdx = 3*(i*width+j);
			curClr = checkColor(imageArr24[tmpIdx], imageArr24[tmpIdx+1],imageArr24[tmpIdx+2]);
			if (curClr == PLColor){ // ͳ�������ĳ��Ʊ�����ɫ���ص�ĸ���������ȥ��������˿��Ӱ��
				++PLClrCnt;
			}
			//preClr = checkColor(imageArr24[tmpIdx+3], imageArr24[tmpIdx+4],imageArr24[tmpIdx+5]);
			//printf("%d ", curClr);
			flag = isMatchPLVerClr(PLColor, preClr, curClr, bound, PLClrCnt);
			if (flag == 1){//������Ӧ��ɫ�����䣬������ǰ���ǳ�������
				--newDown;
				//printf("\n%d %d %d %d %d \n", i, newUp, preClr, curClr, j-bound[LEFT]);
				break;
			}else{
				preClr = midClr;		// ʹ���������1����ֹ���ڱ�Եģ�������¼�����
				midClr = curClr;
			}
		}
		if (flag == 1){ // ������Ӧ��ɫ�����䣬���³�ʼ����־
			flag = 0;
			PLClrCnt = 0;
		}else{	// �����ڴ�����Ӧ��ɫ�����䣬�����ڸ��±߽磬�˳�
			break;
		}
		//printf("\n");
	}
	//printf("\n%d %d \n", bound[DOWN], newDown);
	bound[UP] = newUp;
	bound[DOWN] = newDown;
	
	return 0;
}

BYTE*	rgbToGray(const BYTE* imageArr, LONG width, LONG height){
	int		i, k;
	BYTE	blue, green, red;
	BYTE	*bmpArr;

	bmpArr = (BYTE*)malloc(sizeof(BYTE)*width*height);
	for (i=0, k=0; i<width*height; ++i, k+=3){
		blue	= imageArr[k];
		green	= imageArr[k+1];
		red		= imageArr[k+2];
		bmpArr[i]	= (299*red+587*green+114*blue)/1000;
	}
	//free(imageArr);
	printSuccess("rgbToGray");
	return bmpArr;
}

void	medianFilter(BYTE* imageArr, LONG width, LONG height){
	int		i, j, k, g;
	BYTE	sudoku[9];	//��¼�Ե�ǰ��Ϊ���ĵľŹ������

	for (i=1; i<height-1; ++i){
		for(j=1; j<width-1; ++j){
			for (k=0; k<3; ++k){
				for (g=0; g<3; ++g){
					sudoku[3*k+g] = imageArr[(i+g-1)*width+(j+k-1)];
				}
			}
			imageArr[i*width+j] = getMedianValue(sudoku, 9, 256);
		}
	}
	printSuccess("medianFilter");
}

void	sobleSideEnhance(BYTE* imageArr, LONG width, LONG height, int mode){
	int		x, y,
			pixel_x, pixel_y, pixel; //�ֱ�������������Ե����ͼ��Ҷ�ֵ
	BYTE*	tempArr = (BYTE*)calloc(width*height, sizeof(BYTE));
	
	for (y=1; y<height-1; ++y){
		for(x=1; x<width-1; ++x){
			pixel_x = (imageArr[(y-1)*width+x-1] + 2*imageArr[(y-1)*width+x] + 
				imageArr[(y-1)*width+x+1]) - (imageArr[(y+1)*width+x-1] +
				2*imageArr[(y+1)*width+x] + imageArr[(y+1)*width+x+1]);
			pixel_y = (imageArr[(y-1)*width+x+1] + 2*imageArr[y*width+x+1] + 
				imageArr[(y+1)*width+x+1]) - (imageArr[(y-1)*width+x-1] +
				2*imageArr[y*width+x-1] + imageArr[(y+1)*width+x-1]);
			if (mode == 0)
			{
				pixel =  abs(pixel_x)+abs(pixel_y);
			}
			else if (mode == 1)
			{
				pixel = abs(pixel_x);
			}
			else if (mode == 2)
			{
				pixel = abs(pixel_y);		// ֻ���д�ֱ��Ե��ǿ
			}
			
			pixel = pixel > 255 ? 255 : pixel;
			
			tempArr[y*width+x] = (BYTE)pixel;	// ����ֱ�ӽ�pixelд��Ŀ�������У�����ᵼ�����ӵ�ԭͼ���ݴ���
		}
	}
	memcpy((BYTE*)imageArr, (BYTE*)tempArr, width*height*sizeof(BYTE));
	/*for (y=1; y<height-1; ++y){
		for (x=1; x<width-1; ++x){
			imageArr[y*width+x] = tempArr[y*width+x];
		}
	}*/

	free(tempArr);
	printSuccess("sobleSideEnhance");
}

BYTE	ostuThresh(BYTE* imageArr, LONG width, LONG height){
	LONG	iN1=0, iN2=0,				// �ֱ��屳����Ŀ�����صĸ���
			totalPix = width*height;	
	BYTE	Th = 0, T;				// ������ֵ������ʼ��Ϊ0
	//		dVar1, dVar2,, dDiffVar			// �ֱ���Ŀ��ͱ����ķ���
	double	dInnerVar,				// �ֱ����������ڵķ���
			dAvgGray1, dAvgGray2,	// �ֱ���Ŀ��ͱ�����ƽ������
			dSum1=0, dSum2=0,		// �ֱ��屳����Ŀ�����صĺ�
			dMaxVar = 0,			// 
			dWeight1, dweight2;	// �ֱ��屳����Ŀ�����ص���ռͼ��ı���
	int		i; 

	for (T=1; T!=255; ++T){ // ������(T<256)��Ϊ�ж���������ΪBYTE��������ѭ������С��256
		for (i=0; i<totalPix; ++i){
			if (imageArr[i] > T){
				++iN2;
				dSum2 += imageArr[i];
			}else{
				++iN1;
				dSum1 += imageArr[i];
			}
		}
		dAvgGray1 = (iN1==0) ? 0.0 : dSum1/iN1;
		dAvgGray2 = (iN2==0) ? 0.0 : dSum2/iN2;
		dWeight1 = (double)iN1/totalPix;
		dweight2 = (1.0-dWeight1);
		// ��䷽��
		dInnerVar = dWeight1*dweight2*(dAvgGray1-dAvgGray2)*(dAvgGray1-dAvgGray2);
		if (dInnerVar > dMaxVar){
			dMaxVar = dInnerVar;
			Th = T;
		}
		//printf(" %d %lf %d %d %d \n", T, dInnerVar, Th, iN1, iN2);
		iN1=0; iN2=0; dSum1=0; dSum2=0; 
	}
	
	return Th;
}

void	binarization(BYTE* imageArr, LONG width, LONG height){
	int		i;
	BYTE	Th;
	LONG	totalPixl = width*height;

	Th = ostuThresh(imageArr, width, height);
	//printf("%d \n", Th);
	for (i=0; i<totalPixl; ++i){
		imageArr[i] = imageArr[i] > Th ? 255 : 0;
	}

	printSuccess("binarization");
}

void	binarization1(BYTE* imageArr, LONG width, LONG height){
	int		i, j=0;
	LONG	initTh, newTh, leftTh=0, rightTh=0, sum1=0, sum2;
	BYTE	histogramNums[256];

	// ͳ��ֱ��ͼ
	for (i=0; i<width*height; ++i){
		++histogramNums[imageArr[i]];
	}

	// ͳ����ѧ��������Ϊ��ʼ��ֵ
	for (i=0; i<256; i++){
		sum1 += i*histogramNums[i];
	}
	initTh = sum1/(width*height);
	printf("%d \n", initTh);
	// ���Ҷ�ֱ��ͼ��initThΪ�е㣬��ֱ��ͼ��Ϊ�����֣��ֱ������ֲ���ֵ
	while(1) {
		if (initTh > 255){
			printf("error");
			exit(0);
		}
		sum1 = 0;
		sum2 = 0;
		for (i=0; i<initTh; ++i){
			sum1 += i*histogramNums[i];
			sum2 += histogramNums[i];
		}
		leftTh = sum1/sum2;
		sum1 = 0;
		sum2 = 0;
		for (i=initTh; i<256; ++i){
			sum1 += i*histogramNums[i];
			sum2 += histogramNums[i];
		}
		rightTh = sum1/sum2;
		printf("%d %d \n", leftTh, rightTh);
		newTh = leftTh+rightTh;
		if (newTh != initTh){
			initTh = newTh;
		}else{
			break;
		}
		j++;
		if(j>100) break;
	};

	printf("%d %d \n", newTh, j);
}
 
void	removeNoise(BYTE* imageArr, LONG width, LONG height){
	LONG*	M = (LONG*)calloc(width*height, sizeof(LONG));	// ��¼�����ҵı߳�
	LONG*	N = (LONG*)calloc(width*height, sizeof(LONG));	// ��¼���ҵ���ı߳�
	//BYTE*	N = (BYTE*)calloc(width*height, sizeof(BYTE));	// ��¼���ҵ���ı߳�
	LONG	i, j, curIndex;
	LONG	temp[6] = {0};
	const	LONG	ThLong = 100, ThShort = 20;

	/*for (i=0; i<width*height; ++i){
	imageArr[i] = (imageArr[i]==255) ?
	}*/

	for (j=2; j<height; ++j){
		for (i=2; i<width-2; ++i){
			curIndex = j*width+i;
			if (imageArr[curIndex] == 255){
				if (imageArr[curIndex-1]+imageArr[(j-1)*width+i-1]
				+imageArr[(j-1)*width+i]+imageArr[(j-1)*width+i+1] > 0){
					temp[0] = M[curIndex-1];
					temp[1] = M[(j-1)*width+i-1];
					temp[2] = M[(j-1)*width+i];
					temp[3] = M[(j-1)*width+i+1];
					M[curIndex] = 1 + getExtremumValueByte(temp, 4, 1);
				} else{
					temp[0] = M[curIndex-2];
					temp[1] = M[(j-1)*width+i-2];
					temp[2] = M[(j-1)*width+i+2];
					temp[3] = M[(j-2)*width+i-1];
					temp[4] = M[(j-2)*width+i];
					temp[5] = M[(j-2)*width+i+1];
					M[curIndex] = 1 + getExtremumValueByte(temp, 6, 1);
				}
			}
		}
	}

	//for (j=0; j<height-2; ++j){
	//	for (i=2; i<width-2; ++i){
	for (j=height-3; j>=0; --j){
		for (i=width-3; i>1; --i){
			curIndex = j*width+i;
			if (imageArr[curIndex] == 255){
				if (imageArr[curIndex+1]+imageArr[(j+1)*width+i-1]
				+imageArr[(j+1)*width+i]+imageArr[(j+1)*width+i+1] >0){
					temp[0] = N[curIndex+1];
					temp[1] = N[(j+1)*width+i-1];
					temp[2] = N[(j+1)*width+i];
					temp[3] = N[(j+1)*width+i+1];
					N[curIndex] = 1 + getExtremumValueByte(temp, 4, 1);
				} else{
					temp[0] = N[curIndex+2];
					temp[1] = N[(j+1)*width+i-2];
					temp[2] = N[(j+1)*width+i+2];
					temp[3] = N[(j+2)*width+i-1];
					temp[4] = N[(j+2)*width+i];
					temp[5] = N[(j+2)*width+i+1];
					N[curIndex] = 1 + getExtremumValueByte(temp, 6, 1);
				}
			}
		}
	}
	//printArr(M, width*height);
	/*for (i=0; i<height*height; ++i){
		if (M[i]!=0 || N[i]!=0){
			printf("%d=%d ", M[i], N[i]);
		}
			
	}*/
	//printArr(N, width*height);
	//printf("%d %d", width, height);
	for (j=0; j<height; ++j){
		for (i=0; i<width; ++i){
			curIndex = j*width+i;
			if (imageArr[ curIndex] == 255){
				if (M[curIndex] + N[curIndex] > ThLong || M[curIndex] + N[curIndex] < ThShort){
					imageArr[curIndex] = 0;
				}
			}
		}
	}
}

void	drawBound(BYTE* bmpArr, LONG width, LONG heigh,  LONG bound[]){
	int		i;
	if (bound[0]>bound[1]){
		for (i=0; i<width; ++i){
			bmpArr[bound[0]*width+i] = 255;
			bmpArr[bound[1]*width+i] = 255;
		}

	}
	if (bound[3]>bound[2]){
		for (i=0; i<heigh; ++i){
			bmpArr[i*width+bound[2]] = 255;
			bmpArr[i*width+bound[3]] = 255;
		}
	}
}

int		checkBounds(const LONG bound[]){
	return (bound[UP]>bound[DOWN] && bound[RIGHT]>bound[LEFT]);
}

int		getPLColor(const BYTE* imageArr, LONG width, LONG heigh){
	double	ratio[5] = {0};		// ��Ӧ������ɫ���ֵĸ��ʣ��ֱ���������ƣ��ף��ڣ��ǳ��Ʊ�����ɫ
	int		cnt[5] = {0};			// ��Ӧ������ɫ���ֵĴ������ֱ���������ƣ��ף��ڣ��ǳ��Ʊ�����ɫ
	int		i, j, k;
	double	r, g, b;
	LONG	totalPixls = width*heigh;
	LONG	tempTotal = totalPixls*3;
	for (i=0; i<totalPixls; i+=3){
		b = imageArr[i];			// ��ɫͨ���Ĵ�ȡ˳��Ϊb,g,r��������r,g,b
		g = imageArr[i+1];
		r = imageArr[i+2];
		++cnt[checkColor(b, g, r)];
	}
	//printf("%d \n", i);
	for (i=0; i<5; ++i){
		ratio[i] = (double)cnt[i]/totalPixls;
		//printf("%d %d %lf ", cnt[i], totalPixls, ratio[i]);
	}

	return getExtremumIndex(ratio, 5, 1);
}

int		checkColor(double b, double g, double r){
	int		res;
	if(b>1.5*r && b>1.5*g)
		res = BLUE_PL;
	else if(b<2*r/3 && b<2*g/3)
		res = YELLOW_PL;
	else if(b>150 && r>150 && g>150)
		res = WHITE_PL;
	else if(b<70 && r<70 && g<70)
		res = BLACK_PL;
	else
		res = OTHERS_PL;
	return res;
}

int		isMatchPLVerClr(int PLColor, BYTE preClr, BYTE curClr, LONG* bound, LONG PLClrCnt){
	LONG	PLClrCntIndex = (LONG)(bound[RIGHT]-bound[LEFT]+1)/7*1.5; //������ɫ���ص�ĸ������ó���������������ж�Ϊ������˿
	int		flag = 0;
	switch (PLColor){
	case BLUE_PL:  // ���װ��ֳ���
		//flag = (preClr==BLUE_PL && curClr==WHITE_PL && PLClrCnt<PLClrCntIndex);
		flag = (preClr==BLUE_PL && curClr==WHITE_PL);
		break;
	case YELLOW_PL: // �Ƶ׺��ֳ���
		//flag = (preClr==YELLOW_PL && curClr==BLACK_PL && PLClrCnt<PLClrCntIndex);
		flag = (preClr==YELLOW_PL && curClr==BLACK_PL);
		break;
	case WHITE_PL: // �׵׺��ֳ���
		//flag = (preClr==WHITE_PL && curClr==BLACK_PL && PLClrCnt<PLClrCntIndex);
		flag = (preClr==WHITE_PL && curClr==BLACK_PL);
		break;
	case BLACK_PL: // �ڵװ��ֳ���
		//flag = (preClr==BLACK_PL && curClr==WHITE_PL && PLClrCnt<PLClrCntIndex);
		flag = (preClr==BLACK_PL && curClr==WHITE_PL);
		break;
	}

	return flag;
}

int		isMatchPLHorClr(int PLColor, BYTE curClr){
	int		flag = 0;
	switch (PLColor){
	case BLUE_PL:  // ���װ��ֳ���
		flag = (curClr == BLUE_PL)?1:((curClr == WHITE_PL)?2:0);
		//flag = (curClr==BLUE_PL || curClr==WHITE_PL);
		break;
	case YELLOW_PL: // �Ƶ׺��ֳ���
		//flag = (curClr==YELLOW_PL || curClr==BLACK_PL);
		flag = (curClr == YELLOW_PL)?1:((curClr == BLACK_PL)?2:0);
		break;
	case WHITE_PL: // �׵׺��ֳ���
		//flag = (curClr==WHITE_PL || curClr==BLACK_PL);
		flag = (curClr == WHITE_PL)?1:((curClr == BLACK_PL)?2:0);
		break;
	case BLACK_PL: // �ڵװ��ֳ���
		//flag = (curClr==BLACK_PL || curClr==WHITE_PL);
		flag = (curClr == BLACK_PL)?1:((curClr == WHITE_PL)?2:0);
		break;
	}

	return flag;
}


