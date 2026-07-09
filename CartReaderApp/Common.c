#include "Common.h"
#include "flashparam.h"
#include <string.h>
#include "Operate.h"

//SD Card
FIL myDir;
FIL myFile;
// Array that holds the data
FATFS fs;
byte sdBuffer[512];

//remember folder number to create a new folder for every save
saveFolderConfig conf;
char folder[36];

// File browser
char fileName[FILENAME_LENGTH];
char filePath[FILEPATH_LENGTH];
byte currPage;
byte lastPage;
byte numPages;
bool root = 0;

// Common
char romName[64];
unsigned long sramSize = 0;
int romType = 0;
int manufacturerid = 0;
byte saveType;
halfword romSize = 0;
char checksumStr[5];
bool errorLvl = 0;
bool ignoreError = 0;
char flashid[5];
//char vendorID[5];
//
//unsigned long sramBase;
//unsigned long flashBanks;



// Variable to count errors
unsigned long writeErrors;

/*********************************************************************
*
System base parts

*/

static volatile int ticks = 0;

void SysClockInit()
{
  // Enable SysTick timer interrupt
  //Sets timer to pulse every 108000 clock cycles (1us)
  SysTick->LOAD = (SystemCoreClock / 1000000) - 1;
  //Reset SysTick counter
  SysTick->VAL = 0;
  //Set SysTick clock source to CPU clock, enable the exception request and enable the counter
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Handler(void) 
{
  // Increment the ticks value every time the SysTick exception fires
  ticks++;
}

int getSystick()
{
  return ticks;
}

void delay(int n) {
  // Wait for N ticks. Restarts the timer for more accurate delay
  unsigned endTicks = ticks + n;
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | ~SysTick_CTRL_ENABLE_Msk;
  SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk;
  while (ticks < endTicks);
}

void ResetSystem()
{
  //
  __set_FAULTMASK(1);//关闭总中断
  NVIC_SystemReset();//请求单片机重启
}

void SysClockFree()
{
  // Disable SysTick interrupt
  SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}


void delayMicroseconds(uint16_t us)
{
  delay(us);
}

FRESULT createStoreDirectory(FIL* tfile, char* romName, char* extension, char* systemName, char* fileType){
  // Get name, add extension and convert to char array for sd lib
  strcpy(fileName, romName);
  strcat(fileName, extension);
  char buf[100];

  // create a new folder for the rom file
  conf = get_config();
  f_chdir("/");

  sprintf(buf, "%s/%s/", systemName, fileType, romName);
  if(conf.optype < 2) {
    sprintf(folder, "%s/%d", romName, conf.foldern);
  } else {
    sprintf(folder, "%s/%s", romName, conf.custname[conf.optype-2]);
  }
  strcat(buf, folder);

  FRESULT res;
  res = my_mkdir(buf);
  res = f_chdir(buf);

  OledClear();
  OledShowString(0,0,"Saving to ",8);
  OledShowString(4,1,folder,8);

  // write new folder number back to eeprom
  if(conf.optype<2){
    conf.foldern += conf.optype;
  }
  save_config(conf);

  //open file on sd card
  res = f_open(tfile, fileName, FA_CREATE_ALWAYS|FA_WRITE);
  if (res != FR_OK) {
    print_Error("Can't create file", 1);
  }
  return res;
}