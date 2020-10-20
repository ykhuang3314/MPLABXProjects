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
   
void DataExchange16bitTest(uint16_t data);
uint16_t Intan_ReadREG(uint16_t addr);
bool Intan_WriteREG(uint16_t addr, uint16_t data);
uint16_t Intan_Convert_Single(uint16_t channel);
void Intan_Convert_32(uint16_t *result);
void Intan_ADC_Calibrate(void);
void Intan_ADC_Clear(void);
void Intan_REG_Initialization(double fSCLK, uint16_t *REG_data);
bool Intan_Initialization(double fSCLK);
bool Intan_SPI_Test(void);
void Intan_Meas_Single(uint16_t channel, uint16_t sec_no, uint16_t init_addr, bool flag);

#ifdef	__cplusplus
}
#endif

#endif	/* INTAN_RHD2132_H */

