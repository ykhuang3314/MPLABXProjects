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
    
void DataExchange16bitTest(uint16_t data);
uint16_t Intan_ReadREG(uint16_t addr);
bool Intan_WriteREG(uint16_t addr, uint16_t data);
uint16_t Intan_Convert_Single(uint16_t channel);
void Intan_ADC_Calibrate(void);
void Intan_ADC_Clear(void);
void Intan_REG_Initialization(double fSCLK, uint16_t *REG_data);
bool Intan_Initialization(double fSCLK);
bool Intan_SPI_Test(void);


#ifdef	__cplusplus
}
#endif

#endif	/* INTAN_RHD2132_H */

