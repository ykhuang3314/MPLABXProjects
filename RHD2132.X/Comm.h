/* 
 * File:   Comm.h
 * Author: ykhuang
 *
 * Created on October 2, 2020, 3:12 PM
 */

#ifndef COMM_H
#define	COMM_H

#ifdef	__cplusplus
extern "C" {
#endif

void _put(uint8_t *pt);
void write(uint8_t *pt, uint8_t length);
void process_message(void);

#ifdef	__cplusplus
}
#endif

#endif	/* COMM_H */

