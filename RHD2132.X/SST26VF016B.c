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
int cnt_data, cnt_addr;
uint8_t wdata[64];
uint8_t status;
volatile SST26VF016B_PP State_PP;
volatile SST26VF016B_ID State_ID;
volatile SST26VF016B_RDSR State_RDSR;

uint8_t READ_STATUS_REG(void){
    
    uint8_t Tx_buf[2];
    Tx_buf[0] = RDSR; 
    Tx_buf[1] = NOP;
    
    uint8_t Rx_buf[2];
    
    int i;
    CS2_SetLow();
    for(i=0; i<2; i++){
        Rx_buf[i] = SPI2_Exchange8bit(Tx_buf[i]);
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
        Rx_buf[i] = SPI2_Exchange8bit(Tx_buf[i]);
    }
    CS2_SetHigh();
    
    memcpy(data, &Rx_buf[1], 3);
    
}

void Check_Status_NoWait(void){
    
    switch(State_RDSR){
        
        case IDLE_RDSR:
            IFS2bits.SPI2IF = 0;
            CS2_SetLow();
            spi2_exchangeByte_NoWait(RDSR);
            State_RDSR = DUMMY_RDSR;
            break;
            
        case DUMMY_RDSR:
            
            spi2_exchangeByte_NoWait(NOP);
            State_RDSR = END_RDSR;
            break;
            
        case END_RDSR:
            CS2_SetHigh();
            if(status & isBUSY_MASK){
                IFS2bits.SPI2IF = 0;
                CS2_SetLow();
                spi2_exchangeByte_NoWait(RDSR);
                State_RDSR = DUMMY_RDSR;
            }
            else{
                State_PP = IDLE_RDSR;
                MEM_SPI_State = SPI2_IDLE;          
            }
            break;              
    }
}

void IS_BUSY_NoWait(void){
    
    uint8_t Rx;
    bool flag_run;
    int i;
    
    i = 0;
    flag_run = true;
    
    IFS2bits.SPI2IF = 0;
    Check_Status_NoWait();
       
    while(flag_run){
        if(MEM_SPI_State == SPI2_EXCHANGE){
            if(IFS2bits.SPI2IF){ 
                IFS2bits.SPI2IF = 0;
                MEM_SPI_State = SPI2_IDLE;
                //Need to read receive buffer such that the hardware clear SPIRBF bit
                Rx = SPI2BUF;
                if(State_RDSR == END_RDSR)
                    status = Rx;
                Check_Status_NoWait();                 
            }
        }
        else{
            flag_run = false;
        }
    } 
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
    Rx = SPI2_Exchange8bit(CMD);
    CS2_SetHigh();
}

void WRITE_DISABLE(void){
    
    uint8_t CMD, Rx;
    CMD = WRDI;
    CS2_SetLow();
    Rx = SPI2_Exchange8bit(CMD);
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
        Rx_buf[i] = SPI2_Exchange8bit(Tx_buf[i]);
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
        Rx = SPI2_Exchange8bit(Tx_buf[i]);
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
    Rx = SPI2_Exchange8bit(CMD);
    CS2_SetHigh();
}


void UNLOCK_PROTECTION(void){
    
    uint8_t CMD, Rx;
    CMD = ULBPR;
    //prior to unlock block protection, execute write enable
    WRITE_ENABLE();
    CS2_SetLow();
    Rx = SPI2_Exchange8bit(CMD);
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
        Rx = SPI2_Exchange8bit(Tx_buf[i]);
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
    Rx = SPI2_Exchange8bit(CMD);
    CS2_SetHigh();
    //waiting for erasing
    while(IS_BUSY() && flagwait);
}

// The followings are for testing purpose 

bool TEST_COMM_MEM(void){
    uint8_t ID[3];
    JEDECID(ID);
    //write_byte(ID, 3);
    if((ID[0] == 0xBF) && (ID[1] == 0x26) && (ID[2] == 0x41))
        return true;
    else
        return false;
}

void Writing_State_Initialize(void){
    
    State_PP = IDLE_PP;
    State_ID = CMD_ID;
    State_RDSR = IDLE_RDSR;
}

