#include "mcc_generated_files/mcc.h"
#include <string.h>
#include <stdio.h>
#define FCY _XTAL_FREQ/2
#include "libpic30.h"
#include "Intan_RHD2132.h"
#include "SPI_nowait.h"

volatile RHD2132_ReadReg State_RR;
uint16_t Cmd_Conv;
uint16_t Cmd_Read;
uint16_t start_channel;
uint16_t current_channel;
uint16_t end_channel;

uint16_t Intan_ReadREG(uint16_t addr)
{
    uint16_t CMD;    
    uint16_t Tx_buf[3];
    
    CMD = READ_CMD | (addr<<8);
    
    Tx_buf[0] = CMD;
    Tx_buf[1] = Dummy_CMD;
    Tx_buf[2] = Dummy_CMD;
    
    uint16_t Rx_buf[3];
    int i; 
    for(i=0; i<3; i++){
        CS1_SetLow();
        Rx_buf[i] = SPI1_Exchange16bit(Tx_buf[i]);
        CS1_SetHigh();
    }
    return Rx_buf[2];
}

void Intan_State_Initialize(void){
    
    State_RR = IDLE_RR;
}

void Intan_ReadREG_NoWait(void){
    
    switch(State_RR){
        
        case(IDLE_RR):
            IFS0bits.SPI1IF = 0;
            SetReadCMD(40);
            CS1_SetLow();
            SPI1_Exchange16bit_NoWait(Cmd_Read);
            State_RR = READ41;
            break;
    
        case(READ41):
            SetReadCMD(41);
            CS1_SetLow();
            SPI1_Exchange16bit_NoWait(Cmd_Read);
            State_RR = READ42;
            break;
        
        case(READ42):
            SetReadCMD(42);
            CS1_SetLow();
            SPI1_Exchange16bit_NoWait(Cmd_Read);
            State_RR = READ43;
            break;
        
        case(READ43):
            SetReadCMD(43);
            CS1_SetLow();
            SPI1_Exchange16bit_NoWait(Cmd_Read);
            State_RR = READ44;
            break;
            
        case(READ44):
            SetReadCMD(44);
            CS1_SetLow();
            SPI1_Exchange16bit_NoWait(Cmd_Read);
            State_RR = DUMMY1;
            break;    
        
        case(DUMMY1):
            CS1_SetLow();
            SPI1_Exchange16bit_NoWait(Dummy_CMD);
            State_RR = DUMMY2;
            break;
        
        case(DUMMY2):
            CS1_SetLow();
            SPI1_Exchange16bit_NoWait(Dummy_CMD);
            State_RR = END_RR;
            break;
        
        case(END_RR):
            State_RR = IDLE_RR;
            INTAN_SPI_State = SPI1_IDLE;
            break;
            
        default:
            break;
    } 
}

bool Intan_SPI_NoWait_Test(void){
    
    uint16_t Rx_buf[7];
    bool TestResult, flag_run;
    int i;
    
    i = 0;
    flag_run = true;
    
    Intan_ReadREG_NoWait();
       
    while(flag_run){
        if(INTAN_SPI_State == SPI1_EXCHANGE){
            if(IFS0bits.SPI1IF){ 
                IFS0bits.SPI1IF = 0;
                CS1_SetHigh();
                INTAN_SPI_State = SPI1_IDLE;
                //Need to read receive buffer such that the hardware clear SPIRBF bit
                //or clear the receive overflow flag bit manually to make it work.
                Rx_buf[i] = SPI1BUF;
                i++;
                Intan_ReadREG_NoWait();                 
            }
        }
        else{
            flag_run = false;
        }
    }
    
    TestResult = ((Rx_buf[2] & 0x00FF) == 0x0049);
    TestResult &= ((Rx_buf[3] & 0x00FF) == 0x004E);
    TestResult &= ((Rx_buf[4] & 0x00FF) == 0x0054);
    TestResult &= ((Rx_buf[5] & 0x00FF) == 0x0041);
    TestResult &= ((Rx_buf[6] & 0x00FF) == 0x004E);
    
    return TestResult;
}

bool Intan_SPI_Test(void)
{
    //Read REG40-44 to verify the fidelity of the SPI interface
    //REG40-44 contains INTAN in ASCII
    uint16_t addr;
    uint16_t Rx_buf[7], CMD;
    bool TestResult;
    
    addr = 40;
    int i;
    for(i=0; i<7; i++)
    {
        if (i>4)
            CMD = Dummy_CMD;
        else
            CMD = READ_CMD | (addr<<8);
        
        CS1_SetLow();
        Rx_buf[i] = SPI1_Exchange16bit(CMD);
        CS1_SetHigh();
        addr++;
    }

    TestResult = ((Rx_buf[2] & 0x00FF) == 0x0049);
    TestResult &= ((Rx_buf[3] & 0x00FF) == 0x004E);
    TestResult &= ((Rx_buf[4] & 0x00FF) == 0x0054);
    TestResult &= ((Rx_buf[5] & 0x00FF) == 0x0041);
    TestResult &= ((Rx_buf[6] & 0x00FF) == 0x004E);
    
    return TestResult;
    
}

