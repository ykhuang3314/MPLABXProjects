#include "mcc_generated_files/mcc.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "SST26VF016B.h"
#define FCY _XTAL_FREQ/2
#include "libpic30.h"
#include "Comm.h"
#include "SPI_nowait.h"


uint8_t Write_Addr[3];
int cnt_addr, cnt_data;
uint8_t Rx_SPI2;
uint8_t wdata[6];
volatile SST26VF016B_PP State_PP;
volatile SST26VF016B_WREN State_WREN;

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

// having issue with dynamic memory allocation
/*
void READ_MEM(uint32_t addr, uint8_t *data, uint16_t n){
    
    uint8_t *Tx_buf;
    Tx_buf = (uint8_t*)malloc(n+4);
    
    Tx_buf[0] = READ;
    Tx_buf[1] = (addr >> 16) & 0xFF;
    Tx_buf[2] = (addr >> 8) & 0xFF;
    Tx_buf[3] = addr & 0xFF;
    
    int i;
    for(i=4; i<(n+4); i++){
        Tx_buf[i] = NOP;
    }
    
    uint8_t *Rx_buf;
    Rx_buf = (uint8_t*)malloc(n+4);

    CS2_SetLow();
    for(i=0; i<(n+4); i++){
        Rx_buf[i] = spi2_exchangeByte(Tx_buf[i]);
    }
    CS2_SetHigh();    
    memcpy(data, &Rx_buf[4], n);
    
    free(Tx_buf);
    free(Rx_buf);
}
*/
void READ_MEM_256(uint32_t addr, uint8_t *data){
    
    int data_size;
    data_size = 256;
    uint8_t Tx_buf[data_size+4];
        
    Tx_buf[0] = READ;
    Tx_buf[1] = (addr >> 16) & 0xFF;
    Tx_buf[2] = (addr >> 8) & 0xFF;
    Tx_buf[3] = addr & 0xFF;
    
    int i;
    for(i=4; i<(data_size+4); i++){
        Tx_buf[i] = NOP;
    }
    
    uint8_t Rx_buf[data_size+4];

    CS2_SetLow();
    for(i=0; i<(data_size+4); i++){
        Rx_buf[i] = spi2_exchangeByte(Tx_buf[i]);
    }
    CS2_SetHigh();
    
    memcpy(data, &Rx_buf[4], data_size);
}

// having issue with dynamic memory allocation

/*
void PAGE_PROGRAM(uint16_t sec_no, uint16_t addr, uint8_t *data, uint16_t n){
    
    // Starting address
    uint32_t address;
    address = sec_no;
    address <<= 12;
    address += addr;
    
    //prior to page program, execute write enable
    WRITE_ENABLE();
    
    // Write data
    uint8_t *Tx_buf;
    uint8_t Rx;
    Tx_buf = (uint8_t*)malloc(n+4);
    
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
*/

void PAGE_PROGRAM_256(uint16_t sec_no, uint16_t addr, uint8_t *data){
    
    int data_size;
    data_size = 256;
            
    // Starting address
    uint32_t address;
    address = sec_no;
    address <<= 12;
    address += addr;
    
    //prior to page program, execute write enable
    WRITE_ENABLE();
    
    // Write data
    uint8_t Tx_buf[data_size+4];
    uint8_t Rx;
    
    Tx_buf[0] = PP;
    Tx_buf[1] = (address >> 16) & 0xFF;
    Tx_buf[2] = (address >> 8) & 0xFF;
    Tx_buf[3] = address & 0xFF;
    memcpy(&Tx_buf[4], data, data_size);
    
    int i;
    CS2_SetLow();
    for(i=0; i<(data_size+4); i++){
        Rx = spi2_exchangeByte(Tx_buf[i]);
    }
    CS2_SetHigh();
    // waiting
    while(IS_BUSY());

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
    Tx_buf[1] = (Addr >> 16) & 0xFF;   // addr[23:16]
    Tx_buf[2] = (Addr >> 8) & 0xFF;    // addr[15:8]
    Tx_buf[3] = Addr & 0xFF;           // addr[7:0]
    
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
    while(IS_BUSY() && flagwait);
}

// The followings are for testing purpose 

bool TEST_COMM_MEM(void){
    uint8_t ID[3];
    JEDECID(ID);
    if((ID[0] == 0xBF) && (ID[1] == 0x26) && (ID[2] == 0x41))
        return true;
    else
        return false;
}

void Writing_State_Initialize(void){
    
    State_PP = IDLE_PP;
    State_WREN = IDLE_WREN;
    //IFS2bits.SPI2IF = 0;
}

void SPI2_NoWait_getByte(uint8_t data){
    Rx_SPI2 = data;
}

