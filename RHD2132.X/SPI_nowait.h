/* 
 * File:   SPI_nowait.h
 * Author: ykhuang
 *
 * Created on October 30, 2020, 9:57 AM
 */

#ifndef SPI_NOWAIT_H
#define	SPI_NOWAIT_H

#ifdef	__cplusplus
extern "C" {
#endif

#include "mcc_generated_files/mcc.h"
    
typedef enum {
    SPI2_IDLE,
    SPI2_EXCHANGE
} SPI2_State;    

typedef enum {
    SPI1_IDLE,
    SPI1_EXCHANGE
} SPI1_State;   

extern volatile SPI2_State MEM_SPI_State;
extern volatile SPI1_State INTAN_SPI_State;

// SPI2
void spi2_NoWait_Initialize(void);
void spi2_exchangeByte_NoWait(uint8_t data);

// SPI1
void SPI1_NoWait_Initialize(void);
void SPI1_Exchange16bit_NoWait(uint16_t data);

#ifdef	__cplusplus
}
#endif

#endif	/* SPI_NOWAIT_H */