void Writing_Initialize(uint16_t sec_no, uint16_t addr, uint8_t *data){
    
    // Starting address
    int size;
    size = 64;
    uint32_t address;
    address = sec_no;
    address <<= 12;
    address += addr;
    
    Write_Addr[0] = (address >> 16) & 0xFF;
    Write_Addr[1] = (address >> 8) & 0xFF;
    Write_Addr[2] = address & 0xFF;
    
    memcpy(wdata, data, size);
    
    cnt_data = size;
    cnt_addr = 3;
    
}

void WRITE_ENABLE_NoWait(void){
    
    uint8_t dummy;

    bool flagrun;
    flagrun = true;

    CS2_SetLow();
    
    IFS2bits.SPI2IF = 0; 
    spi2_exchangeByte_NoWait(WREN);
    
    while(flagrun){
        if(MEM_SPI_State == SPI2_EXCHANGE){
               if(IFS2bits.SPI2IF){ 
                    IFS2bits.SPI2IF = 0;    
                    MEM_SPI_State = SPI2_IDLE;
                    //Need to read receive buffer such that the hardware clear SPIRBF bit
                    //or clear the receive overflow flag bit manually to make it work.
                    dummy = SPI2BUF;
                    //SPI2STATbits.SPIROV = 0;
                    CS2_SetHigh();
                    flagrun = false;
                }
        }
    }
}

void PAGE_PROGRAM_NoWait(void){
    
    switch(State_PP){
        
        case IDLE_PP: // Send page program command
            IFS2bits.SPI2IF = 0;
            CS2_SetLow();
            spi2_exchangeByte_NoWait(PP);
            State_PP = ADDRESS_PP;
            break;
        
        case ADDRESS_PP:
            spi2_exchangeByte_NoWait(Write_Addr[3-cnt_addr]);
            cnt_addr--;
            if(cnt_addr>0)
                State_PP = ADDRESS_PP;          
            else
                State_PP = WRITE_PP;
            break;          
     
        case WRITE_PP: // Writing data
            spi2_exchangeByte_NoWait(wdata[64-cnt_data]);
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
            //_put("done\n");
            break;
            
        default:
            break;
    }
}

void PAGE_PROGRAM_NoWait_V2(void){
    
    switch(State_PP){
        
        case IDLE_PP: // Send page program command
            IFS2bits.SPI2IF = 0;
            CS2_SetLow();
            spi2_exchangeByte_NoWait(PP);
            State_PP = ADDRESS_PP;
            break;
        
        case ADDRESS_PP:
            spi2_exchangeByte_NoWait(Write_Addr[3-cnt_addr]);
            cnt_addr--;
            if(cnt_addr>0)
                State_PP = ADDRESS_PP;          
            else
                State_PP = WRITE_PP;
            break;          
     
        case WRITE_PP: // Writing data
            spi2_exchangeByte_NoWait(wdata[64-cnt_data]);
            cnt_data--;
            if(cnt_data>0)
                State_PP = WRITE_PP;          
            else
                State_PP = ENDWRITE_PP;
            break;
        
        case ENDWRITE_PP:
            CS2_SetHigh();
            spi2_exchangeByte_NoWait(NOP);
            State_PP = RDSR_PP;
            break;
        
        case RDSR_PP:
            IFS2bits.SPI2IF = 0;
            CS2_SetLow();
            spi2_exchangeByte_NoWait(RDSR);
            State_PP = DUMMY_PP;
            break;
            
        case DUMMY_PP:
            spi2_exchangeByte_NoWait(NOP);
            State_PP = ISBUSY_PP;
            break;
            
        case ISBUSY_PP:
            CS2_SetHigh();
            if(status & isBUSY_MASK){
                IFS2bits.SPI2IF = 0;
                CS2_SetLow();
                spi2_exchangeByte_NoWait(RDSR);
                State_RDSR = DUMMY_PP; 
            }
            else{
                State_PP = IDLE_PP;
                MEM_SPI_State = SPI2_IDLE;   
            }   
            break;  
            
        default:
            break;
    }
}

