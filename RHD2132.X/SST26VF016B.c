#include "mcc_generated_files/mcc.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "SST26VF016B.h"
#define FCY _XTAL_FREQ/2
#include "libpic30.h"
#include "Comm.h"

uint8_t READ_STATUS_REG(void){
    
    uint8_t Tx_buf[2];
    Tx_buf[0] = RDSR; 
    Tx_buf[1] = NOP;
    
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

void WRITE_ENABLE(void){
    
    uint8_t CMD, Rx;
    CMD = WREN;
    CS2_SetLow();
    Rx = spi2_exchangeByte(CMD);
    CS2_SetHigh();
}

void WRITE_DISABLE(void){
    
    uint8_t CMD, Rx;
    CMD = WRDI;
    CS2_SetLow();
    Rx = spi2_exchangeByte(CMD);
    CS2_SetHigh();

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

void PAGE_PROGRAM(uint16_t sec_no, uint16_t addr, uint8_t* data, uint16_t n){
    
    // Starting address
    uint32_t address;
    address = sec_no;
    address <<= 12;
    address += addr;
    
    // Set to be writable and check if it's busy
    WRITE_ENABLE();
    
    // Write data
    uint8_t *Tx_buf;
    uint8_t Rx;
    Tx_buf = (uint8_t *) malloc(n+4);
    
    Tx_buf[0] = PP;
    Tx_buf[1] = (address >> 16) & 0xFF;
    Tx_buf[2] = (address >> 8) & 0xFF;
    Tx_buf[3] = address & 0xFF;
    memcpy(&Tx_buf[4], data, n);
    
    int i;
    CS2_SetLow();
    for(i=0; i<(n+4); i++){
        Rx = spi2_exchangeByte(Tx_buf[i]);
    }
    CS2_SetHigh();
    // waiting
    while(IS_BUSY());
    
    free(Tx_buf);

}

void LOCK_PROTECTION(void){
    uint8_t CMD, Rx;
    CMD = LBPR;
    //prior to lock block protection, execute write enable
    WRITE_ENABLE();
    CS2_SetLow();
    Rx = spi2_exchangeByte(CMD);
    CS2_SetHigh();
}

void UNLOCK_PROTECTION(void){
    uint8_t CMD, Rx;
    CMD = ULBPR;
    //prior to unlock block protection, execute write enable
    WRITE_ENABLE();
    CS2_SetLow();
    Rx = spi2_exchangeByte(CMD);
    CS2_SetHigh();
}

void SECTOR_ERASE (uint16_t sec_no, bool flagwait){
    uint8_t CMD, Rx;
    CMD = SE;
    uint32_t Addr;
    Addr = sec_no;
    Addr = Addr << 12; // 4KByte sector
    uint8_t Tx_buf[4];
    Tx_buf[0] = CMD;
    // 24-bit address (MSB first)
    Tx_buf[1] = (Addr >> 16) && 0xFF;   // addr[23:16]
    Tx_buf[2] = (Addr >> 8) && 0xFF;    // addr[15:8]
    Tx_buf[3] = Addr && 0xFF;           // addr[7:0]
    
    //prior to erase sector, execute write enable
    WRITE_ENABLE();
    
    int i;
    CS2_SetLow();
    for(i=0; i<4; i++){
        Rx = spi2_exchangeByte(Tx_buf[i]);
    }
    CS2_SetHigh();
    
    //waiting for erasing
    while(IS_BUSY() && flagwait);
}

void CHIP_ERASE (bool flagwait){
    uint8_t CMD, Rx;
    CMD = CE;
    //prior to erasing memory, execute write enable
    WRITE_ENABLE();
    CS2_SetLow();
    Rx = spi2_exchangeByte(CMD);
    CS2_SetHigh();
    //waiting for erasing
    while(IS_BUSY() && flagwait){
        __delay_us(10);
    }
}

bool TEST_COMM_MEM(void){
    uint8_t ID[3];
    JEDECID(ID);
    if((ID[0] == 0xBF) && (ID[1] == 0x26) && (ID[2] == 0x41))
        return true;
    else
        return false;
}


bool TEST_WRITE_READ(void){
    
    bool TestResult;
    
    uint16_t data_size, wsec_no, init_addr;
    data_size = 6;
    uint8_t wdata[data_size], rdata[data_size];
    
    wdata[0] = 'K';
    wdata[1] = 'T';
    wdata[2] = 'H';
    wdata[3] = 'E';
    wdata[4] = 'M';
    wdata[5] = 'G';
    
    // address
    wsec_no = 0; init_addr = 0;
    
    // To write data into memory, unlock block protection REG.
    UNLOCK_PROTECTION();
    
    // Erase memory array before writing
    SECTOR_ERASE (wsec_no, true);    
    
    // Write data into memory
    PAGE_PROGRAM (wsec_no, init_addr, wdata, data_size);
    
    // Starting address for reading
    uint32_t addr;
    addr = (wsec_no << 12) | init_addr;
    READ_MEM (addr, rdata, data_size);
    
    int i; 
    TestResult = true;
    for(i=0; i<data_size; i++){
        TestResult |= (rdata[i] == wdata[i]);
    }
    
    return TestResult;
    
}