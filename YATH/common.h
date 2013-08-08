#ifndef _COMMON_H
#define _COMMON_H

#include <windows.h>

#define yathcmdNONE				0
#define yathcmdERROR			0xffffffff
#define yathcmdLISTFILES		100
#define yathcmdCHANGEDIR		101
#define yathcmdGETCURRENTRIR	102
#define yathcmdMAKEDIR			103
#define yathcmdREMOVEFILE		104
#define yathcmdREMOVEDIR		105
#define yathcmdGETFILE			106
#define yathcmdPUTFILE			107
#define yathcmdMOVE				108


typedef struct yathtagFILEINFO {
	DWORD		FileAttributes;
	FILETIME	FileCreationTime;
	FILETIME	FileLastAcessTime;
	FILETIME	FileLastWriteTime;
	DWORD		FileSizeHi;
	DWORD		FileSizeLo;	
	char		FileName[ MAX_PATH];
} yathFILEINFO;



#endif