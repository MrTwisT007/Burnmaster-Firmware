#ifndef _COMMON_H_
#define _COMMON_H_

#include <gd32f10x.h>
#include <stdio.h>
#include <stdlib.h>
#include "Display.h"
#include "fatfs/ff.h"



#define byte uint8_t
#define halfword uint16_t
#define word uint32_t
#define bool uint8_t
#define true (1)
#define false (0)


#define FILENAME_LENGTH 64
#define FILEPATH_LENGTH 128
#define FILEOPTS_LENGTH 20

typedef struct {
  uint32_t foldern;
  uint32_t optype;
  char     custname[3][16];
} saveFolderConfig;

extern saveFolderConfig conf;
extern char folder[36];
extern FATFS fs;

extern char fileName[FILENAME_LENGTH];
extern char filePath[FILEPATH_LENGTH];
extern byte currPage;
extern byte lastPage;
extern byte numPages;
extern bool root;
extern bool filebrowse;
extern bool errorLvl;
extern bool ignoreError;
extern char flashid[5];
extern int manufacturerid;

// Variable to count errors
extern unsigned long writeErrors;


extern char romName[64];
extern unsigned long sramSize;
extern int romType;
extern byte saveType;
extern halfword romSize;
extern char checksumStr[5];


//SD Card
extern byte sdBuffer[512];


int getSystick();
void SysClockInit();
void SysTick_Handler(void);
void delay(int n);
void ResetSystem();
void SysClockFree();

void delayMicroseconds(uint16_t us);
FRESULT createStoreDirectory(FIL* tfile, char* romName, char* extension, char* systemName, char* fileType);

#endif