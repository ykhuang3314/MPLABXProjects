#include "mcc_generated_files/mcc.h"
#include "Intan_RHD2132.h"
#include "SST26VF016B.h"
#include "Measurement.h"
#include <string.h>

int NumSec;

void _put(char *pt)
{
    uint8_t index;
    index = 0;
    while(pt[index]){
        UART1_Write(pt[index++]);    
    }
    
    while(!UART1_IsTxDone());
}

void write_byte(uint8_t *pt, int length)
{
    int index;
    for(index=0; index<length; index++){
        UART1_Write(pt[index]);    
    }
    while(!UART1_IsTxDone());
}

void process_message(void)
{
    uint8_t message;
    message = UART1_Read();
    
    
    switch(message){
        
        case 'c':     
            
            //SPT1 Test
            _put("SPI1:");          
            
            if(Intan_SPI_Test())
                _put("pass.");
            else
                _put("fail.");
            
            
            //SPI2 Test
            _put("SPI2:");
            
            if(TEST_COMM_MEM())
                _put("pass.");
            else
                _put("fail.");

            break;
        
                        
        case 't': // test UART
            _put("UART:");
            _put("pass.");
            break;
          
        case 'i': // Intan Initialization including REG configuration and ADC calibration
            if(Intan_Initialization(500))
                _put("done.");
            else
                _put("fail.");
            NumSec = 16;
            break; 
        
        case 'C':
            Intan_ADC_Calibrate();
            break;
        
        case 'x':
            Test_write_initialize();
            break;
            
        case 'd': // test storing data into the flash memory and fetching data from the flash memory    
           
            if(TEST_WRITE_READ())
                _put("pass.");
            else
                _put("fail.");    
            break;
        
        case 'e':
            // Unlock the memory and erase the chip
            UNLOCK_PROTECTION();
            CHIP_ERASE(true);
            _put("done.");
            break;
            
        // Testing for converting   
        case 's': //converting data
            
            Intan_Convert_NoWait_Test(0, 10);
            break;
        
        case 'p': //print out data stored in memory
            PRINT_MEM(0, NumSec, true);
            break;     
        
        case 'm': // Single-Channel Measurement
            //void Measurement(uint16_t start_ch, uint16_t end_ch, uint16_t no_sec, bool flag)
            _put("Meas.");
            Measurement(0, 0, NumSec, true);
            break;
        
        case 'M': // Multi-Channel Measurement
            _put("Meas.");
            Measurement(0, 3, NumSec, true);
            break;
        
        case '1':
            _put("Meas.");
            Measurement(0, 0, NumSec, true);
            break;
        case '2':
            _put("Meas.");
            Measurement(0, 1, NumSec, true);
            break;
        case '3':
            _put("Meas.");
            Measurement(0, 2, NumSec, true);
            break;
        case '4':
            _put("Meas.");
            Measurement(0, 3, NumSec, true);
            break;
        case '5':
            _put("Meas.");
            Measurement(0, 4, NumSec, true);
            break;
        case '6':
            _put("Meas.");
            Measurement(0, 5, NumSec, true);
            break;
        case '7':
            _put("Meas.");
            Measurement(0, 6, NumSec, true);
            break;
        case '8':
            _put("Meas.");
            Measurement(0, 7, NumSec, true);
            break;            
        case 'f':
            _put("Meas.");
            Measurement(0, 15, NumSec, true);           
            break;
        case 'a':
            _put("Meas.");
            Measurement(0, 31, NumSec, true);
            break;
        
        case 'v':
            _put("VDD.");
            Measurement(48, 48, NumSec, true);
            break;
            
        case 'A':
            _put("Aux1.");
            Measurement(32, 32, NumSec, true);
            break;
            
        default:
            break;
    }
}

