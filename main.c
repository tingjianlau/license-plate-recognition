#define  _CRT_SECURE_NO_WARNINGS // 屏蔽对于fopen的warning
//*******************************************************************************/
#include	<stdio.h>
#include	<string.h>
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

#define TEMP "temp\\"

#define PATH_MAX	100

void goGoGo(char* src, int begin, int end);		//实验图片的张数
char*	statusInfo(int status);


int main(){
	
	goGoGo(TEMP, 39, 100);
	
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