bool Intan_WriteREG(uint16_t addr, uint16_t data)
{
    //Write data to the register
    if(addr < 0 || addr > 63)
    {
        printf("Register address out of range(0-63). \n");
        return false;
    }
    if(data < 0 || data > 255)
    {
        printf("Register data out of range(0-255). \n");
        return false;
    }
    uint16_t CMD;
    CMD = WRITE_CMD | (addr << 8) | data;
    uint16_t Tx_buf[3];
    Tx_buf[0] = CMD;
    Tx_buf[1] = Dummy_CMD;
    Tx_buf[2] = Dummy_CMD;
    uint16_t Rx_buf[3];
    int i; 
    for(i=0; i<3; i++){
        CS1_SetLow();
        Rx_buf[i] = SPI1_Exchange16bit(Tx_buf[i]);
        CS1_SetHigh();
    }
    if((Rx_buf[2] & 0x00FF) == data)
        return true;
    else
        return false;        
}

void SetConvertCMD(uint16_t channel){
    
    Cmd_Conv = CONVERT_CMD | (channel << 8);
}

void SetReadCMD(uint16_t REG_Addr){
    
    Cmd_Read = READ_CMD | (REG_Addr<<8);
}

void Intan_ADC_Calibrate(void)
{
    //initiate ADC calibration routine
    uint16_t tmp, Tx_buf[15]; 
    UNUSED(tmp);
    
    Tx_buf[0] = CALIBRATE_CMD;
    int i;
    for(i = 1; i < 15; i++)
    {
        Tx_buf[i] = Dummy_CMD; // at least nine dummy CMD need to be sent after a CALIBRATE CMD 
    }
    
    for(i=0; i<15; i++)
    {    
        //__delay_us(1);
        CS1_SetLow();
        tmp = SPI1_Exchange16bit(Tx_buf[i]);
        CS1_SetHigh();
    }    
}

void Intan_ADC_Clear(void)
{ 
    uint16_t tmp; UNUSED(tmp);
    CS1_SetLow();
    tmp = SPI1_Exchange16bit(CLEAR_CMD);
    CS1_SetHigh();
}


