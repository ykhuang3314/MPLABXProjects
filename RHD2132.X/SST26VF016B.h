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

//Mask for address
#define SECTOR_MASK 0xFFFFF000 // one sector contains 4kbyte     
#define ADDRESS_MASK 0x00000FFF // the address within a sector
    

typedef enum{
    IDLE_PP,
    ADDRESS0_PP,
    ADDRESS1_PP,
    ADDRESS2_PP,        
    WRITE_PP,
    END_PP, 
}SST26VF016B_PP;


extern volatile SST26VF016B_PP State_PP;
extern int cnt_data;
extern uint8_t wdata[6]; 
extern uint8_t Write_Addr[3];


uint8_t READ_STATUS_REG(void);
void JEDECID(uint8_t *data);
bool IS_BUSY(void);
bool IS_WRITEABLE(void);
void WRITE_ENABLE(void);
void WRITE_DISABLE(void);
//void READ_MEM(uint32_t addr, uint8_t *data, uint16_t n);
void READ_MEM_256(uint32_t addr, uint8_t *data);
//void PAGE_PROGRAM(uint16_t sec_no, uint16_t addr, uint8_t *data, uint16_t n);
void PAGE_PROGRAM_256(uint16_t sec_no, uint16_t addr, uint8_t *data);
void LOCK_PROTECTION(void);
void UNLOCK_PROTECTION(void);
void SECTOR_ERASE (uint16_t sec_no, bool flagwait);
void CHIP_ERASE (bool flagwait);

void WRITE_ENABLE_NoWait(void);
void Writing_State_Initialize(void);
void Writing_Initialize(uint16_t sec_no, uint16_t addr, uint8_t *data);
void PAGE_PROGRAM_NoWait(void);

// functions for testing purpose
bool TEST_COMM_MEM (void); 
bool TEST_WRITE_READ(void);
void READ_MEM_TEST(uint32_t addr, uint8_t *data);


#ifdef	__cplusplus
}
#endif

#endif	/* SST26VF016B_H */

