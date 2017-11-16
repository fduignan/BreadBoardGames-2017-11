#ifndef __CONSOLE_H
#define __CONSOLE_H

#include <stdint.h>
#include "font5x7.h"
#include "STM32F103.h"

/*
 * August 2017 : Refactored.  Including display and keypad in the same class: Console
 * This puts all of the hardware specific code (apart from the CPU initialization) into
 * one place which may help porting.
 *
 * August 2017: New I/O for STM32F103
 * Pins use may seem random however it simplifies the wiring greatly
 * DB0 : A11
 * DB1 : A10
 * DB2 : A9
 * DB3 : A8
 * DB4 : B15
 * DB5 : B14
 * DB6 : B13
 * DB7 : B12
 * RST : A15
 * RD  : B3
 * WR  : B4
 * RS  : B5
 * CS  : B6 
 * 
 * Piezo buzzer : B0
 */

// Assumption: Top left of display is 0,0
// Screen is 240 wide and 320 high
#define SCREEN_WIDTH 176
#define SCREEN_HEIGHT 220
// Define a macro to allow easy definition of colours
// Format of colour value: <BGND 1 bit><Red 5 bits><Green 5 bits><Blue 5 bits>
#define RGBToWord( R,  G,  B)  (  ((R&0xf8) << (11-3)) | ((G&0xfc) << (5-2)) | ((B&0xf8)>>3) )
// Define some primary colours
#define BGND RGBToWord(0,0,0)
#define BLACK RGBToWord(0,0,0)
#define RED RGBToWord(0xff,0,0)
#define GRN RGBToWord(0,0xff,0)
#define BLUE RGBToWord(0x1f,0x1f,0xff)
#define WHI RGBToWord(0xff,0xff,0xff)
#define YEL RGBToWord(0xff,0xff,0x00)
void delay(volatile uint32_t dly);
void registerMillisecondCallback(void *tick);   


class Console {
public:
    // It would have been nice to initialize the display in the contructor but this doesn't seem
    // to work.  A "begin" function must be called during setup to start the display up properly    
    void begin();    
    void putPixel(uint16_t x, uint16_t y, uint16_t colour);
    void putImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t *Image);
    void fillRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t Colour);
    void putText(const char *Text, uint16_t len, uint16_t x, uint16_t y, uint16_t ForeColour, uint16_t BackColour);
    void putNumber(uint16_t Number, uint16_t x, uint16_t y, uint16_t ForeColour, uint16_t BackColour);    
    
    int LeftPressed();
    int RightPressed();
    int FirePressed();
    void playTone(uint32_t Frequency, uint32_t ms);
    void stopTone();
    int  SoundPlaying();
    void playMelody(const uint32_t *Tones,const uint32_t *Times,int Len);
    void sleep(uint32_t ms);
    uint32_t random(uint32_t lower,uint32_t upper);    
    friend void onMillisecond();
private:
    volatile uint32_t uptime;
    volatile uint32_t milliseconds;    
    volatile uint32_t tone_time;    
    void initDisplay(void);
    void setupButtons();
    void initSound();
    void openAperture(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2);
    uint32_t prbs();
    void output_byte(uint8_t b);
    void Write_Command(uint16_t cmd);
    void Write_Data(uint16_t data);
    void Write_Command_Data(uint16_t cmd, uint16_t dat);    
    inline void WRLow()
    {
        GPIOB->BRR = BIT4; 
    }
    inline void WRHigh()
    {
        GPIOB->BSRR = BIT4;
    }
    inline void CommandMode()
    {    
        GPIOB->BRR = BIT5;
    }
    inline void DataMode()
    {
        GPIOB->BSRR = BIT5;        
    }
    inline void CSLow()
    {
        GPIOB->BRR = BIT6;
    }
    inline void CSHigh()
    {
        GPIOB->BSRR = BIT6;
    }
    inline void RDLow()
    {
        GPIOB->BRR = BIT3;
    }
    inline void RDHigh()
    {
        GPIOB->BSRR = BIT3;
    }
    inline void RSTLow()
    {
        GPIOA->BRR = BIT15;
    }
    inline void RSTHigh()
    {
        GPIOA->BSRR = BIT15;
    }
    

};
extern Console console;
#endif

