#ifndef _UTILS_H_
#define _UTILS_H_

// ��ӡһά����
void	printArr(int* nums, int numsSize);
void	printArrDbe(double* nums, int numsSize);
//void	printArrLong(const LONG* nums, int numsSize);

// ��ӡ������Ϣ
void printErrors(char* info);
// ��ӡ�ɹ���Ϣ
void	printSuccess(char* info);

char*	strJoin(char* a, char* b);
// ��ȡС������
double	getDecimalPart(double x);

double	getExtremumValue(const double nums[], int numsSize, int mode);

//LONG	getExtremumValueByte(const LONG nums[], int numsSize, int mode);
int		getExtremumIndex(const double nums[], int numsSize, int mode);

float	fMax(float a, float b);
#endif

