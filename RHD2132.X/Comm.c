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

    switch(message){
        
        case 't': //test uart
            _put("pass \n");
            break;
        
        case 'r': //test spi by reading REG
            if(Intan_SPI_Test())
                _put("pass \n");
            else
                _put("fail \n");
            break;
        
        case 'w': //test spi by writing REG0
            
            if(Intan_WriteREG(0, 0xDE))
                _put("pass \n");
            else
                _put("fail \n");
            break;
        
        case 'i': //Initialization
            if(Intan_Initialization(250))
                _put("pass \n");
            else
                _put("fail \n");
            break;
        
        case 'm': // test the communication between memory and pic

            if(TEST_COMM_MEM())
                _put("pass \n");
            else
                _put("fail\n");
            break;
            
        case 'd': // test storing data into the flash memory and fetching data from the flash memory    
              
            if(TEST_WRITE_READ())
                _put("pass \n");
            else
                _put("fail \n");    
            break;
        
        
        case 'c': //converting data
            break;
        
        default:
            break;
    }
}