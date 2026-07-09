#include <stdio.h>
#include <string.h>
#include <gd32f10x.h>
#include "flashparam.h"
#include "Common.h"
 


void save_dword(uint32_t data, uint32_t offset, uint8_t erasePage)
{
  fmc_unlock();
  if(erasePage)
    fmc_page_erase(FMC_WRITE_START_ADDR);
  fmc_word_program(FMC_WRITE_START_ADDR+offset, data);
  //lock the main FMC after the program operation */
  fmc_lock();
}

uint32_t load_dword(uint8_t offset)
{
  //uint32_t *ptr = (volatile uint32_t *)FMC_WRITE_START_ADDR;
  //return  ptr[0];
  return *(volatile uint32_t *)(uint32_t)(FMC_WRITE_START_ADDR+offset);
}

//Save save folder config struct in flash
void save_config(saveFolderConfig conf) {
  fmc_unlock();
  fmc_page_erase(FMC_WRITE_START_ADDR);
  for (uint8_t i=0; i < sizeof(conf)/4; i++) {
    fmc_word_program(FMC_WRITE_START_ADDR+(i*sizeof(uint32_t)), ((uint32_t*)&conf)[i]);
  }
  fmc_lock();
}

saveFolderConfig get_config() {
  return *(volatile saveFolderConfig *)FMC_WRITE_START_ADDR;
}