#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bitmapStruct.h"

void	printArr(const int* nums, int numsSize){
	int i;

	printf("\n");
	for (i=0; i<numsSize; ++i){
		printf("%4d ", nums[i]);
	}
	printf("\n");
}

void	printArrLong(const LONG* nums, int numsSize){
	int i;

	printf("\n");
	for (i=0; i<numsSize; ++i){
		printf("%4d ", nums[i]);
	}
	printf("\n");
}

void	printArrDbe(const double* nums, int numsSize){
	int i;

	printf("\n");
	for (i=0; i<numsSize; ++i){
		printf("%6.2lf ", nums[i]);
	}
	printf("\n");
}

void printErrors(char* info){
	printf("\n!!! Error: %s !!!\n\n", info);
}

void	printSuccess(char* info){
	//printf("\n!!! Success: %s !!!\n\n", info);
}

char*	strJoin(char* a, char* b){
	char* c = (char*)malloc(strlen(a)+strlen(b)+1);
	if(c == NULL){
		printErrors("malloc failed");
	}
	strcat(c, a);
	strcat(c, b);

	return c;
}

double	getDecimalPart(double x){
	return x - (int)x;
}

double	getExtremumValue(const double nums[], int numsSize, int mode){
	int		i; 
	double	max, min;

	max = min = nums[0];
	for (i=0; i<numsSize; ++i){
		max = (max>nums[i]) ? max : nums[i];
		min = (min<nums[i]) ? min : nums[i];
	}

	return (mode==0) ? min : max;
}

LONG	getExtremumValueByte(const LONG nums[], int numsSize, int mode){
	int		i; 
	LONG	max, min;

	max = min = nums[0];
	for (i=0; i<numsSize; ++i){
		max = (max>nums[i]) ? max : nums[i];
		min = (min<nums[i]) ? min : nums[i];
	}

	return (mode==0) ? min : max;
}

int		getExtremumIndex(const double nums[], int numsSize, int mode){
	int i, maxIndex, minIndex;

	maxIndex = minIndex = 0;
	for (i=0; i<numsSize; ++i){
		maxIndex = (nums[maxIndex]>nums[i]) ? maxIndex : i;
		minIndex = (nums[minIndex]<nums[i]) ? minIndex : i;
	}

	return (mode==0) ? minIndex : maxIndex;
}

BYTE*	countingSort(BYTE* nums, int numsSize, int maxValue){
	int		i;
	BYTE*	arrTemp = (BYTE*)calloc(maxValue, sizeof(BYTE));
	BYTE*	result = (BYTE*)calloc(numsSize, sizeof(BYTE));

	for (i=0; i<numsSize; ++i){
		++arrTemp[nums[i]];
	}

	// arrTemp[i] now contains the number of elements equal to i;

	for (i=1; i<maxValue; ++i){
		arrTemp[i] += arrTemp[i-1];
	}

	// arrTemp[i] now contains the number of elements less than or equal to i;

	for (i=numsSize-1; i>=0; --i){
		result[arrTemp[nums[i]]-1] = nums[i];
		--arrTemp[nums[i]];
	}

	return result;
}

BYTE	getMedianValue(BYTE* nums, int numsSize, int maxValue){
	BYTE*	sortedNums = countingSort(nums, numsSize, maxValue);
	
	return sortedNums[numsSize/2];
}

float	fMax(float a, float b){
	return (a>b)?a:b;
}