#include "mcc_generated_files/mcc.h"
#include "Intan_RHD2132.h"
#include "SST26VF016B.h"
#include "Measurement.h"
#include <string.h>


void _put(char *pt)
{
    uint8_t index;
    index = 0;
    while(pt[index]){
        while(!UART1_IsTxReady());
        UART1_Write(pt[index++]);    
    }
}


void process_message(void)
{
    uint8_t message;
    message = UART1_Read();

    
    switch(message){
        
        //verified
        case 't': // test UART
            _put("pass\n");
            break;
        
        // verified
        case 'r': // test spi communication with INTAN chip
            if(Intan_SPI_NoWait_Test())
                _put("pass\n");
            else
                _put("fail\n");
            break;
            
        // verified    
        case 'i': // Intan Initialization including REG configuration and ADC calibration
            if(Intan_Initialization(1000))
                _put("done\n");
            else
                _put("fail\n");
            break;
        
        // verified    
        case 'm': // test the communication between memory and pic

            if(TEST_COMM_MEM())
                _put("pass\n");
            else
                _put("fail\n");
            break;
        
        // verified, but not working with the use of dynamic memory allocation  
        case 'x':
            Test_write_initialize();
            break;
            
        case 'd': // test storing data into the flash memory and fetching data from the flash memory    
           
            if(TEST_WRITE_READ())
                _put("pass\n");
            else
                _put("fail\n");    
            break;
        
        case 'e':
            // Unlock the memory and erase the chip
            UNLOCK_PROTECTION();
            CHIP_ERASE(true);
            _put("done\n");
            break;
            
        // Testing for converting 32 channel   
        case 'c': //converting data
            Intan_Convert_NoWait_Test();
            break;
        
        case 'p': //print out data stored in memory
            PRINT_MEM(0, 1, true);
            break;
        
        // multi-channel measurement
        case 's':

            break;
        
            
        default:
            break;
    }
}

