/* 
 * File:   SST26VF016B.h
 * Author: ykhuang
 *
 * Created on October 5, 2020, 1:45 PM
 */

#ifndef SST26VF016B_H
#define	SST26VF016B_H

#ifdef	__cplusplus
extern "C" {
#endif

// Device Operation Instructions
#define RDSR 0x05 // Read Status REG
#define WRSR 0x01 // Write Status REG
#define RDCR 0x35 // Read Config. REG
#define READ 0x03 // Read Memory
#define RDJEDECID 0x9F // JEDEC-ID
#define WREN 0x06 // Write Enable
#define WRDI 0x04 // Write Disable
#define PP 0x02 // Page Program
    
// Status register mask
#define isBUSY_MASK 0x01
#define isWRITEALBE_MASK 0x02 
     

uint8_t READ_STATUS_REG(void);
void JEDECID(uint8_t *data);
bool IS_BUSY(void);
bool IS_WRITEABLE(void);
bool WRITE_ENABLE(void);
bool WRITE_DISABLE(void);
void READ_MEM(uint32_t addr, uint8_t *data, uint16_t n);
bool PAGE_PROGRAM(uint16_t sec_no, uint16_t addr, uint8_t* data, uint16_t n);
    
#ifdef	__cplusplus
}
#endif

#endif	/* SST26VF016B_H */

