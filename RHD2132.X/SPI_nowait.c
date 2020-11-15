#include "SPI_nowait.h"

volatile SPI2_State MEM_SPI_State;
volatile SPI1_State INTAN_SPI_State;

void spi2_NoWait_Initialize(void){
    
    MEM_SPI_State = SPI2_IDLE; 
}

void spi2_exchangeByte_NoWait(uint8_t data){
    
    SPI2BUF = data;
    MEM_SPI_State = SPI2_EXCHANGE;
}

// Full Duplex SPI Functions
/*
void SPI2_Exchange( uint8_t *pTransmitData, uint8_t *pReceiveData )
{

    while( SPI2STATbits.SPITBF == true )
    {

    }

    SPI2BUF = *((uint8_t*)pTransmitData);

    while ( SPI2STATbits.SRXMPT == true);

    *((uint8_t*)pReceiveData) = SPI2BUF;

}
*/

void SPI1_NoWait_Initialize(void){
    
    INTAN_SPI_State = SPI1_IDLE; 
}


void SPI1_Exchange16bit_NoWait(uint16_t data){
    
    SPI1BUF = data;
    INTAN_SPI_State = SPI1_EXCHANGE;
    
}
 
/*
void SPI1_Exchange( uint8_t *pTransmitData, uint8_t *pReceiveData )
{

    while( SPI1STATbits.SPITBF == true )
    {

    }

    SPI1BUF = *((uint16_t*)pTransmitData);

    while ( SPI1STATbits.SRXMPT == true);

    *((uint16_t*)pReceiveData) = SPI1BUF;

}*/