void Intan_REG_Initialization(double fSCLK, uint16_t *REG_data)
{
    // RHD2000 Register 0 variables
    int adcReferenceBw;
    int ampFastSettle;
    int ampVrefEnable;
    int adcComparatorBias;
    int adcComparatorSelect;

    // RHD2000 Register 1 variables
    int vddSenseEnable;
    int adcBufferBias;

    // RHD2000 Register 2 variables
    int muxBias;

    // RHD2000 Register 3 variables
    int muxLoad;
    int tempS1;
    int tempS2;
    int tempEn;
    int digOutHiZ;
    int digOut;

    // RHD2000 Register 4 variables
    int weakMiso;
    int twosComp;
    int absMode;
    int dspEn;
    int dspCutoffFreq;

    // RHD2000 Register 5 variables
    int zcheckDacPower;
    int zcheckLoad;
    int zcheckScale;
    int zcheckConnAll;
    int zcheckSelPol;
    int zcheckEn;

    // RHD2000 Register 6 variables
    int zcheckDac = 0;     

    // RHD2000 Register 7 variables
    int zcheckSelect;

    // RHD2000 Register 8-13 variables
    int offChipRH1;
    int offChipRH2;
    int offChipRL;
    int adcAux1En;
    int adcAux2En;
    int adcAux3En;
    int rH1Dac1;
    int rH1Dac2;
    int rH2Dac1;
    int rH2Dac2;
    int rLDac1;
    int rLDac2;
    int rLDac3;

    // RHD2000 Register 14-17 variables
    int aPwr[32];
    
    //Define sampling frequency(kS/sec) and corresponding parameter 
    double sampleRate = fSCLK/16;
    

    if (sampleRate < 120) {
        muxBias = 40;
        adcBufferBias = 32;
    } else if (sampleRate < 140) {
        muxBias = 40;
        adcBufferBias = 16;
    } else if (sampleRate < 175) {
        muxBias = 40;
        adcBufferBias = 8;
    } else if (sampleRate < 220) {
        muxBias = 32;
        adcBufferBias = 8;
    } else if (sampleRate < 280) {
        muxBias = 26;
        adcBufferBias = 8;
    } else if (sampleRate < 350) {
        muxBias = 18;
        adcBufferBias = 4;
    } else if (sampleRate < 440) {
        muxBias = 16;
        adcBufferBias = 3;
    } else if (sampleRate < 525) {
        muxBias = 7;
        adcBufferBias = 3;
    } else {
        muxBias = 4;
        adcBufferBias = 2;
    }  
    
    // Set default values for all register settings
    adcReferenceBw = 3;         // ADC reference generator bandwidth (0 [highest BW] - 3 [lowest BW]);
                                // always set to 3
    ampFastSettle = 0;          // amplifier fast settle (off = normal operation)
    ampVrefEnable = 1;          // enable amplifier voltage references (0 = power down; 1 = enable);
                                // 1 = normal operation
    adcComparatorBias = 3;      // ADC comparator preamp bias current (0 [lowest] - 3 [highest], only
                                // valid for comparator select = 2,3); always set to 3
    adcComparatorSelect = 2;    // ADC comparator select; always set to 2

    vddSenseEnable = 1;         // supply voltage sensor enable (0 = disable; 1 = enable)
    
    muxLoad = 0;                // configure the total capaciance at the input of the ADC
                                // always be set to 0
    
    tempS1 = 0;                 // temperature sensor S1 (0-1); 0 = power saving mode when temperature sensor is
                                // not in use
    tempS2 = 0;                 // temperature sensor S2 (0-1); 0 = power saving mode when temperature sensor is
                                // not in use
    tempEn = 0;                 // temperature sensor enable (0 = disable; 1 = enable)
    digOut = 0;                 // auxiliary digital output state
    digOutHiZ = 0;
    
    weakMiso = 1;               // weak MISO (0 = MISO line is HiZ when CS is inactive; 1 = MISO line is weakly
                                // driven when CS is inactive)
    twosComp = 0;               // two's complement ADC results (0 = unsigned offset representation; 1 = signed
                                // representation)
    absMode = 0;                // absolute value mode (0 = normal output; 1 = output passed through abs(x) function)
    dspEn = 1;                  // DSP offset removal enable/disable
    dspCutoffFreq = 1;          // DSP offset removal HPF cutoff frequency

    zcheckDacPower = 0;         // impedance testing DAC power-up (0 = power down; 1 = power up)
    zcheckLoad = 0;             // impedance testing dummy load (0 = normal operation; 1 = insert 60 pF to ground)
    zcheckScale = 0;            // impedance testing scale factor (100 fF, 1.0 pF, or 10.0 pF)
    zcheckConnAll = 0;          // impedance testing connect all (0 = normal operation; 1 = connect all electrodes together)
    zcheckSelPol = 0;           // impedance testing polarity select (RHD2216 only) (0 = test positive inputs;
                                // 1 = test negative inputs)
    zcheckEn = 0;               // impedance testing enable/disable

    zcheckSelect = 0;           // impedance testing amplifier select (0-63)

    offChipRH1 = 0;             // bandwidth resistor RH1 on/off chip (0 = on chip; 1 = off chip)
    offChipRH2 = 0;             // bandwidth resistor RH2 on/off chip (0 = on chip; 1 = off chip)
    offChipRL = 0;              // bandwidth resistor RL on/off chip (0 = on chip; 1 = off chip)
    adcAux1En = 1;              // enable ADC aux1 input (when RH1 is on chip) (0 = disable; 1 = enable)
    adcAux2En = 1;              // enable ADC aux2 input (when RH2 is on chip) (0 = disable; 1 = enable)
    adcAux3En = 1;              // enable ADC aux3 input (when RL is on chip) (0 = disable; 1 = enable)
    
    
    // upper bandwidth 
    rH1Dac1 = 46; rH1Dac2 = 2; rH2Dac1 = 30; rH2Dac2 = 3; // set upper bandwidth of amplifiers to be 1kHz
    //rH1Dac1 = 30; rH1Dac2 = 5; rH2Dac1 = 43; rH2Dac2 = 6; // set upper bandwidth of amplifiers to be 500Hz
    
    // lower bandwidth
    rLDac1 = 25; rLDac2 = 0; rLDac3 = 0;                  // set lower bandwidth of amplifiers to be 100Hz
    //rLDac1 = 44; rLDac2 = 6; rLDac3 = 0;                  // set lower bandwidth of amplifiers to be 1Hz
    
    int i;
    for(i = 0; i < 32; i++)
    {
        aPwr[i] = 1;            // turn on all amplifiers 
    } 
    
    //Create data word to be written into REG0 to REG17
    uint16_t Tx_buf[18], tmp, CMD; 
    UNUSED(tmp);
    Tx_buf[0] = (adcReferenceBw << 6) | (ampFastSettle << 5) | (ampVrefEnable << 4) | (adcComparatorBias << 2) | adcComparatorSelect;
    Tx_buf[1] = (vddSenseEnable << 6) | adcBufferBias;
    Tx_buf[2] = muxBias;
    Tx_buf[3] = (muxLoad << 5) | (tempS2 << 4) | (tempS1 << 3) | (tempEn << 2) | (digOutHiZ << 1) | digOut;
    Tx_buf[4] = (weakMiso << 7) | (twosComp << 6) | (absMode << 5) | (dspEn << 4) | dspCutoffFreq;
    Tx_buf[5] = (zcheckDacPower << 6) | (zcheckLoad << 5) | (zcheckScale << 3) | (zcheckConnAll << 2) | (zcheckSelPol << 1) | zcheckEn;       
    Tx_buf[6] = zcheckDac;       
    Tx_buf[7] = zcheckSelect;
    Tx_buf[8] = (offChipRH1 << 7) | rH1Dac1;
    Tx_buf[9] = (adcAux1En << 7) | rH1Dac2;
    Tx_buf[10] = (offChipRH2 << 7) | rH2Dac1;
    Tx_buf[11] = (adcAux2En << 7) | rH2Dac2;
    Tx_buf[12] = (offChipRL << 7) | rLDac1;
    Tx_buf[13] = (adcAux3En << 7) | (rLDac3 << 6) | rLDac2;
    Tx_buf[14] = (aPwr[7] << 7) | (aPwr[6] << 6) | (aPwr[5] << 5) | (aPwr[4] << 4) | (aPwr[3] << 3) | (aPwr[2] << 2) | (aPwr[1] << 1) | aPwr[0];
    Tx_buf[15] = (aPwr[15] << 7) | (aPwr[14] << 6) | (aPwr[13] << 5) | (aPwr[12] << 4) | (aPwr[11] << 3) | (aPwr[10] << 2) | (aPwr[9] << 1) | aPwr[8];
    Tx_buf[16] = (aPwr[23] << 7) | (aPwr[22] << 6) | (aPwr[21] << 5) | (aPwr[20] << 4) | (aPwr[19] << 3) | (aPwr[18] << 2) | (aPwr[17] << 1) | aPwr[16];
    Tx_buf[17] = (aPwr[31] << 7) | (aPwr[30] << 6) | (aPwr[29] << 5) | (aPwr[28] << 4) | (aPwr[27] << 3) | (aPwr[26] << 2) | (aPwr[25] << 1) | aPwr[24];      
    
    memcpy(REG_data, &Tx_buf[0], 36);
    //Writing data to registers
    for(i = 0; i < 18; i++)
    {   
        CMD = WRITE_CMD | (i << 8) | Tx_buf[i];
        CS1_SetLow();
        tmp = SPI1_Exchange16bit(CMD);
        CS1_SetHigh();
    }
}

