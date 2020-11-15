#include "mcc_generated_files/mcc.h"
#include <string.h>
#include <stdio.h>
#define FCY _XTAL_FREQ/2
#include "libpic30.h"
#include "Comm.h"
#include "SST26VF016B.h"
#include "Intan_RHD2132.h"
#include "SPI_nowait.h"

void CONVERT_16_to_8(uint16_t *data_16b, uint8_t *data_8b, int num_element){
    
    // received data from Intan RHD2132 is 16bit; while, the spi flash memory is 8bit.
    // conversion is required before dumping the data into memory
    
    
    /*
    int num_element;
    num_element = sizeof(data_16b)/sizeof(data_16[0]);
    */
    
    int i;
    for(i=0; i<num_element; i++){
        
        // Divide the 16bit data into 2 8bit data (MSB first)
        data_8b[2*i] = (data_16b[i] >> 8) & 0xFF;   //MSB
        data_8b[2*i+1] = data_16b[i] & 0xFF;        //LSB 
    }
}


void PRINT_MEM(uint16_t start_sector, uint16_t end_sector, bool flag){
    
    int i, j, data_size, no_ch;
    data_size = 256;
    no_ch = 32;
    
    uint8_t rdata[data_size];
    uint8_t display[no_ch];
    
    uint32_t start_addr, end_addr;
    start_addr = start_sector;
    start_addr <<= 12;
    end_addr = end_sector;
    end_addr <<= 12;
    
    while(flag){
        if(start_addr < end_addr){
            
            READ_MEM_256(start_addr, rdata);
            
            for(i=0; i<(data_size/no_ch); i++){
                // Display data on COM via UART
                for(j=0; j<no_ch; j++){
                    display[j] = rdata[i*no_ch+j];
                }
                _put((char*)display);
                _put("\n");
            }
            start_addr += data_size;
        }
        else{
            flag = false;
            //_put("done\n");
        }
    }
}

void Measurement_Multi32(uint16_t no_sec, bool flag){
    
    int datasize;
    datasize = 32;
    
    uint16_t result_16bit[datasize];
    uint8_t result_8bit[2*datasize], dummy;
    
    int i;
    bool flag_intan, flag_mem, flag_run;
    i = 0;
    flag_run = flag;
    flag_intan = true;
    
    Intan_CONVERT32();
       
    while(flag_intan){
        if(INTAN_SPI_State == SPI1_EXCHANGE){
            if(IFS0bits.SPI1IF){ 
                IFS0bits.SPI1IF = 0;
                CS1_SetHigh();
                INTAN_SPI_State = SPI1_IDLE;
                //Need to read receive buffer such that the hardware clear SPIRBF bit
                //or clear the receive overflow flag bit manually to make it work.
                result_16bit[i] = SPI1BUF;
                i++;
                if(i < datasize)
                    Intan_CONVERT32();
                else{
                    INTAN_SPI_State = SPI1_IDLE;
                    i = 0;
                }
            }
        }
        else{
            flag_intan = false;
        }
    }
    
    CONVERT_16_to_8(result_16bit, result_8bit, datasize);
    
    
    uint16_t addr, sec;
    addr = 0;
    sec = 0;
    
    while(flag_run){
        
        flag_intan = true;
        flag_mem = true;  
        
        WRITE_ENABLE_NoWait();
        Intan_CONVERT32();
        PAGE_PROGRAM_NoWait();
        
        Writing_Initialize(sec, addr, result_8bit);
       
        while(flag_intan || flag_mem){         
            
            if(INTAN_SPI_State == SPI1_EXCHANGE){
                if(IFS0bits.SPI1IF){ 
                    IFS0bits.SPI1IF = 0;
                    CS1_SetHigh();
                    INTAN_SPI_State = SPI1_IDLE;
                    //Need to read receive buffer such that the hardware clear SPIRBF bit
                    //or clear the receive overflow flag bit manually to make it work.
                    result_16bit[i] = SPI1BUF;
                    i++;
                    if(i == datasize){
                        i = 0;
                        INTAN_SPI_State = SPI1_IDLE;
                    }
                    else
                        Intan_CONVERT32();
                }
            }
            else{
                flag_intan = false;
            }
            
            if(MEM_SPI_State == SPI2_EXCHANGE){
                if(IFS2bits.SPI2IF){ 
                    IFS2bits.SPI2IF = 0;    
                    MEM_SPI_State = SPI2_IDLE;
                    //Need to read receive buffer such that the hardware clear SPIRBF bit                
                    dummy = SPI2BUF;                    
                    PAGE_PROGRAM_NoWait();                 
                }
            }
            else{
                flag_mem = false;
            }
        }
        
        CONVERT_16_to_8(result_16bit, result_8bit, datasize);
        
        addr += 2*datasize;
        if(addr == 4096){
            addr = 0;
            sec +=1;
            if(sec > no_sec){
                _put("done\n");
                flag_run = false;
            }
        }    
        
    }
    
}