void Test_write_initialize(void){
    
    uint16_t data_size, wsec_no, init_addr;
    data_size = 64;
    
    uint8_t data[data_size];
    
    
    int i;
    for(i=0; i<64; i++){
        data[i] = '0'+i;
    }
    
    // Address
    wsec_no = 0; init_addr = 0;
    
    // To write data into memory, unlock block protection REG.
    UNLOCK_PROTECTION();
    
    // Erase memory array before writing
    SECTOR_ERASE(wsec_no, true);
    
    // initialization for writing
    Writing_Initialize(wsec_no, init_addr, data);
    
    //WRITE_ENABLE();
}

bool TEST_WRITE_READ(void){
    
    uint16_t wsec_no, init_addr;
    bool flag_run, TestResult;
    
    int data_size;
    data_size = 64;
    
    uint8_t rdata[256], Rx;
    
    // Write data into memory
    flag_run = true;
    WRITE_ENABLE_NoWait();
    
    // Address
    wsec_no = 0; init_addr = 0;    
    //PAGE_PROGRAM_256(wsec_no, init_addr, wdata);
    
    PAGE_PROGRAM_NoWait_V2();
    //PAGE_PROGRAM_NoWait();
    
    while(flag_run){
        if(MEM_SPI_State == SPI2_EXCHANGE){
            if(IFS2bits.SPI2IF){ 
                IFS2bits.SPI2IF = 0;
                MEM_SPI_State = SPI2_IDLE;
                Rx = SPI2BUF;
                
                if(State_PP == ISBUSY_PP)
                    status = Rx;
                PAGE_PROGRAM_NoWait_V2();
                
                //PAGE_PROGRAM_NoWait();
            }
        }
        else{
            flag_run = false;
        }
    }
    
    //IS_BUSY_NoWait();  
    
    
    // Starting address for reading
    uint32_t addr;
    addr = (wsec_no << 12) | init_addr;
    READ_MEM_256(addr, rdata);
    
    TestResult = true;
    int i;
    for(i=0; i<data_size; i++){
        TestResult &= (rdata[i] == wdata[i]);
    }
    
    return TestResult;
}

void JEDECID_NoWait(void){
    
    switch(State_ID){
        
        case(CMD_ID):
            IFS2bits.SPI2IF = 0;
            CS2_SetLow();
            spi2_exchangeByte_NoWait(RDJEDECID);
            State_ID = DUMMY1_ID;
            break;
            
        case(DUMMY1_ID):
            spi2_exchangeByte_NoWait(NOP);
            State_ID = DUMMY2_ID;
            break;
            
        case(DUMMY2_ID):
            spi2_exchangeByte_NoWait(NOP);
            State_ID = DUMMY3_ID;
            break;
            
        case(DUMMY3_ID):
            spi2_exchangeByte_NoWait(NOP);
            State_ID = END_ID;
            break;
            
        case(END_ID):
            CS2_SetHigh();
            State_ID = CMD_ID;
            break;         
    }
}

bool TEST_COMM_NoWait(void){
    
    uint8_t Rx_buf[4];
    bool TestResult, flag_run;
    int i;
    
    i = 0;
    flag_run = true;
    
    IFS2bits.SPI2IF = 0;
    JEDECID_NoWait();
       
    while(flag_run){
        if(MEM_SPI_State == SPI2_EXCHANGE){
            if(IFS2bits.SPI2IF){ 
                IFS2bits.SPI2IF = 0;
                MEM_SPI_State = SPI2_IDLE;
                //Need to read receive buffer such that the hardware clear SPIRBF bit
                //or clear the receive overflow flag bit manually to make it work.
                Rx_buf[i] = SPI2BUF;
                i++;
                JEDECID_NoWait();                 
            }
        }
        else{
            flag_run = false;
        }
    }
    
    TestResult = (Rx_buf[1] == 0xBF);
    TestResult &= (Rx_buf[2] == 0x26);
    TestResult &= (Rx_buf[3] == 0x41);
    
    return TestResult;
}