// BreadBoardGames : Retro arcade gaming on an MCU.
// For construction details and update please visit http://ioprog.com/bbg
// This program is relased under the GNU Public License Version 2 a copy of which should be included with
// this collection of software
// Changelog:
// October 2017: Modified to work with the LDDP4524 in preparation for BreadBoardGames in Wexford
// August 2017: Refactored: Console related code placed into a single class that handles display and buttons
// August 2017: Ported to STM32F103C8T6 "blue pill" boards from aliexpress.
// July 2017: Changed game to suit the more common ST7775 176x220 displays.  The number of sprites on screen was reduced.
// July 2017: Added 4 levels to both games and code so invaders can shoot back (Thanks to David for testing)
// July 2017 : Edited energia-1.6.10E18/hardware/energia/msp430/cores/msp430/wiring.c to force speed to 8MHz
// This allows the CPU to work at much lower battery voltages.
// July 2017 : replaced digitalWrite function calls in Display.h
// that controlled the WR and CS lines with direct port writes:
// MUCH MUCH faster
// April 2017: Modified to work with the S6D1121 TFT display
// Lots more pins used as it is a parallel interface display
// Low level I/O routines and display initialization rewritten

#include <stdint.h>
#include "../include/cortexm3.h"
#include "Sprite.h"
#include "console.h"
#include "images.h"
#include "serial.h"

void PlayInvaders();
void PlayBrici();
void Bunclody17();

void Bunclody17()
{        
    // Experimental coding area
    console.fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_WIDTH, 0);    
    // Insert your code here
    
    
    console.putText("Fire to restart", 15, 8, 120, RGBToWord(0xff, 0xff, 0), RGBToWord(0, 0, 0));
    while (!console.FirePressed())
        console.sleep(100);
}

#define STARTUP_MELODY_NOTES 10
const uint32_t StartupMelodyTones[STARTUP_MELODY_NOTES]={1000,500,1000,500,1000,500,1000,500,1000,500};
const uint32_t StartupMelodyTimes[STARTUP_MELODY_NOTES]={200,200,200,200,200,200,200,200,200,200};
void setup()
{
  //initUART(9600);   // Open a UART on pins PA2(TX) and PA3(RX) for debugging if needed : see serial.h
  console.begin();     
}
uint8_t GameStarted = 0;
void loop()
{
  GameStarted = 0;
  console.fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  console.putText("Bread", 5, SCREEN_WIDTH/3, 50, RGBToWord(0xff, 0xff, 0x0), 0);
  console.putText("Board", 5, SCREEN_WIDTH/3, 70, RGBToWord(0xff, 0xff, 0x0), 0);
  console.putText("Games!", 5, SCREEN_WIDTH/3, 90, RGBToWord(0xff, 0xff, 0x0), 0);
  console.putText("Left for Brici", 14, 5, 120, RGBToWord(0xff, 0x3f, 0x3f), 0);
  console.putText("Right for Galaga", 16, 5, 140, RGBToWord(0x1f, 0xff, 0x1f), 0);
  console.putText("Fire for Bunclody17", 19, 5, 160, RGBToWord(0x3f, 0x3f, 0xff), 0);
  console.putText("web:ioprog.com/bbg", 18, 5, 200, RGBToWord(0xff, 0xff, 0xff), 0);
  console.playMelody(StartupMelodyTones, StartupMelodyTimes,STARTUP_MELODY_NOTES);
  while (GameStarted == 0)
  {
    if (console.LeftPressed())
    {
      GameStarted = 1;
      PlayBrici();
    }
    if (console.RightPressed())
    {
      GameStarted = 1;
      PlayInvaders();
    }
    if (console.FirePressed())
    {
      GameStarted = 1;
      Bunclody17();
    }
    console.random(0,100); // cycle the random number generator
    console.sleep(100);
  }
}



