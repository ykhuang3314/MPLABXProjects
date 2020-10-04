#include "mcc_generated_files/mcc.h"
#include "Intan_RHD2132.h"

void _put(uint8_t *pt)
{
    unsigned char index;
    index = 0;
    while(pt[index]){
        while(!UART1_IsTxReady());
        UART1_Write(pt[index++]);    
    }
}

void write(uint8_t *pt, uint8_t length){
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
    uint16_t IntanChar[5];
    bool TestResult;
    
    message = UART1_Read();
    
    switch(message){
        
        case 't': //test uart
            _put("ok \n");
            break;
        
        case 'r': //test spi by reading REG
            Intan_SPI_Test(IntanChar);
            TestResult = ((IntanChar[0]|0x00FF) == 73) && ((IntanChar[1]|0x00FF) == 78) && ((IntanChar[2]|0x00FF) == 84) && ((IntanChar[3]|0x00FF) == 65) && ((IntanChar[4]|0x00FF) == 78);
            if(TestResult)
                _put("success \n");
            else
                _put("fail \n");
            break;
        
        case 'w': //test spi by writing REG0
            TestResult = Intan_WriteREG(0, 0xDE);
            if(TestResult)
                _put("success \n");
            else
                _put("fail \n");
            break;
        
        case 'i': //Initialization
            Intan_Initialization(250);
            break;
        
        case 'c': //converting data
            break;
        
        default:
            break;
    }
}