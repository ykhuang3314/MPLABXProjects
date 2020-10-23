#include "mcc_generated_files/mcc.h"
#include <string.h>
#include <stdio.h>
#define FCY _XTAL_FREQ/2
#include "libpic30.h"
#include "Comm.h"
#include "SST26VF016B.h"
#include "Intan_RHD2132.h"

void CONVERT_16_to_8(uint16_t *data_16b, uint8_t *data_8b, int num_element){
    
    /*
    int num_element;
    num_element = sizeof(data_16b)/sizeof(data_16[0]);
    */
    
    int i;
    for(i=0; i<num_element; i++){
        
        //Divide the 16bit data into 2 8bit data (MSB first)
        data_8b[2*i] = (data_16b[i] >> 8) & 0xFF;
        data_8b[2*i+1] = data_16b[i] & 0xFF; 
    }
}


void Intan_Meas_Single(uint16_t channel, uint16_t sec_no, uint16_t init_addr, uint16_t end_sector, bool flag){
    
    uint16_t CMD;
    CMD = CONVERT_CMD | (channel << 8);
    
    int data_size, count, count_mem;
    data_size = 256; // maximum allowable bytes for single page program
    uint16_t Rx_buf[data_size/2], dummy;
    uint8_t wdata[data_size];
            
    count = 0; 
    count_mem = 0;
    
    // Unlock the memory and erase the chip
    UNLOCK_PROTECTION();
    CHIP_ERASE(true);
    
    while(flag){
        
        //ignore the received data in first two cycle
        if(count < 2){
            CS1_SetLow();
            dummy = SPI1_Exchange16bit(CMD);
            CS2_SetHigh();
            count++;
        }
        else{
            if(count_mem < data_size/2){
                CS1_SetLow();
                Rx_buf[count_mem] = SPI1_Exchange16bit(CMD);
                CS1_SetHigh();
                count_mem++;              
            }
            else{
                count_mem = 0;
                //unlock block protection reg to write data
                UNLOCK_PROTECTION();
                //convert 16-bit data array to 8-bit data array (MSB first)
                CONVERT_16_to_8(Rx_buf, wdata, data_size/2);
                //dump the data into memory once 256byte data is received
                PAGE_PROGRAM_256(sec_no, init_addr, wdata);
                //increase starting address
                init_addr += 256;
                //once the starting address exceed 4K, move onto next memory sector
                if(init_addr > 4096){
                    init_addr = 0;
                    sec_no += 1;
                }
                if(sec_no > end_sector || sec_no > 512) // 512 4Kbyte sector are available for storing data 
                    flag = false;
            }
        }
    }
}

void Data_Print(uint8_t Din_MSB, uint8_t Din_LSB, unsigned short ADC_Vref, char *Result){
    
    // 16bit ADC value
    uint16_t Data;
    Data = Din_MSB;
    Data <<= 8;
    Data += Din_LSB;
    
    // D/A conversion. Unit: uV
    unsigned long long tmp;
    unsigned long value, tmp2; 
    
    tmp = (unsigned long long)(ADC_Vref*1000)*Data; 
    // 16bit resolution
    value = (unsigned long)(tmp >> 16);
    
    // Convert value into char string with 3 decimals (unit: mV; range: 0.000 ~ 1200.000)
    Result[4] = '.';
    int i;
    for (i=7; i>=0; i--){
        if(i!=4){
            tmp2 = value%10;
            value /= 10;
            Result[i] = (char)tmp2 + '0';
        }
    }
}

void PRINT_MEM(uint16_t start_sector, uint16_t end_sector, bool flag){
    
    int i, data_size = 256;
    uint8_t rdata[256];
    
    uint32_t start_addr, end_addr;
    start_addr = start_sector;
    start_addr <<= 12;
    end_addr = end_sector;
    end_addr <<= 12;
    
    char Result[8];
    
    while(flag){
        if(start_addr <= end_addr){
            READ_MEM_256(start_addr, rdata);
            for(i=0; i<data_size; i+=2){
                // Display data on COM via UART
                Data_Print(rdata[i], rdata[i+1], 1200, Result);
                _put(Result);
                _put("\n");
            }
            start_addr += 256;
        }
        else
            flag = false;        
    }
}


    