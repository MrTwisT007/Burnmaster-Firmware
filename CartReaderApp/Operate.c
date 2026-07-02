
#include <gd32f10x.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Common.h"
#include "Display.h"
#include "Operate.h"


#define DEBOUNCE_US 10000
#define REPEAT_INTERVAL 100
#define REPEAT_PREDELAY 200

/*********************************************************************
*
Keyboard

*/

int buttonPressed = 0;
bool isButtonHeld = false;
unsigned char choice_ori = 0;


void KeyBrdInit()
{
  //
  gpio_init(GPIOE, GPIO_MODE_IPU, GPIO_OSPEED_2MHZ, 0x3F/*GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3|GPIO_PIN_4|GPIO_PIN_5*/);
  gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_2MHZ, GPIO_PIN_1);//低电量获取
}

uint8_t keyState()
{
  //
  return (~GPIO_ISTAT(GPIOE))&0x3F;
}


uint8_t checkButton(int previousbutton)
{
  int ms_counter = 0;
  uint8_t keycode = keyState();
  if (!isButtonHeld)
    delay(DEBOUNCE_US);

  //Button repeat loop
  //Check if the previously pressed/held button is OK or CANCEL
  if(previousbutton > 8) {
    //If the pressed button matches the previous button, wait till the user lets go
    while((keycode = keyState()) == previousbutton) {
      delay(DEBOUNCE_US);
    }
  // Otherwise, repeat the button code at regular interval
  } else if (previousbutton > 0) {
    while(keyState() == previousbutton) {
      delay(1000);
      ms_counter++;
      if(!isButtonHeld) {
        if(ms_counter == REPEAT_PREDELAY) {
          isButtonHeld = true;
          break;
        }
      } else {
        if(ms_counter == REPEAT_INTERVAL) {
          break;
        }
      }
    }
  } else {
    isButtonHeld = false;
  }
  
  //Low battery indicator
  if(gpio_input_bit_get(GPIOB,GPIO_PIN_1) == RESET)
  {
    LED_RED_ON;
  }
  else
  {
    LED_RED_OFF;
  }
  
  //If button is still held at this point, return the value
  if(keyState() == keycode)
  {
    return keycode&0x3F;
  }

  return BTNNONE;
}


void WaitOKBtn()
{
  while((buttonPressed = checkButton(buttonPressed)) != BTNOK)
  {
    //delay(88);    
  }
}



