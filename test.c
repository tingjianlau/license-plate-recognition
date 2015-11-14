#define _CRT_SECURE_NO_WARNINGS
//
//#include <stdio.h>
//#include <dos.h>
//
//#include "fileManage.h"

//int main(){
//	FIND_T	ffblk;
//	int		done;
//
//	done = _dos_findfirst("F:\\deep learning\\LPR\\sourceImg\\*.bmp", &ffblk, 0);
//
//	while(!done){
//		printf("%s \n", ffblk.name);
//		done = _dos_findnext(&ffblk);
//	}
//
//	return 0;
//}
//#include<stdio.h>
//#include<windows.h>
//
////HANDLE FindFirstFile(
////	LPCTSTR lpFileName,//filename
////	LPWIN32_FIND_DATA lpFindFileData//databuffer
////	);
//int main()
//{
//	WIN32_FIND_DATA p;
//	//const char* str = 
//	LPCTSTR src = (LPCTSTR)"F:\\deep learning\\LPR\\sourceImg\\*.bmp";
//	HANDLE h=FindFirstFile(src,&p);
//	puts((const char*)p.cFileName);
//	while(FindNextFile(h,&p))
//		puts((const char*)p.cFileName);
//	return 0;
//}
