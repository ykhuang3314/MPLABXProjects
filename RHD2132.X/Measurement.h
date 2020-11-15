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
void PRINT_MEM(uint16_t start_sec, uint16_t end_sec, bool flag);
void Measurement_Multi32(bool flag);

#ifdef	__cplusplus
}
#endif

#endif	/* MEASUREMENT_H */

