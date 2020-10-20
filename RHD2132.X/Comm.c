#include "mcc_generated_files/mcc.h"
#include "Intan_RHD2132.h"
#include "SST26VF016B.h"
#include "SST26VF016B.h"


void _put(char *pt)
{
    uint8_t index;
    index = 0;
    while(pt[index]){
        while(!UART1_IsTxReady());
        UART1_Write(pt[index++]);    
    }
}

void write(char *pt, uint8_t length){
    int i; 
    for(i=0; i<length; i++)
    {
        while(!UART1_IsTxReady());
        UART1_Write(pt[i]);
    }
}

void process_message(void)
{
    uint8_t message;
    message = UART1_Read();
    uint8_t rdata[6];
    
    switch(message){
        
        //verified
        case 't': // test UART
            _put("pass \n");
            break;
        
        // verified
        case 'r': // test spi communication with INTAN chip
            if(Intan_SPI_Test())
                _put("pass \n");
            else
                _put("fail \n");
            break;
        
        /* function verified
        case 'w': //test spi by writing REG0
            
            if(Intan_WriteREG(0, 0xDE))
                _put("pass \n");
            else
                _put("fail \n");
            break;
        */
        
            
        // verified    
        case 'i': // Intan Initialization including REG configuration and ADC calibration
            if(Intan_Initialization(250))
                _put("pass \n");
            else
                _put("fail \n");
            break;
        
        // verified    
        case 'm': // test the communication between memory and pic

            if(TEST_COMM_MEM())
                _put("pass \n");
            else
                _put("fail \n");
            break;
        
        // verified, but not working with the use of dynamic memory allocation  
        
        case 'd': // test storing data into the flash memory and fetching data from the flash memory    
              
            if(TEST_WRITE_READ())
                _put("pass \n");
            else
                _put("fail \n");    
            break;
        
        //Test for erasing memory and reading data from memory    
        // verified
        /*
        case 'C':
            UNLOCK_PROTECTION();
            SECTOR_ERASE(0, true);
            break;
        
        case 'R':
            READ_MEM_TEST(0, rdata);
            if(rdata[0] == 0xFF)
                _put("pass \n");
            else
                _put("fail \n"); 
            break;       
        */
        case 'c': //converting data
            break;
        
        default:
            break;
    }
}

