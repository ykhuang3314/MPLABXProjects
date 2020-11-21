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

void _put(char *pt);
void write_byte(uint8_t *pt, int length);
void process_message(void);

#ifdef	__cplusplus
}
#endif

#endif	/* COMM_H */