bool Intan_Initialization(double fSCLK)
{
    uint16_t data[18], ReadResult[18];
    
    Intan_REG_Initialization(fSCLK, data);
    //__delay_ms(1);
    //Intan_ADC_Calibrate();
    
    bool TestResult;
    TestResult = true;
    
    int i;
    for(i=0; i<18; i++){
        ReadResult[i] = Intan_ReadREG(i);
        
    }
    for(i=0; i<18; i++){
        TestResult &= ((ReadResult[i] & 0x00FF) == (data[i] & 0x00FF));
    }
    return TestResult;
}


void Intan_Convert_Initialize(uint16_t start, uint16_t end){
    
    start_channel = start;
    current_channel = start_channel;
    end_channel = end;
}

void Intan_Convert_Multi(void){

    IFS0bits.SPI1IF = 0;
    SetConvertCMD(current_channel);
    CS1_SetLow();
    SPI1_Exchange16bit_NoWait(Cmd_Conv);
    current_channel++;
    if(current_channel > end_channel)
        current_channel = start_channel;
        
}


void Intan_Convert_NoWait_Test(uint16_t start_ch, uint16_t end_ch){

    uint16_t Rx_buf[32];
    bool flag_run;
    int i; 
    
    flag_run = true;
    i = 0;
    
    Intan_Convert_Initialize(start_ch, end_ch);
    //Intan_CONVERT32();
    Intan_Convert_Multi();
    
    while(flag_run){
        if(INTAN_SPI_State == SPI1_EXCHANGE){
            if(IFS0bits.SPI1IF){ 
                IFS0bits.SPI1IF = 0;
                CS1_SetHigh();
                INTAN_SPI_State = SPI1_IDLE;
                //Need to read receive buffer such that the hardware clear SPIRBF bit
                //or clear the receive overflow flag bit manually to make it work.
                Rx_buf[i] = SPI1BUF;
                i++;
                if(i < 32)
                    //Intan_CONVERT32();
                    Intan_Convert_Multi();
                else
                    INTAN_SPI_State = SPI1_IDLE;
            }
        }
        else{
            flag_run = false;
        }
    }
    
}

