#ifndef _GBA_H_
#define _GBA_H_

#define ADDR_1    GPIOD
#define ADDR_2    GPIOA
#define ADDR_3    GPIOE
#define CTRLGBA   GPIOB

#define CS_SRAM    GPIO_PIN_7
#define GBA_WR     GPIO_PIN_13
#define GBA_RD     GPIO_PIN_14
#define CS_ROM     GPIO_PIN_15
#define A0         GPIO_PIN_8
#define AD23       GPIO_PIN_15

void TestMemGBA(bool bFast);
uint8_t gbaScreen();
void setup_GBA();
unsigned long verifyFLASH_GBA(unsigned long flashSize, uint32_t pos, uint8_t bank, uint8_t numbanks);


#endif

//ADDR1 A0  PD8
//ADDR1 A1  PD9
//ADDR1 A2  PD10
//ADDR1 A3  PD11
//ADDR1 A4  PD12
//ADDR1 A5  PD13
//ADDR1 A6  PD14
//ADDR1 A7  PD15
//
//ADDR2 A8  PA8
//ADDR2 A9  PA9
//ADDR2 A10 PA10
//ADDR2 A11 PA11
//ADDR1 A12 PD4
//ADDR1 A13 PD5
//ADDR1 A14 PD6
//ADDR1 A15 PD7
//
//ADDR3 A16 PE8
//ADDR3 A17 PE9
//ADDR3 A18 PE10
//ADDR3 A19 PE11
//ADDR3 A20 PE12
//ADDR3 A21 PE13
//ADDR3 A22 PE14
//ADDR3 A23 PE15
//
//
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