// Display a question box with selectable itemNames. Make sure default choice is in (0, numSelections]
unsigned char questionBox_OLED(char * question, const char* const itemNames[7], int numSelections, int selection, uint8_t wrapAround, uint8_t clrSrc) 
{
  //clear the screen
  if(clrSrc > 0)OledClear();

  // change the rgb led to the start menu color
  //rgbLed(selection);

  // print menu
  OledShowString(0,0,question,8);
  char tanswer[21] = {0};
  for (unsigned char i = 0; i < numSelections; i++) {
    memcpy(tanswer,itemNames[i],20);
    OledShowString(6,i+1,tanswer,8);
  }

  // start with the default choice
  unsigned char choice = selection;
  choice_ori = selection;

  // draw selection box
  OledShowChar(0,choice,'*',8);

  //unsigned long idleTime = millis();
  uint8_t currentColor = 0;

  uint32_t scroll_tick = 0;
  uint8_t scroll_start = 0;


  // wait until user makes his choice
  while (1) 
  {
    // Attract Mode
    /*
    if (millis() - idleTime > 300000) {
      if ((millis() - idleTime) % 4000 == 0) {
        if (currentColor < 7) {
          currentColor++;
          if (currentColor == 1) {
            currentColor = 2; // skip red as that signifies an error to the user
          }
        }
        else {
          currentColor = 0;
        }
      }
      rgbLed(currentColor);
    }*/
    buttonPressed = checkButton(buttonPressed);
    if(buttonPressed==BTNNONE)
    {
      //
      scroll_tick = scroll_tick + 1;
      if((scroll_tick > 14) && (scroll_tick%3 == 1))
      {
        if(OledShowString(6,choice,itemNames[choice - 1] + scroll_start,8) > 0)
        {
          scroll_start++;
        }
      }
    }
    else
    {
      printf("getKey-%d\n",buttonPressed);
      scroll_tick = 0;
      scroll_start = 0;
    }
    
    if(buttonPressed==BTNLEFT)
    {
      if(wrapAround)
      {}
      else{
        choice = MENU_PGUP;
        break;
      }
    }
    else
    if (buttonPressed == BTNRIGHT)
    {
      if(wrapAround)
      {}
      else{
        choice = MENU_PGDN;
        break;
      }
    }
    else
    if (buttonPressed == BTNUP)
    {
      choice--;
      if(choice <= 0)
      {
        if(wrapAround)
        {
          choice = numSelections;
        }
        else
        {
          choice = MENU_UPUP;
          break;
        }
      }
    }
    else
    if (buttonPressed == BTNDOWN)
    {
      choice++;
      if(choice > numSelections)
      {
        if(wrapAround)
        {
          choice = 1;
        }
        else
        {
          choice = MENU_DOWNDOWN;
          break;
        }
      }
    }
    else
    if (buttonPressed == BTNCANCEL)
    {
      choice = MENU_CANCEL;
      break;
    }
    else 
    if (buttonPressed == BTNOK) {
      break;
    }
  
    //show menu selected
    if(choice != choice_ori)
    {
      //
      OledShowChar(0,choice_ori,' ',8);
      OledShowString(6,choice_ori,itemNames[choice_ori-1],8);
      OledShowChar(0,choice,'*',8);
      choice_ori=choice;
    }
  }

  return choice;
}



uint8_t my_mkdir(char * dir)
{
  //
  uint8_t bret = false;
  bool opendir_err = 0;
  char SonPath[10][30]; //最多10层，每层最多30字符
  char RootPath[128];   //最低已存在路径
  memset(SonPath, '\0', sizeof(SonPath)); //初始化
  strcpy(RootPath, dir);
  uint8_t num = 0;           // 剔除的次数
  DIR W_Ddir;


  do                      //遍历寻找文件夹
  {
    char *dot = strrchr(RootPath, '\\');   //剔除一层
    if(dot == NULL)dot = strrchr(RootPath, '/');
    if(dot == NULL)
    {
      if(RootPath[0]){dot = RootPath;}
      else break;
    }

    strcpy(SonPath[num], dot);
    FRESULT W_Dresult = f_opendir(&W_Ddir, RootPath); //检测文件夹
    if(W_Dresult == FR_OK)
    {
            printf("Exist[%s]\r\n",RootPath);
            f_closedir(&W_Ddir);
            break;
    }
    else
    {
            printf("Err - %d[%s]\r\n",W_Dresult, RootPath);
            if(W_Dresult == FR_NO_PATH)
            {
                    opendir_err = 1;
                    num ++;
                    SonPath[num][0] = 0;
                    dot[0] = 0x00;
            }
            else return bret;
    }
  }
  while(1);


  if(opendir_err == 1)
  {
    //遍历创建文件夹
    opendir_err = 0;
    for(int i=0;i<num;i++)
    {
            strcat(RootPath, SonPath[num-i-1]);
            FRESULT W_Dresult = f_mkdir(RootPath);
            if(W_Dresult == FR_OK) { printf(">> Mk dir OK[%s]\r\n", RootPath); bret = true;}
            else printf(">> Err - %d [%s]\r\n",W_Dresult, RootPath);
    }
  }
  else bret = true;

  //
  return  bret;
}


/******************************************
  Filebrowser Module
*****************************************/
char fileNames[128][100];
char answer1[100];
char answer2[100];
char answer3[100];
char answer4[100];
char answer5[100];
char answer6[100];
char answer7[100];
char* tanswers[7] = {answer1,answer2,answer3,answer4,answer5,answer6,answer7};

