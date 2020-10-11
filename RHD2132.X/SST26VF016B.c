#include "mcc_generated_files/mcc.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "SST26VF016B.h"

uint8_t READ_STATUS_REG(void){
    
    uint8_t Tx_buf[2];
    Tx_buf[0] = RDSR;
    
    uint8_t Rx_buf[2];
    
    int i;
    CS2_SetLow();
    for(i=0; i<2; i++){
        Rx_buf[i] = spi2_exchangeByte(Tx_buf[i]);
    }
    CS2_SetHigh();
    return Rx_buf[1];    
}

void JEDECID(uint8_t* data){
    
    uint8_t Tx_buf[4];
    Tx_buf[0] = RDJEDECID;
    int i;
    for (i=1; i<4; i++){
        Tx_buf[i] = NOP;
    }
    uint8_t Rx_buf[4];
    
    CS2_SetLow();
    for(i=0; i<4; i++){
        Rx_buf[i] = spi2_exchangeByte(Tx_buf[i]);
    }
    CS2_SetHigh();
    
    memcpy(data, &Rx_buf[1], 3);
    
}

bool IS_BUSY(void){
    
    // By checking status REG
    uint8_t Status;
    Status = READ_STATUS_REG();
    if(Status & isBUSY_MASK)
        return true;
    else
        return false;
}

bool IS_WRITEABLE(void){
    
    // By checking status REG
    uint8_t Status;
    Status = READ_STATUS_REG();
    if(Status & isWRITEALBE_MASK)
        return true;
    else
        return false;
}

bool WRITE_ENABLE(void){
    
    uint8_t CMD;
    CMD = WREN;
    spi2_writeByte(CMD);
    
    return(IS_WRITEABLE());
}

bool WRITE_DISABLE(void){
    
    uint8_t CMD;
    CMD = WRDI;
    spi2_writeByte(CMD);
    
    // check status register to confirm that data is not able to be written
    return(!(IS_WRITEABLE()));
}

void READ_MEM(uint32_t addr, uint8_t *data, uint16_t n){
    
    uint8_t *Tx_buf;
    Tx_buf = (uint8_t *) malloc(n+4);
    
    Tx_buf[0] = READ;
    Tx_buf[1] = (addr >> 16) & 0xFF;
    Tx_buf[2] = (addr >> 8) & 0xFF;
    Tx_buf[3] = addr & 0xFF;
    
    uint8_t *Rx_buf;
    Rx_buf = (uint8_t *) malloc(n+4);
    
    int i;
    CS2_SetLow();
    for(i=0; i<(n+4); i++){
        Rx_buf[i] = spi2_exchangeByte(Tx_buf[i]);
    }
    CS2_SetHigh();    
    memcpy(data, &Rx_buf[4], n);
    
    free(Tx_buf);
    free(Rx_buf);
}

bool PAGE_PROGRAM(uint16_t sec_no, uint16_t addr, uint8_t* data, uint16_t n){
    
    // maximum data cycle = 256
    if(n>256)
        return false;
    
    // Starting address
    uint32_t address;
    address = sec_no;
    address <<= 12;
    address += addr;
    
    // Set to be writable and check if it's busy
    while(!(WRITE_ENABLE()));
    if(IS_BUSY())
        return false;
    
    // Write data
    uint8_t *Tx_buf;
    Tx_buf = (uint8_t *) malloc(n+4);
    
    Tx_buf[0] = PP;
    Tx_buf[1] = (address >> 16) & 0xFF;
    Tx_buf[2] = (address >> 8) & 0xFF;
    Tx_buf[3] = address & 0xFF;
    memcpy(&Tx_buf[4], data, n);
    
    int i;
    CS2_SetLow();
    for(i=0; i<(n+4); i++){
        spi2_writeByte(Tx_buf[i]);
    }
    CS2_SetHigh();
    // waiting
    while(IS_BUSY());
    
    free(Tx_buf);
    
    return true;
}