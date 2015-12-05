#define  _CRT_SECURE_NO_WARNINGS // 屏蔽对于fopen的warning
//*******************************************************************************/
#include	<stdio.h>
#include	<string.h>
#include	<math.h>
#include	"fileManage.h"
#include	"bmpManage.h"
#include	"preprocess.h"
#include	"utils.h"

#define ROOT_PATH "F:\\deep learning\\LPR\\"
#define IMAGE_LIST_FILE "image list.txt"
#define	LOCATION_RES_FILE "location results.txt"
#define SOURCE_FOLDER	"test\\"
#define RGB_TO_GRAY_FOLDER "rgbToGray\\"
#define MEDIAA_FILTER_FOLDER "medianFilter\\"
#define SIDE_ENHANCE_FOLDER "sobleSideEnhance\\"
#define BINAR_FOLDER "binarization\\"
#define BOUND_LOCATION "bounds\\"
#define LOCATION_PL "locatePL\\"
#define LOCATION_PL_CLR "locatePL_Clr\\"
#define BLUE_PRO "蓝色前景图\\"
#define WHITE_PRO "白色前景图\\"

#define TEMP "temp\\1\\"

#define PATH_MAX	100

void goGoGo(char* src, int begin, int end);		//实验图片的张数
char*	statusInfo(int status);


int main(){
	//FILE* fp;
	//BYTE* tempArr, *rotateArr;
	//BITMAP_IMAGE bmpImage;
	//LONG	width, height, temp1, temp2;
	//LONG	xOrigin, yOrigin, rFirst, rSize, i, j, rLast;
	//int		numAngles;
	//double	*thetaPtr, *radonTable, *maxArr;
	//int		angles;
	//LONG	rotatedWidth = 0, rotatedHeight = 0;
	//
	//fp = loadImage("F:\\deep learning\\LPR\\temp\\云A526EG.BMP", &bmpImage);
	//tempArr = creatImageArr(fp, &bmpImage);
	//width = bmpImage.infoHeader.biWidth;
	//height = bmpImage.infoHeader.biHeight;

	//angles = KTTransform(tempArr, width, height);
	//rotateArr = rotate(tempArr, -angles, width, height, &rotatedWidth, &rotatedHeight);
	////printf("%lf \n", cos(2*PI));
	//bmpImage.infoHeader.biWidth = rotatedWidth;
	//bmpImage.infoHeader.biHeight = rotatedHeight;
	//creatBmpByArr("F:\\deep learning\\LPR\\temp\\云A526EG_2.BMP", &bmpImage, rotateArr, 8);
	/*angles = hough(tempArr, width, height);
	printf("%d \n", angles);*/
	// ===================================
	//xOrigin = (width-1)/2;
	//yOrigin = (height - 1) / 2;

	//temp1 = height - 1 - yOrigin;
	//temp2 = width - 1 - xOrigin;
	//rLast = (LONG)ceil(sqrt((double)(temp1*temp1 + temp2*temp2))) + 1;
	//rFirst = -rLast;
	//rSize = rLast - rFirst + 1;

	//numAngles = 3;
	////rFirst = 0;
	////rSize = (LONG)sqrt(width*width + height*height) + 1;
	//thetaPtr = (double*)calloc(numAngles, sizeof(double));
	//for (i = 0; i < numAngles; i++)
	//{
	//	thetaPtr[i] = i+10;
	//}
	//radonTable = (double*)calloc(rSize*numAngles, sizeof(double));
	//maxArr = (double*)calloc(numAngles, sizeof(double));
	///*for ( i = 0; i < height; i++)
	//{
	//	for ( j = 0; j < width; j++)
	//	{
	//		if (tempArr[i*width + j] != 0)
	//		{
	//			printf("%d ", tempArr[i*width + j]);
	//		}
	//		
	//	}
	//}*/
	//radon(radonTable, tempArr, thetaPtr, width, height, xOrigin, yOrigin, numAngles, rFirst, rSize);
	////printf("%d ", (LONG)sqrt(width*width + height*height) + 1);
	//for (i = 0; i < numAngles; i++)
	//{
	//	/*for ( j = 0; j < rSize; j++)
	//	{
	//		if (radonTable[i*rSize + j] != 0 )
	//		{
	//			printf("%.2lf ", radonTable[i*rSize + j]);
	//		}
	//		
	//	}*/
	//	//quickSort(radonTable + i*rSize, 0, rSize - 1);
	//	//maxArr[i] = getMaxValue(radonTable + i*rSize, rSize);
	//	//printf("%lf \n", maxArr[i]);
	//}
	//for (i = 0; i < numAngles; i++)
	//{
	//	//for (j = 1; j < rSize; j++)
	//	//{
	//		//maxArr[i] += fabs(radonTable[i*rSize + j] - radonTable[i*rSize + j-1]);
	//		maxArr[i] = getMaxValue(radonTable + i*rSize, rSize);
	//		printf("%lf \n", maxArr[i]);
	//	//}
	//}
	//printf("\n ==%d ", getMaxIndex(maxArr, numAngles));
	//
	//maxArr[i] = getMaxValue(radonTable + i*rSize, rSize);
	// ===================================
	/*fclose(fp);
	free(tempArr);
	free(rotateArr);*/
	//printf("\n %d ", getMaxIndex(maxArr, numAngles));
	
	goGoGo(TEMP, 1, 32);
	
	return 0;
}

