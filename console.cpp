// This file includes the C++ code needed to implement the game console.  The game console consists of
// the display, buttons, sound system and MCU timing system.  The display functions need to be rewritten
// for different types of display.  This version is for LGDP4524 220x176 displays

#ifndef __CONSOLE_CPP
#define __CONSOLE_CPP

#include <stdint.h>
#include "font5x7.h"
#include "../include/cortexm3.h"
#include "../include/STM32F103.h"
#include "console.h"

Console console;  // single global instance of the console
void onMillisecond(void); // global function that handles millisecond timing
void Console::begin() 
{

    // Low level I/O port initialisation
    // Turn on GPIO A and B
    RCC->APB2ENR |= BIT2 + BIT3;
    // Make all bits connected to the display outputs to start with
    // Note: PA15, PB4, PB3 are mapped to JTAG functionality at reset.    
    // Need to restore these to port operation first
    RCC->APB2ENR |= BIT0; // enable clock for AFIO latches
    AFIO->MAPR = 0x02000000; // turn off JTAG but leave SWD on    
    // Port A bits 8,9,10,11,15    
    GPIOA->CRH = (GPIOA->CRH & 0x0fff0000) | 0x10001111;
    // Port B bits 3,4,5,6,12,13,14,15
    GPIOB->CRL = (GPIOB->CRL & 0xf0000fff) | 0x01111000;
    GPIOB->CRH = (GPIOB->CRH & 0x0000ffff) | 0x11110000;
    
    
    // register the millisecond counter early on to allow the delay function work
    registerMillisecondCallback((void *)onMillisecond);
    
    // get the display started
    initDisplay();
    
    // black out the screen 
    fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0x00);
    setupButtons();
    // initialise the timing variables
    uptime = milliseconds = tone_time = 0;   
    initSound();
    
}
void Console::initDisplay()
{
    // The set of steps carried out here are specific to the type of display being used
    
    RDHigh();
    CSHigh();
    WRHigh();
    
    RSTLow();
    sleep(10);
    RSTHigh();
    sleep(100);
    RDHigh();
    CSHigh();
    WRHigh();
    sleep(100);
    CSLow();
    // Power control startup sequence (from manufacturer)
    sleep(150);
    //*********************Initial code******************************
    Write_Command_Data(0x0007, 0x0000);
    Write_Command_Data(0x0012, 0x0000);
    Write_Command_Data(0x0013, 0x0000);
    Write_Command_Data(0x0014, 0x0331);
    sleep(50);

    Write_Command_Data(0x0011, 0x0101);
    Write_Command_Data(0x0012, 0x000A);
    Write_Command_Data(0x0013, 0x0D45);
    Write_Command_Data(0x0010, 0x0164);
    Write_Command_Data(0x0012, 0x001A);
    sleep(50);
    Write_Command_Data(0x0013, 0x2D45);
    Write_Command_Data(0x0010, 0x0160);
    sleep(50);
    Write_Command_Data(0x0060, 0x2000);
    Write_Command_Data(0x0060, 0x0000);
    Write_Command_Data(0x0061, 0x0002);
    sleep(50);
    Write_Command_Data(0x0001, 0x011B);
    Write_Command_Data(0x0002, 0x0700);
    Write_Command_Data(0x0003, 0x1030);
    Write_Command_Data(0x0008, 0x0504);
    Write_Command_Data(0x0009, 0x0000);
    Write_Command_Data(0x000B, 0x0C30);
    Write_Command_Data(0x0021, 0x0000);
    Write_Command_Data(0x0030, 0x0606);
    Write_Command_Data(0x0031, 0x0606);
    Write_Command_Data(0x0032, 0x0606);
    Write_Command_Data(0x0033, 0x0803);
    Write_Command_Data(0x0034, 0x0202);
    Write_Command_Data(0x0035, 0x0404);
    Write_Command_Data(0x0036, 0x0404);
    Write_Command_Data(0x0037, 0x0404);
    Write_Command_Data(0x0038, 0x0402);
    Write_Command_Data(0x0039, 0x100C);
    Write_Command_Data(0x003A, 0x0101);
    Write_Command_Data(0x003B, 0x0101);
    Write_Command_Data(0x003C, 0x0101);
    Write_Command_Data(0x003D, 0x0101);
    Write_Command_Data(0x003E, 0x0001);
    Write_Command_Data(0x003F, 0x0001);
    Write_Command_Data(0x0042, 0xDB00);
    Write_Command_Data(0x0044, 0xAF00);
    Write_Command_Data(0x0045, 0xDB00);
    sleep(50);
    Write_Command_Data(0x0071, 0x0040);
    Write_Command_Data(0x0072, 0x0002);
    sleep(50);
    Write_Command_Data(0x0010, 0x6060);
    Write_Command_Data(0x0007, 0x0005);
    sleep(50);
    Write_Command_Data(0x0007, 0x0025);
    Write_Command_Data(0x0007, 0x0027);
    sleep(50);
    Write_Command_Data(0x0007, 0x0037);
    Write_Command_Data(0x0021, 0x0000);

    Write_Command(0x22);
}
void Console::putPixel(uint16_t x, uint16_t y, uint16_t colour)
{
    // This function opens a 1x1 drawing aperture on the display and the specified colour into it.  
    openAperture(x, y, x + 1, y + 1);
    Write_Data(colour);
}
void Console::putImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t *Image)
{
    // This function opens a drawing aperture on the display and writes the bytes from the supplied
    // image into it
    uint16_t Colour;
    openAperture(x, y, x + width - 1, y + height - 1);
    for (y = 0; y < height; y++)
    {
        for (x = 0; x < width; x++)
        {
            Colour = *(Image++);
            Write_Data(Colour);
        }
    }
}
void Console::fillRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t Colour)
{
    // This function opens a drawing aperture on the display and writes bytes of the specified colour
    // into it.  
    openAperture(x, y, x + width - 1, y + height - 1);
    for (y = 0; y < height; y++)
        for (x = 0; x < width; x++)
        {
            Write_Data(Colour);
        }
}
void Console::putText(const char *Text, uint16_t len, uint16_t x, uint16_t y, uint16_t ForeColour, uint16_t BackColour)
{
    // This function draws each character individually.  It uses an array called TextBox as a temporary storage
    // location to hold the dots for the character in question.  It constructs the image of the character and then
    // calls on putImage to place it on the screen
    uint8_t Index = 0;
    uint8_t Row, Col;
    const uint8_t *CharacterCode = 0;    
    uint16_t TextBox[FONT_WIDTH * FONT_HEIGHT];
    for (Index = 0; Index < len; Index++)
    {
        CharacterCode = &Font5x7[FONT_WIDTH * (Text[Index] - 32)];
        Col = 0;
        while (Col < FONT_WIDTH)
        {
            Row = 0;
            while (Row < FONT_HEIGHT)
            {
                if (CharacterCode[Col] & (1 << Row))
                {
                    TextBox[(Row * FONT_WIDTH) + Col] = ForeColour;
                }
                else
                {
                    TextBox[(Row * FONT_WIDTH) + Col] = BackColour;
                }
                Row++;
            }
            Col++;
        }
        putImage(x, y, FONT_WIDTH, FONT_HEIGHT, (const uint16_t *)TextBox);
        x = x + FONT_HEIGHT + 2;
    }
}
void Console::putNumber(uint16_t Number, uint16_t x, uint16_t y, uint16_t ForeColour, uint16_t BackColour)
{
    // This function converts the supplied number into a character string and then calls on puText to
    // write it to the display
    char Buffer[5]; // Maximum value = 65535
    Buffer[4] = Number % 10 + '0';
    Number = Number / 10;
    Buffer[3] = Number % 10 + '0';
    Number = Number / 10;
    Buffer[2] = Number % 10 + '0';
    Number = Number / 10;
    Buffer[1] = Number % 10 + '0';
    Number = Number / 10;
    Buffer[0] = Number % 10 + '0';
    putText(Buffer, 5, x, y, ForeColour, BackColour);
}


