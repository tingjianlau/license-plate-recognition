#ifndef _UTILS_H_
#define _UTILS_H_

#define	MAX_2(a, b) (( (a) > (b) )? (a) : (b) ) 
#define	MIn_2(a, b) (( (a) < (b) )? (a) : (b) )

#define	MAX_3(a, b, c) ( ( (a) > (b) ) ?( (a) > (c) ? (a) : (c) ):( (b) > (c) ? (b) : (c) ))
#define	MIN_3(a, b, c) ( ( (a) < (b) ) ?( (a) < (c) ? (a) : (c) ):( (b) < (c) ? (b) : (c) ))

#define	PI 3.141592653
// 打印一维数组
void	printArr(int* nums, int numsSize);
void	printArrDbe(double* nums, int numsSize);
//void	printArrLong(const LONG* nums, int numsSize);

// 打印错误信息
void printErrors(char* info);
// 打印成功信息
void	printSuccess(char* info);

char*	strJoin(char* a, char* b);
// 获取小数部分
double	getDecimalPart(double x);

double	getExtremumValue(const double nums[], int numsSize, int mode);

//LONG	getExtremumValueByte(const LONG nums[], int numsSize, int mode);
int		getExtremumIndex(const double nums[], int numsSize, int mode);

float	fMax(float a, float b);
double	getMaxValue(const double nums[], int numsSize);
int		getMaxIndex(const double nums[], int numsSize);
void	quickSort(double* A, int p, int r);
#endif

