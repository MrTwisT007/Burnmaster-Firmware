#pragma onece

#define ADDRLOW    GPIOD
#define ADDRHIGH   GPIOA
#define DATA       GPIOE
#define CTRL       GPIOB

#define RST        GPIO_PIN_7
#define CLK        GPIO_PIN_12
#define WR         GPIO_PIN_13
#define RD         GPIO_PIN_14
#define CS         GPIO_PIN_15


extern int sramBanks;
extern int romBanks;
extern word lastByte;

void TestMemGB(boolean bFast);
void gbFlashScreen();
void gbScreen();

//ADDRLOW   A0  PD8     FLASH/SRAM
//ADDRLOW   A1  PD9     FLASH/SRAM
//ADDRLOW   A2  PD10    FLASH/SRAM
//ADDRLOW   A3  PD11    FLASH/SRAM
//ADDRLOW   A4  PD12    FLASH/SRAM
//ADDRLOW   A5  PD13    FLASH/SRAM
//ADDRLOW   A6  PD14    FLASH/SRAM
//ADDRLOW   A7  PD15    FLASH/SRAM
                        
//ADDRHIGH  A8  PA8     FLASH/SRAM
//ADDRHIGH  A9  PA9     FLASH/SRAM
//ADDRHIGH  A10 PA10    FLASH/SRAM
//ADDRHIGH  A11 PA11    FLASH/SRAM
//ADDRLOW   A12 PD4     FLASH/MBC5/SRAM
//ADDRLOW   A13 PD5     FLASH/MBC5
//ADDRLOW   A14 PD6           MBC5
//ADDRLOW   A15 PD7 FLASH(CE)/MBC5   Reading past 0x7FFF will DESELECT FLASH. MBC can still access it through AA15 when ROM bank is > 0
                        
//DATA      D0  PE8     FLASH/MBC5/SRAM
//DATA      D1  PE9     FLASH/MBC5/SRAM
//DATA      D2  PE10    FLASH/MBC5/SRAM
//DATA      D3  PE11    FLASH/MBC5/SRAM
//DATA      D4  PE12    FLASH/MBC5/SRAM
//DATA      D5  PE13    FLASH/MBC5/SRAM
//DATA      D6  PE14    FLASH/MBC5/SRAM
//DATA      D7  PE15    FLASH/MBC5/SRAM
                        
//CTRL      RST PB7     MBC5/BMS
//CTRL      CLK PB12    
//CTRL      WR  PB13    MBC5/SRAM
//CTRL      RD  PB14    FLASH/SRAM
//CTRL      CS  PB15    MBC5

//GPIO IO control registers:
//GPIO_CTL0 0-7     {CTLn[1:0],MDn[1:0]}
//GPIO_CTL1 8-15
//
//CTLn:
//input:
//00 0x0: Analog mode
//01 0x1: Floating input
//10 0x2: Input with pull-up / pull-down
//11 0x3: Reserved
//
//output:
//00 0x0: GPIO output with push-pull
//01 0x1: GPIO output with open-drain
//10 0x2: AFIO output with push-pull
//11 0x3: AFIO output with open-drain
//
//MDn:
//00 0x0: Input mode (reset state)
//01 0x1: Output mode ,max speed 10MHz
//10 0x2: Output mode ,max speed 2 MHz
//11 0x3: Output mode ,max speed 50MHz
//
//GPIO Output control registers:
//GPIO_OCTL 0-16     {16'h0,OCTL[15:0]}