/* The databus pins for the display are connected to the microcontroller pins
 * in a mixed up order because it simplifies construction.  This mixing of the 
 * bit order needs to be fixed up in software.  This function handles this
// Bit reversal lookup table (16 entries or 4 bits)

/*
bit   rev
0000  0000
0001  1000
0010  0100
0011  1100
0100  0010
0101  1010
0110  0110
0111  1110
1000  0001
1001  1001
1010  0101
1011  1101
1100  0011
1101  1011
1110  0111
1111  1111
*/
const uint8_t reverse_bits[]={0,8,4,12,2,10,6,14,1,9,5,13,3,11,7,15 };

void Console::output_byte(uint8_t b)
{
    /*
        * DB0 : A11
        * DB1 : A10
        * DB2 : A9
        * DB3 : A8
        * DB4 : B15
        * DB5 : B14
        * DB6 : B13
        * DB7 : B12
    */
    
    GPIOA->BRR = BIT8+BIT9+BIT10+BIT11;
    GPIOB->BRR = BIT12+BIT13+BIT14+BIT15;    
    if (b)
    { // only write out bits if b is non zero to save time
        GPIOA->BSRR = ((uint32_t)reverse_bits[b & 0x0f]) << 8;
        GPIOB->BSRR = ((uint32_t)reverse_bits[b >> 4]) << 12;        
    }
}


