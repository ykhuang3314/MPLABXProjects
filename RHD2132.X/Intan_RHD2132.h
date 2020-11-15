/* 
 * File:   Intan_RHD2132.h
 * Author: ykhuang
 *
 * Created on September 4, 2020, 3:12 PM
 */

#ifndef INTAN_RHD2132_H
#define	INTAN_RHD2132_H

#ifdef	__cplusplus
extern "C" {
#endif

//Command 
#define Dummy_CMD       0x0000
#define READ_CMD        0xC000 
#define WRITE_CMD       0x8000
#define CONVERT_CMD     0x0000
#define CALIBRATE_CMD   0x5500
#define CLEAR_CMD       0x3A00

#define UNUSED(a) ((void)(a))    

typedef enum{
    CH0_Conv,
    CH1_Conv,        
    CH2_Conv,
    CH3_Conv,
    CH4_Conv,
    CH5_Conv,
    CH6_Conv,
    CH7_Conv,
    CH8_Conv,
    CH9_Conv,
    CH10_Conv,
    CH11_Conv,
    CH12_Conv,
    CH13_Conv,
    CH14_Conv,
    CH15_Conv,
    CH16_Conv,
    CH17_Conv,
    CH18_Conv,
    CH19_Conv,
    CH20_Conv,
    CH21_Conv,
    CH22_Conv,
    CH23_Conv,
    CH24_Conv,
    CH25_Conv,
    CH26_Conv,
    CH27_Conv,
    CH28_Conv,
    CH29_Conv,
    CH30_Conv,
    CH31_Conv,
    CH32_Conv,    
    END_Conv,
}RHD2132_CONVERT;

typedef enum{
    IDLE_RR,
    READ41,
    READ42,
    READ43,
    READ44,
    DUMMY1,
    DUMMY2,
    END_RR
}RHD2132_ReadReg;

extern volatile RHD2132_CONVERT State_Conv;
extern volatile RHD2132_ReadReg State_RR;
extern uint16_t Cmd_Conv;
extern uint16_t Cmd_Read;

uint16_t Intan_ReadREG(uint16_t addr);
bool Intan_WriteREG(uint16_t addr, uint16_t data);
void SetConvertCMD(uint16_t channel);
void Intan_ADC_Calibrate(void);
void Intan_ADC_Clear(void);
void Intan_REG_Initialization(double fSCLK, uint16_t *REG_data);
bool Intan_Initialization(double fSCLK);
void Intan_CONVERT32(void);

// Functions for testing purpose
//void DataExchange16bitTest(uint16_t data);
bool Intan_SPI_Test(void);
void SetReadCMD(uint16_t REG_Addr);
void Intan_ReadREG_NoWait(void);
bool Intan_SPI_NoWait_Test(void);
void Intan_State_Initialize(void);
void Intan_Convert_NoWait_Test(void);

#ifdef	__cplusplus
}
#endif

#endif	/* INTAN_RHD2132_H */

