#define  _CRT_SECURE_NO_WARNINGS // 屏蔽对于fopen的warning
//*******************************************************************************/
#include	<stdio.h>
#include	<string.h>
#include	<math.h>
#include	"fileManage.h"
#include	"bmpManage.h"
#include	"preprocess.h"
#include	"utils.h"

#define ROOT_PATH "E:\\Deep Learning\\LPR\\LPR\\"
#define IMAGE_LIST_FILE "image list.txt"
#define	LOCATION_RES_FILE "location results.txt"
#define	FEATURES_FILE "features.txt"
#define	WIGHT_FILE "neuron.txt"
#define RGB_TO_GRAY_FOLDER "rgbToGray\\"
#define MEDIAA_FILTER_FOLDER "medianFilter\\"
#define SIDE_ENHANCE_FOLDER "sobleSideEnhance\\"
#define BINAR_FOLDER "binarization\\"
#define BOUND_LOCATION "bounds\\"
#define LOCATION_PL "locatePL\\"
#define LOCATION_PL_CLR "locatePL_Clr\\"
#define BLUE_PRO "蓝色前景图\\"
#define WHITE_PRO "白色前景图\\"

#define TEMP "locatePL_Clr\\"
#define SOURCE_FOLDER	"source\\"

#define PATH_MAX	100

void preTrain(char* src, int begin, int end);		//实验图片的张数
char*	statusInfo(int status);


char	features[PATH_MAX] = { 0 };
char	weightSrc[PATH_MAX] = { 0 };
int		succPLCnt = 0;	// 成功提取特征的车牌的数量
int main(){
	// 神经网络训练之前的工作：包括车牌的定位，切割，归一化，细化及特征提取
	preTrain(TEMP, 1, 1);

	trainNetworkMain(features, weightSrc, 6* succPLCnt);
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
	
	
	
	return 0;
}

void preTrain(char* src, int begin, int end){
	FILE* fpFeatures;
	int		i=0;
	char	temp1[PATH_MAX]= {0};
	char	temp2[PATH_MAX]= {0};
	char	imageFname[20];	//图片名
	char	imageListSrc[PATH_MAX] ={0};
	char	source[PATH_MAX] = {0};
	char	dest[PATH_MAX] = {0};
	char	locateRes[PATH_MAX] = { 0 };
	FILE*	fpImgList, *pfRes;
	int		cnt = 0, realCnt = 0, status=0, cntLocFail=0, cntLocSuc=0, cntLocSuc2=0;
	BYTE    *thinnerImg = NULL;

	strcat(locateRes, ROOT_PATH);
	strcat(imageListSrc, ROOT_PATH);
	strcat(source, ROOT_PATH);
	strcat(dest, ROOT_PATH);
	strcat(features, ROOT_PATH);
	strcat(weightSrc, ROOT_PATH);
	
	strcat(locateRes, LOCATION_RES_FILE);
	strcat(imageListSrc, IMAGE_LIST_FILE);
	strcat(source, SOURCE_FOLDER);
	strcat(features, FEATURES_FILE);
	strcat(weightSrc, WIGHT_FILE);
	strcat(dest, src);					// 修改此处，改变目标文件的存储位置

	strcpy(temp1, source);
	strcpy(temp2, dest);
	// 提取特征并写入文本中
	fpFeatures = openTextFile(features, "w");
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
				
				// 车牌定位，分割，归一化，细化
				thinnerImg = locatePL_clr(source, dest, &status);
				

				if (status == LOCATE_FAIL || status == FIRST_LOCATE_SPLIT_FAIL || status == SEC_LOCATE_SPLIT_FAIL)
				{
					cntLocFail++;
				}
				else if (status == FIRST_LOCATE_SPLIT_SECC || status == SEC_LOCATE_SPLIT_SECC)
				{
					// 提取特征及真实的字符编号
					writeFeatures(imageFname, fpFeatures, thinnerImg);
					succPLCnt++;
					cntLocSuc++;
				}
				fprintf(pfRes, "%s -> %s (%d) \n\n", imageFname, statusInfo(status), status);
				printf("%s -> %s (%d) \n\n",imageFname, statusInfo(status), status);
				strcpy(source, temp1);
				strcpy(dest, temp2);
			}
			if(cnt>=end) break;
		}
		
	}
	fprintf(pfRes, "\ntotally handle %d photos! \n", cntLocFail + cntLocSuc);
	fprintf(pfRes, "失败: %d;  成功: %d; 成功率: %.2lf \n", cntLocFail, cntLocSuc, (cntLocSuc*1.0 / (cntLocFail + cntLocSuc)*1.0));
	printf("\ntotally handle %d photos! \n", cntLocFail+ cntLocSuc);
	printf("失败: %d;  成功: %d; 成功率: %.2lf \n\n", cntLocFail, cntLocSuc, (cntLocSuc*1.0 / (cntLocFail + cntLocSuc)*1.0));
	
	fclose(fpImgList);
	fclose(pfRes);
	fclose(fpFeatures);
	return;
}

char*	statusInfo(int status) {
	char*	info = (char*)malloc(30 * sizeof(char));

	switch (status)
	{
	case LOCATE_FAIL:
		info = "定位失败";
		break;
	case FIRST_LOCATE_SPLIT_SECC:
		info = "第一次定位成功且切割成功";
		break;
	case SEC_LOCATE_SPLIT_SECC:
		info = "第二次定位成功且切割成功";
		break;
	case FIRST_LOCATE_SPLIT_FAIL:
		info = "第一次定位成功但切割失败";
		break;
	case SEC_LOCATE_SPLIT_FAIL:
		info = "第二次定位成功但切割失败";
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