void goGoGo(char* src, int begin, int end){
	int		i=0;
	char	temp1[PATH_MAX]= {0};
	char	temp2[PATH_MAX]= {0};
	char	imageFname[20];	//图片名
	char	imageListSrc[PATH_MAX] ={0};
	char	source[PATH_MAX] = {0};
	char	dest[PATH_MAX] = {0};
	char	locateRes[PATH_MAX] = { 0 };
	FILE*	fpImgList, *pfRes;
	int		cnt = 0, realCnt = 0, status, cntLocFail=0, cntLocSuc1=0, cntLocSuc2=0;

	strcat(locateRes, ROOT_PATH);
	strcat(imageListSrc, ROOT_PATH);
	strcat(source, ROOT_PATH);
	strcat(dest, ROOT_PATH);

	strcat(locateRes, LOCATION_RES_FILE);
	strcat(imageListSrc, IMAGE_LIST_FILE);
	strcat(source, SOURCE_FOLDER);
	strcat(dest, src);					// 修改此处，改变目标文件的存储位置

	strcpy(temp1, source);
	strcpy(temp2, dest);
	if ((fpImgList = fopen(imageListSrc, "r"))!=NULL){
		

		if ((pfRes = fopen(locateRes, "w")) == NULL)
		{
			printf("open the file %s failed!\n", locateRes);
			exit(0);
		}
		while(fscanf(fpImgList,"%s",imageFname)>0){ //循环读入
			//if(i++>=cnt) break;
			cnt++;
			if (cnt>=begin && cnt<=end){
				strcat(source, imageFname);
				strcat(dest, imageFname);
				//locatePL(source, dest);
				status = locatePL_clr(source, dest);
				if (status == 0)
				{
					cntLocFail++;
				}
				else if (status == 1)
				{
					cntLocSuc1++;
				}
				else if (status == 2)
				{
					cntLocSuc2++;
				}
				fprintf(pfRes, "%s -> %s \n", imageFname, statusInfo(status));
				printf("%s -> %d \n",imageFname, status);
				strcpy(source, temp1);
				strcpy(dest, temp2);
			}
			if(cnt>=end) break;
		}
		
	}
	fprintf(pfRes, "totally handle %d photos! \n", cntLocFail + cntLocSuc1 + cntLocSuc2);
	fprintf(pfRes, "失败: %d;  第一次定位成功: %d;  第二次定位成功: %d; 成功率: %d \n", cntLocFail, cntLocSuc1, cntLocSuc2, (cntLocFail / (cntLocFail + cntLocSuc1 + cntLocSuc2)*1.0));
	printf("totally handle %d photos! \n", cntLocFail+ cntLocSuc1+ cntLocSuc2);
	printf("失败: %d;  第一次定位成功: %d;  第二次定位成功: %d; 成功率: %d \n", cntLocFail, cntLocSuc1, cntLocSuc2, (cntLocFail/(cntLocFail+ cntLocSuc1+ cntLocSuc2)*1.0));
	
	fclose(fpImgList);
	fclose(pfRes);
	return;
}

char*	statusInfo(int status) {
	char*	info = (char*)malloc(30 * sizeof(char));

	switch (status)
	{
	case 1:
		info = "第一次定位成功";
		break;
	case 2:
		info = "第二次定位成功";
		break;
	default:
		info = "定位失败";
		break;
	}

	return info;
}

//void	locateLog(char* src, char* bmpName, int status) {
//	FILE*	pfRes;
//
//	if ((pfRes = fopen(src, "w"))==NULL)
//	{
//		sprintf("open the file %s failed!", src);
//		exit(0);
//	}
//	sprintf(pfRes, "%s -> %s \n", bmpName, statusInfo(status));	
//
//	fclose()
//}