void Console::Write_Command(uint16_t cmd)
{
    // Output a command to the display
    CommandMode();
    output_byte(cmd >> 8);
    WRLow();
    WRHigh();
    output_byte(cmd);
    WRLow();
    WRHigh();
}
void Console::Write_Data(uint16_t data)
{
    // Output a data word to the display
    DataMode();
    output_byte(data >> 8);
    WRLow();
    WRHigh();
    output_byte(data);
    WRLow();
    WRHigh();
}
void Console::Write_Command_Data(uint16_t cmd, uint16_t dat)
{
    // Output a command followed by data to the display
    Write_Command(cmd);
    Write_Data(dat);
}
void Console::openAperture(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2)
{
    // open up an area for drawing on the display
    Write_Command_Data(0x44, (x2<<8)+x1); 
    Write_Command_Data(0x45, (y2<<8)+y1); 
    Write_Command_Data(0x21, (y1<<8)+x1); 
    Write_Command(0x22);
}
void Console::setupButtons()
{
  // Button layout:
  // B1   : Left
  // B11  : Fire
  // P10  : Right
  // Set up the button pins as inputs with pull-ups
  GPIOB->CRL &=0xffffff0f;
  GPIOB->CRL |=0x00000080; 
  GPIOB->BSRR = BIT1; 
  
  GPIOB->CRH &=0xffff00ff;
  GPIOB->CRH |=0x00008800; 
  GPIOB->BSRR = BIT10+BIT11; 
  
  
  
}
int Console::LeftPressed()
{
// return 1 if the user pressed 'Left'
    if (GPIOB->IDR & BIT1)
        return 0;
    else
        return 1;
  
}
int Console::RightPressed()
{
    // return 1 if the user pressed 'Right'
    if (GPIOB->IDR & BIT10)
        return 0;
    else
        return 1;

}
int Console::FirePressed()
{
// return 1 if the user pressed 'Fire'
// Some debouncing of the button is performed    
    static int PreviousState = 0;
    if ((GPIOB->IDR & BIT11) == 0)
    {
        if (PreviousState == 0)
        {
            PreviousState = 1;
            return 1;
        }
        else
            return 0;

    }
    else
    {
        PreviousState = 0;
        return 0;
    }
}
void Console::initSound()
{
    // Output is a piezo speaker on B0 which is Timer 3, Channel 3
    // Turn on Timer 3
    RCC->APB1ENR |= BIT1; 
    // Ensure that Port B is enabled
    RCC->APB2ENR |= BIT3;
    // Set Timer 3 to default values
    TIM3->CR1 = 0; 
    // Enable PWM mode on channel 3
    TIM3->CCMR2_Output = BIT6+BIT5; 
    // Connect up Timer 3 output
    TIM3->CCER |= BIT8;
};
void Console::playTone(uint32_t Frequency, uint32_t ms)
{
    // Configure PB0 as Timer3 output
    GPIOB->CRL &=0xfffffff0;
    GPIOB->CRL |= BIT3+BIT1;
    // Will assume a 72MHz input frequency
    // The auto-reload register has a maximum value of 65536.  
    // This should map to the lowest frequency we would be interested in.
    // 72MHz/65536 = 1098Hz.  This is too high for a 'lowest' frequency so 
    // some pre-scaling of the input frequency is required.
    TIM3->CR1 &= ~BIT0; // disable the timer    
    TIM3->PSC = 72000000UL/65536UL;  // prescale so that the lowest frequency is 1Hz.
    TIM3->ARR = (72000000UL/(uint32_t)(TIM3->PSC))/(Frequency);
    TIM3->CCR3 = TIM3->ARR/2; // 50 % Duty cycle
    TIM3->CNT = 0;
    TIM3->CR1 |= BIT0; // enable the timer        
    tone_time = ms;    
}
void Console::stopTone()
{
    TIM3->CR1 &= ~BIT0; // disable the timer
    // Force the output low
     // Configure PB0 as a high impedance input (seems to save power)
    GPIOB->CRL &=0xfffffff0;
    GPIOB->CRL |= 4;
}
int Console::SoundPlaying()
{
    return (tone_time != 0);
}
void Console::playMelody(const uint32_t *Tones, const uint32_t *Times,int Len)
{
    int Index;
    for (Index = 0; Index < Len; Index++)
    {
        while(SoundPlaying()); // wait for previous note to complete
        sleep(10); // leave a slight gap between tones
        playTone(Tones[Index],Times[Index]);        
    }
}
void Console::sleep(uint32_t dly)
{
    if (dly)
    {   
        milliseconds = 0;
        prbs(); // Stir the random number pot
        while(milliseconds<dly)
            cpu_sleep(); // may as well sleep while waiting for millisecond to elapse (saves power)
    }  
}
uint32_t Console::random(uint32_t lower,uint32_t upper)
{
    return (prbs()%(upper-lower))+lower;
}
uint32_t Console::prbs()
{
	// This is an unverified 31 bit PRBS generator
	// It should be maximum length but this has not been verified 
	static unsigned long shift_register=0xa5a5a5a5;
	unsigned long new_bit=0;
	static int busy=0; // need to prevent re-entrancy here
	if (!busy)
	{
		busy=1;
		new_bit= ((shift_register & (1<<27))>>27) ^ ((shift_register & (1<<30))>>30);
		new_bit= ~new_bit;
		new_bit = new_bit & 1;
		shift_register=shift_register << 1;
		shift_register=shift_register | (new_bit);
		busy=0;
	}
	return shift_register & 0x7ffffff; // return 31 LSB's 
}

void onMillisecond()
{
    // This function is called every second by the hardware timing system
    // It's main job is to update the global timing variables (used for sleep functions)
    // It also maintains the tone generatoin system
    console.uptime++;
    console.milliseconds++;    
    // Interaction with the sound system    
    if (console.tone_time)
    {
        console.tone_time--;
        if (console.tone_time==0)
            console.stopTone();
    }    
}
#endif

