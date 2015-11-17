#define  _CRT_SECURE_NO_WARNINGS // ���ζ���fopen��warning
//*******************************************************************************/
#include	<stdio.h>
#include	<string.h>
#include	"fileManage.h"
#include	"bmpManage.h"
#include	"preprocess.h"
#include	"utils.h"

#define ROOT_PATH "F:\\deep learning\\LPR\\"
#define IMAGE_LIST_FILE "image list.txt"
#define SOURCE_FOLDER	"source\\"
#define RGB_TO_GRAY_FOLDER "rgbToGray\\"
#define MEDIAA_FILTER_FOLDER "medianFilter\\"
#define SIDE_ENHANCE_FOLDER "sobleSideEnhance\\"
#define BINAR_FOLDER "binarization\\"
#define BOUND_LOCATION "bounds\\"
#define LOCATION_PL "locatePL\\"
#define LOCATION_PL_CLR "locatePL_Clr\\"
#define BLUE_PRO "��ɫǰ��ͼ\\"
#define WHITE_PRO "��ɫǰ��ͼ\\"

#define TEMP "temp\\"

#define PATH_MAX	100

void goGoGo(char* src, int begin, int end);		//ʵ��ͼƬ������

int main(){
	
	goGoGo(TEMP,11, 20);
	
	return 0;
}

void goGoGo(char* src, int begin, int end){
	int		i=0;
	char	temp1[PATH_MAX]= {0};
	char	temp2[PATH_MAX]= {0};
	char	imageFname[20];	//ͼƬ��
	char	imageListSrc[PATH_MAX] ={0};
	char	source[PATH_MAX] = {0};
	char	dest[PATH_MAX] = {0};
	FILE*	fpImgList;
	int		cnt = 0;

	strcat(imageListSrc, ROOT_PATH);
	strcat(source, ROOT_PATH);
	strcat(dest, ROOT_PATH);

	strcat(imageListSrc, IMAGE_LIST_FILE);
	strcat(source, SOURCE_FOLDER);
	strcat(dest, src);					// �޸Ĵ˴����ı�Ŀ���ļ��Ĵ洢λ��

	strcpy(temp1, source);
	strcpy(temp2, dest);
	if ((fpImgList = fopen(imageListSrc, "r"))!=NULL){
		while(fscanf(fpImgList,"%s",imageFname)>0){ //ѭ������
			//if(i++>=cnt) break;
			cnt++;
			if (cnt>=begin && cnt<=end){
				strcat(source, imageFname);
				strcat(dest, imageFname);
				//locatePL(source, dest);
				locatePL_clr(source, dest);
				printf("%s \n",imageFname);
				strcpy(source, temp1);
				strcpy(dest, temp2);
			}
			if(cnt>=end) break;
		}
		fclose(fpImgList);
	}
	
	printf("totally handle %d photos! \n", end-begin+1);
	return;
}