void Writing_Initialize(uint16_t sec_no, uint16_t addr, uint8_t *data){
    
    // Starting address
    uint32_t address;
    address = sec_no;
    address <<= 12;
    address += addr;
    
    Write_Addr[0] = (address >> 16) & 0xFF;
    Write_Addr[1] = (address >> 8) & 0xFF;
    Write_Addr[2] = address & 0xFF;
    
    memcpy(wdata, data, 6);
    
    cnt_addr = 3;
    cnt_data = 6;
    
    //clear flags
    //IFS2bits.SPI2IF = 0;
    //SPI2STATbits.SPIROV = 0;
    //SPI2STATbits.SPITBF = 0;
}

void WRITE_ENABLE_NoWait(void){
    
    switch(State_WREN){
        
        case IDLE_WREN:
            CS2_SetLow();
            spi2_exchangeByte_NoWait(WREN);
            State_WREN = END_WREN;
            break;
        
        case END_WREN:
            CS2_SetHigh();
            State_WREN = IDLE_WREN;
            MEM_SPI_State = SPI2_IDLE;
            break;
            
        default:
            break;
    }
}

void PAGE_PROGRAM_NoWait(void){
    
    switch(State_PP){
        
        case IDLE_PP: // Send page program command
            CS2_SetLow();
            spi2_exchangeByte_NoWait(PP);
            State_PP = ADDRESS_PP;
            break;
           
        case ADDRESS_PP: // Send 3byte Address for writing (MSB first)
            spi2_exchangeByte_NoWait(Write_Addr[3-cnt_addr]);
            cnt_addr--;
            if(cnt_addr>0)
                State_PP = ADDRESS_PP;           
            else
                State_PP = WRITE_PP;          
            break;
            
        case WRITE_PP: // Writing data
            spi2_exchangeByte_NoWait(wdata[6-cnt_data]);
            cnt_data--;
            if(cnt_data>0)
                State_PP = WRITE_PP;          
            else
                State_PP = END_PP;
            break;
        
        case END_PP:
            CS2_SetHigh();
            State_PP = IDLE_PP;
            MEM_SPI_State = SPI2_IDLE;
            _put("done\n");
            break;
            
        default:
            break;
    }
}

bool TEST_WRITE_READ(void){
    
    bool TestResult, flag_run;
    
    uint16_t data_size, wsec_no, init_addr;
    data_size = 6;
    uint8_t wdata[data_size], rdata[data_size];
    //uint8_t dummy;
    
    wdata[0] = 'K';
    wdata[1] = 'T';
    wdata[2] = 'H';
    wdata[3] = 'E';
    wdata[4] = 'M';
    wdata[5] = 'G';
    
    char results[data_size];
    
    // Address
    wsec_no = 0; init_addr = 0;
    
    // initialization for writing
    Writing_Initialize(wsec_no, init_addr, wdata);
    
    // To write data into memory, unlock block protection REG.
    UNLOCK_PROTECTION();
    
    // Erase memory array before writing
    SECTOR_ERASE(wsec_no, true);    
    
    // Write data into memory
    flag_run = true;
    WRITE_ENABLE();

    PAGE_PROGRAM_NoWait();
    while(flag_run){
        if(MEM_SPI_State == SPI2_EXCHANGE){
            if(IFS2bits.SPI2IF){ 
                IFS2bits.SPI2IF = 0;    
                MEM_SPI_State = SPI2_IDLE;
                //Need to read receive buffer such that the hardware clear SPIRBF bit
                //or clear the receive overflow flag bit manually to make it work.
                //dummy = SPI2BUF;
                SPI2STATbits.SPIROV = 0;
                PAGE_PROGRAM_NoWait();                 
            }
        }
        else{
            flag_run = false;
        }
    }
    
    IS_BUSY();
    
    // Starting address for reading
    uint32_t addr;
    addr = (wsec_no << 12) | init_addr;
    READ_MEM_TEST(addr, rdata);
    
    int i;   
    TestResult = true;
    for(i=0; i<data_size; i++){
        TestResult &= (rdata[i] == wdata[i]);
        results[i] = (char)rdata[i];
    }
    _put(results);
    _put("\n");
    return TestResult;
}

void READ_MEM_TEST(uint32_t addr, uint8_t *data){
    
    uint8_t Tx_buf[10];
        
    Tx_buf[0] = READ;
    Tx_buf[1] = (addr >> 16) & 0xFF;
    Tx_buf[2] = (addr >> 8) & 0xFF;
    Tx_buf[3] = addr & 0xFF;
    
    int i;
    for(i=4; i<10; i++){
        Tx_buf[i] = NOP;
    }
    
    uint8_t Rx_buf[10];

    CS2_SetLow();
    for(i=0; i<10; i++){
        Rx_buf[i] = spi2_exchangeByte(Tx_buf[i]);
    }
    CS2_SetHigh();    
    memcpy(data, &Rx_buf[4], 6);
}

