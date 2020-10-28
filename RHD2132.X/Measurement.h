/* 
 * File:   Measurement.h
 * Author: ykhuang
 *
 * Created on October 21, 2020, 3:02 PM
 */

#ifndef MEASUREMENT_H
#define	MEASUREMENT_H

#ifdef	__cplusplus
extern "C" {
#endif

void CONVERT_16_to_8(uint16_t *data_16b, uint8_t *data_8b, int num_element);
void Intan_Meas_Single(uint16_t channel, uint16_t sec_no, uint16_t init_addr, uint16_t end_sector, bool flag);
void Data_Print(uint8_t Din_MSB, uint8_t Din_LSB, char *Result);
void PRINT_MEM(uint16_t start_sec, uint16_t end_sec, bool flag);
void Intan_Meas_Multi(bool flag);
void Intan_Meas_Multi_V2(int no_sec, bool flag);

#ifdef	__cplusplus
}
#endif

#endif	/* MEASUREMENT_H */