void fileBrowser(char * start_dir , const char * browserTitle) {
  
  uint8_t filecount = 0;
  // Init Dir
  strcpy(filePath,start_dir);
  FILINFO finfo;
  DIR tdir;
  
  while(1){
    filecount = populateFileList(tdir, finfo, (char *)browserTitle);

    if (!openBrowserMenu(tdir, finfo, filecount, (char *)browserTitle)) {
      break;
    }
  }
}

uint8_t populateFileList(DIR tdir, FILINFO finfo, char * browserTitle) {
  uint8_t filecount = 0;

  while(filecount == 0){
    OledClear();
    
    // Open filepath directory
    if (f_opendir(&tdir,filePath) != FR_OK) {
      OledClear();
      print_Error("SD Error", true);
    }
    f_chdir(filePath);
    
    while(f_readdir(&tdir,&finfo) == FR_OK) {
      if(finfo.fname[0] == 0x00) {
        if (filecount == 0) {
          upOneDir(tdir);
          OledClear();
          OledShowString(16,3,"Empty directory",8);
          OledShowString(8,6,"Press OK to return",8);
          WaitOKBtn();
        }
        break;
      }
      strcpy(fileNames[filecount],finfo.fname);
    
      filecount++;
    }
  }
  return filecount;
}

bool openBrowserMenu(DIR tdir, FILINFO finfo, uint8_t filecount, char *browserTitle) {
  uint8_t currPage = 0;
  uint8_t default_select = 1;
  uint8_t menucnt = 0;
  uint8_t mret;
  bool flipPage = false;
  bool scrollPage = false;
  bool changeDir = false;
  bool onLastPage = false;
  FIL tf;

  while (1){

    // This may only be false after the first run of the loop
    if (!onLastPage) {
      menucnt = 0;
      for(uint8_t i = currPage*7; i < (currPage+1)*7; i++){
        strcpy(tanswers[menucnt],fileNames[i]);
        menucnt++;
        if (i >= (filecount-1)) {
          onLastPage = true;
          break;
        }
      }
    }

    // Clamp item selection position
    default_select = (menucnt > default_select) ? default_select : menucnt;
    
    mret = questionBox_OLED((char *)browserTitle,(const char **)tanswers,menucnt,default_select,0, (scrollPage || flipPage));
    scrollPage = false;
    flipPage = false;
          
    switch(mret)
    {
      case MENU_CANCEL:
        upOneDir(tdir);
        changeDir = true;
        return changeDir;
        break;

      case MENU_1:
      case MENU_2:
      case MENU_3:
      case MENU_4:
      case MENU_5:
      case MENU_6:
      case MENU_7:
        if(f_open(&tf,tanswers[mret - 1],FA_OPEN_EXISTING) != FR_OK) {
          strcat(filePath,"/");
          strcat(filePath,tanswers[mret - 1]);
          f_closedir(&tdir);
          changeDir = true;
          return changeDir;
        } else {
          f_close(&tf);
          strcat(filePath,"/");
          strcat(filePath,tanswers[mret - 1]);
          changeDir = false;
          return changeDir;
        }
        break;

      case MENU_PGUP:
        flipPage = true;
      case MENU_UPUP:
        if (currPage == 0) {
          default_select = 1;
        } else {
          currPage--;
          scrollPage = true;
          onLastPage = false;
          if (flipPage) {
            default_select = choice_ori;
            flipPage = false;
          } else {
            default_select = 7;
          }
        }
        break;

      case MENU_PGDN:
        flipPage = true;
      case MENU_DOWNDOWN: 
        if(onLastPage) {
          default_select = 7;
        } else {
          currPage++;
          scrollPage = true;
          if (flipPage) {
            default_select = choice_ori;
            flipPage = false;
          } else {
            default_select = 1;
          }
        }
        break;

      default:
        print_Error("File Err...",1);
        break;

    }
  }
}

void upOneDir(DIR tdir){
  for(int i = sizeof(filePath) - 1; i>0; i--) {
    if(filePath[i] == '/'||filePath[i] == '\\') {
      filePath[i] = 0x00;
      break;
    }
  }
  
  f_closedir(&tdir);
}