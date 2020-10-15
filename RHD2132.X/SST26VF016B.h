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

// Device Operation Instructions for SST26VF016B  
// Configuration    
#define RDSR 0x05 // Read Status REG
#define WRSR 0x01 // Write Status REG      
#define RDCR 0x35 // Read Config. REG
#define NOP 0x00 // No operation    
// Read    
#define READ 0x03 // Read Memory up to 40MHz
#define READ_FAST 0x0B // Read Memory at high speed up to 104MHz    
// Identification 
#define RDJEDECID 0x9F // JEDEC-ID: Return BF 26 41
// Write    
#define WREN 0x06 // Write Enable
#define WRDI 0x04 // Write Disable
#define PP 0x02 // Page Program: write one byte or a page of up to 256 bytes)
// Erase
#define SE 0x20 // Erase 4KByte of memory array (one sector)
#define BE 0xD8 // Erase 32KByte of memory array (one block)    
#define CE 0xC7 // Erase full array   

// Protection
#define LBPR 0x8D
#define ULBPR 0x98    
    
// Mask for Status REG
#define isBUSY_MASK 0x01 // internal write operation in progress 
#define isWRITEALBE_MASK 0x02 // device is write-enabled 
#define isERASESUSPEND_MASH 0x04 // erase suspended    
#define isPROGRAMSUSPEND_MASH 0x08 // program suspended

uint8_t READ_STATUS_REG(void);
void JEDECID(uint8_t *data);
bool IS_BUSY(void);
bool IS_WRITEABLE(void);
bool WRITE_ENABLE(void);
bool WRITE_DISABLE(void);
void READ_MEM(uint32_t addr, uint8_t *data, uint16_t n);
bool PAGE_PROGRAM(uint16_t sec_no, uint16_t addr, uint8_t* data, uint16_t n);
void LOCK_PROTECTION(void);
void UNLOCK_PROTECTION(void);
void SECTOR_ERASE (uint16_t sec_no, bool flagwait);
//void BLOCK_ERASE (uint16_t block_addr, bool flagwait);
void CHIP_ERASE (bool flagwait);

#ifdef	__cplusplus
}
#endif

#endif	/* SST26VF016B_H */
