
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/spi1_driver.h"


void INTAN_testCom(uint16_t mess)
{
    CS_SetLow();
    
    spi1_exchangeWord(mess);
    
    CS_SetHigh();
    
}


