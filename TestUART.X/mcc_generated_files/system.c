/**
  @Generated PIC24 / dsPIC33 / PIC32MM MCUs Source File

  @Company:
    Microchip Technology Inc.

  @File Name:
    system.h

  @Summary:
    This is the sysetm.h file generated using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description:
    This header file provides implementations for driver APIs for all modules selected in the GUI.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.168.0
        Device            :  PIC24FV16KA302
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.50
        MPLAB             :  MPLAB X v5.40
*/

/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

// Configuration bits: selected in the GUI

// FBS
#pragma config BWRP = OFF    //Boot Segment Write Protect->Disabled
#pragma config BSS = OFF    //Boot segment Protect->No boot program flash segment

// FGS
#pragma config GWRP = OFF    //General Segment Write Protect->General segment may be written
#pragma config GSS0 = OFF    //General Segment Code Protect->No Protection

// FOSCSEL
#pragma config FNOSC = FRC    //Oscillator Select->Fast RC Oscillator (FRC)
#pragma config SOSCSRC = ANA    //SOSC Source Type->Analog Mode for use with crystal
#pragma config LPRCSEL = HP    //LPRC Oscillator Power and Accuracy->High Power, High Accuracy Mode
#pragma config IESO = OFF    //Internal External Switch Over bit->Internal External Switchover mode disabled (Two-speed Start-up disabled)

// FOSC
#pragma config POSCMOD = NONE    //Primary Oscillator Configuration bits->Primary oscillator disabled
#pragma config OSCIOFNC = ON    //CLKO Enable Configuration bit->CLKO output signal is active on the OSCO pin
#pragma config POSCFREQ = HS    //Primary Oscillator Frequency Range Configuration bits->Primary oscillator/external clock input frequency greater than 8MHz
#pragma config SOSCSEL = SOSCHP    //SOSC Power Selection Configuration bits->Secondary Oscillator configured for high-power operation
#pragma config FCKSM = CSDCMD    //Clock Switching and Monitor Selection->Both Clock Switching and Fail-safe Clock Monitor are disabled

// FWDT
#pragma config WDTPS = PS32768    //Watchdog Timer Postscale Select bits->1:32768
#pragma config FWPSA = PR128    //WDT Prescaler bit->WDT prescaler ratio of 1:128
#pragma config FWDTEN = OFF    //Watchdog Timer Enable bits->WDT disabled in hardware; SWDTEN bit disabled
#pragma config WINDIS = OFF    //Windowed Watchdog Timer Disable bit->Standard WDT selected(windowed WDT disabled)

// FPOR
#pragma config BOREN = BOR3    //Brown-out Reset Enable bits->Brown-out Reset enabled in hardware, SBOREN bit disabled
#pragma config LVRCFG = OFF    //Low Voltage Regulator Configuration bit->Low Voltage regulator is not available
#pragma config PWRTEN = ON    //Power-up Timer Enable bit->PWRT enabled
#pragma config I2C1SEL = PRI    //Alternate I2C1 Pin Mapping bit->Use Default SCL1/SDA1 Pins For I2C1
#pragma config BORV = V20    //Brown-out Reset Voltage bits->Brown-out Reset set to lowest voltage (2.0V)
#pragma config MCLRE = ON    //MCLR Pin Enable bit->RA5 input pin disabled,MCLR pin enabled

// FICD
#pragma config ICS = PGx1    //ICD Pin Placement Select bits->EMUC/EMUD share PGC1/PGD1

// FDS
#pragma config DSWDTPS = DSWDTPSF    //Deep Sleep Watchdog Timer Postscale Select bits->1:2,147,483,648 (25.7 Days)
#pragma config DSWDTOSC = LPRC    //DSWDT Reference Clock Select bit->DSWDT uses Low Power RC Oscillator (LPRC)
#pragma config DSBOREN = ON    //Deep Sleep Zero-Power BOR Enable bit->Deep Sleep BOR enabled in Deep Sleep
#pragma config DSWDTEN = ON    //Deep Sleep Watchdog Timer Enable bit->DSWDT enabled

#include "pin_manager.h"
#include "clock.h"
#include "system.h"
#include "interrupt_manager.h"
#include "traps.h"
#include "uart1.h"

void SYSTEM_Initialize(void)
{
    PIN_MANAGER_Initialize();
    INTERRUPT_Initialize();
    CLOCK_Initialize();
    UART1_Initialize();
}

/**
 End of File
*/