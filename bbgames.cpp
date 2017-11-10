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
void Explode(uint16_t X, uint16_t Y);

#define STARTUP_MELODY_NOTES 10
const uint32_t StartupMelodyTones[STARTUP_MELODY_NOTES]={1000,500,1000,500,1000,500,1000,500,1000,500};
const uint32_t StartupMelodyTimes[STARTUP_MELODY_NOTES]={200,200,200,200,200,200,200,200,200,200};
void setup()
{
  //initUART(9600);   // Open a UART on pins PA2(TX) and PA3(RX) for debugging
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
void Bunclody17()
{        
    // Experimental coding area
    console.fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_WIDTH, 0);
    console.putText("Fire to restart", 15, 8, 120, RGBToWord(0xff, 0xff, 0), RGBToWord(0, 0, 0));
    while (!console.FirePressed())
        console.sleep(100);
}
void PlayBrici()
{
#define MAX_BRICI_LEVELS 4
  int Level = MAX_BRICI_LEVELS;
  int LevelComplete = 0;
  int8_t BallCount = 5;
  uint8_t Index;
  int8_t BallXVelocity = 1;
  int8_t BallYVelocity = 1;
  // This game is pushing the limits of memory.  It seems to be about 1 or 2 bytes short of blowing the stack up. :o)
#define BW 24
#define BH 10
#define SCREEN220X176
#ifdef SCREEN320X240
  #define BLOCKCOUNT 30
  Sprite Blocks[BLOCKCOUNT] = {
    { YELLBLK, 0, 50, BW, BH }, { YELLBLK, 24, 50, BW, BH }, { YELLBLK, 48, 50, BW, BH }, { YELLBLK, 72, 50, BW, BH }, { YELLBLK, 96, 50, BW, BH }, { YELLBLK, 120, 50, BW, BH }, { YELLBLK, 144, 50, BW, BH }, { YELLBLK, 168, 50, BW, BH }, { YELLBLK, 192, 50, BW, BH }, { YELLBLK, 216, 50, 24, BH },
    { GRENBLK, 0, 60, BW, BH }, { GRENBLK, 24, 60, BW, BH }, { GRENBLK, 48, 60, BW, BH }, { GRENBLK, 72, 60, BW, BH }, { GRENBLK, 96, 60, BW, BH }, { GRENBLK, 120, 60, BW, BH }, { GRENBLK, 144, 60, BW, BH }, { GRENBLK, 168, 60, BW, BH }, { GRENBLK, 192, 60, BW, BH }, { GRENBLK, 216, 60, 24, BH },
    { BLUEBLK, 0, 70, BW, BH }, { BLUEBLK, 24, 70, BW, BH }, { BLUEBLK, 48, 70, BW, BH }, { BLUEBLK, 72, 70, BW, BH }, { BLUEBLK, 96, 70, BW, BH}, { BLUEBLK, 120, 70, BW, BH }, { BLUEBLK, 144, 70, BW, BH }, { BLUEBLK, 168, 70, BW, BH }, { BLUEBLK, 192, 70, BW, BH }, { BLUEBLK, 216, 70, 24, BH }
  };
#endif
#ifdef SCREEN220X176
#define BLOCKCOUNT 21
  Sprite Blocks[BLOCKCOUNT] = {
    { YELLBLK, 3+0, 20, BW, BH }, { YELLBLK, 3+24, 20, BW, BH }, { YELLBLK, 3+48, 20, BW, BH }, { YELLBLK, 3+72, 20, BW, BH }, { YELLBLK, 3+96, 20, BW, BH }, { YELLBLK, 3+120, 20, BW, BH }, { YELLBLK, 3+144, 20, BW, BH },
    { GRENBLK, 3+0, 30, BW, BH }, { GRENBLK, 3+24, 30, BW, BH }, { GRENBLK, 3+48, 30, BW, BH }, { GRENBLK, 3+72, 30, BW, BH }, { GRENBLK, 3+96, 30, BW, BH }, { GRENBLK, 3+120, 30, BW, BH }, { GRENBLK, 3+144, 30, BW, BH },
    { BLUEBLK, 3+0, 40, BW, BH }, { BLUEBLK, 3+24, 40, BW, BH }, { BLUEBLK, 3+48, 40, BW, BH }, { BLUEBLK, 3+72, 40, BW, BH }, { BLUEBLK, 3+96, 40, BW, BH}, { BLUEBLK, 3+120, 40, BW, BH }, { BLUEBLK, 3+144, 40, BW, BH }
  };
#endif   
  Sprite Bat(BAT, 0, SCREEN_HEIGHT-20, 20, 3);
  Sprite Ball(BALL, 120, SCREEN_HEIGHT/2, 3, 3);
  // put your setup code here, to run once:
  console.fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
  console.putText("Brici", 5, 10, 40, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
  console.sleep(500);
  while (Level > 0)
  {
    Ball.move(console.random(10, SCREEN_WIDTH-10), SCREEN_HEIGHT/2);
    if (console.random(0,2) > 0)
      BallXVelocity = 1;
    else
      BallXVelocity = -1;
    LevelComplete = 0;
    BallYVelocity = -1;
    console.fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_WIDTH, 0);
    for (Index = BallCount; Index > 0; Index--)
      console.fillRectangle(SCREEN_WIDTH - Index * 15, SCREEN_HEIGHT-10, 10, 10, RGBToWord(0xff, 0xf, 0xf));
    for (Index = 0; Index < BLOCKCOUNT; Index++)
    {
      Blocks[Index].hide();
      Blocks[Index].show();
    }
    Ball.show();
    Bat.show();
    console.putText("Level", 5, 5, SCREEN_HEIGHT-10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
    console.putNumber(MAX_BRICI_LEVELS - Level + 1, 60, SCREEN_HEIGHT-10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
    
    while (!LevelComplete)
    {
      if (console.RightPressed())
      {
        // Move right
        if (Bat.getX() < (SCREEN_WIDTH - Bat.getWidth()))
        {
          Bat.move(Bat.getX() + 2, Bat.getY()); // Move the bat faster than the ball
        }
      }

      if (console.LeftPressed())
      {
        // Move left
        if (Bat.getX() > 0)
        {
          Bat.move(Bat.getX() - 2, Bat.getY()); // Move the bat faster than the ball
        }
      }
      
      if (Bat.touching(Ball.getX() + Ball.getWidth() / 2, Ball.getY() + Ball.getHeight() / 2))
      { // Ball bounced off the bat
        BallYVelocity = -BallYVelocity; 
        console.playTone(200,20);
      }
      Bat.redraw(); // redraw bat as it might have lost a pixel due to collisions

      Ball.move(Ball.getX() + BallXVelocity, Ball.getY() + BallYVelocity); // update ball position
      if (Ball.getX() == 2) // bounced off left side?
      {
        BallXVelocity = -BallXVelocity;
        console.playTone(400,20);
      }
      if (Ball.getX() == SCREEN_WIDTH - 2) // bounced off right side?
      {
        BallXVelocity = -BallXVelocity;
        console.playTone(400,20);
      }
      if (Ball.getY() == 2)  // bounced off top? (behind the block)
      {
        BallYVelocity = -BallYVelocity;
        console.playTone(400,20);
      }

      if (Ball.getY() >= Bat.getY() + Bat.getHeight() + 2)  // Did the ball go behind the bat?
      {
        BallCount--;  // Lost a ball!
        if (BallCount == 0) // Any lefy?
        {
          // Nope: hard luck, game over
          console.fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
          console.putText("GAME OVER", 9, 40, 100, RGBToWord(0xff, 0xff, 0xff), 0);
          console.putText("Fire to restart", 15, 8, 120, RGBToWord(0xff, 0xff, 0), RGBToWord(0, 0, 0));
          while (!console.FirePressed())
              console.sleep(10);
          return;
        }
        // start a new ball moving in a random way
        if (console.random(0,100) & BIT0)
          BallXVelocity = 1;
        else
          BallXVelocity = -1;
        BallYVelocity = -1;
        Ball.move(console.random(10, SCREEN_WIDTH - 10), console.random(90, 120));
        console.fillRectangle(SCREEN_WIDTH-5*15, SCREEN_HEIGHT-10, 120, 10, 0);  // wipe out the remaining lives area
        for (Index = BallCount; Index > 0; Index--) //  draw remaining lives
          console.fillRectangle(SCREEN_WIDTH - Index * 15, SCREEN_HEIGHT-10, 10, 10, RGBToWord(0xff, 0xf, 0xf));

      }
      LevelComplete = 1;
      // Check to see if the ball has hit any of the blocks
      for (Index = 0; Index < BLOCKCOUNT; Index++)
      {
        if (Blocks[Index].visible()) // any blocks left?
          LevelComplete = 0; // If yes then the level is not complete
          
        int touch = Blocks[Index].touching(Ball.getX(), Ball.getY());
        if (touch)
        { // Block hit so hide it.
          Blocks[Index].hide();  
          if ( (touch == 1) || (touch == 3) )
            BallYVelocity = -BallYVelocity;
          if ( (touch == 2) || (touch == 4) )
            BallXVelocity = -BallXVelocity;
          console.playTone(1000,20);
        }
      }
      // No Blocks left, Move to next level.
      if ((LevelComplete == 1) && (Level > 0))
      {
        Level--;
        console.putText("Level", 5, 5, SCREEN_HEIGHT-10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
        console.putNumber(MAX_BRICI_LEVELS - Level + 1, 60, SCREEN_HEIGHT-10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));

      }
      console.sleep(Level+5); // Slow the game to human speed
    }
  }
  // All levels complete!
  console.fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_WIDTH, RGBToWord(0, 0, 0xff));
  console.putText("VICTORY!", 8, 40, 100, RGBToWord(0xff, 0xff, 0), RGBToWord(0, 0, 0xff));
  console.putText("Fire to restart", 15, 8, 120, RGBToWord(0xff, 0xff, 0), RGBToWord(0, 0, 0xff));
  while (!console.FirePressed())
      console.sleep(10);
  return;
}
void PlayInvaders()
{
  // Variables for the Invaders game
#define MAX_INVADERS 10
#define MAX_INVADER_LEVELS 4
  Sprite  Invaders[MAX_INVADERS] = {
    { InvaderImage, 3, 3, 10, 12   },
    { InvaderImage, 23, 3, 10, 12  },
    { InvaderImage, 43, 3, 10, 12  },
    { InvaderImage, 63, 3, 10, 12  },
    { InvaderImage, 83, 3, 10, 12  },
    { InvaderImage, 103, 3, 10, 12 },
    { InvaderImage, 123, 3, 10, 12 },
    { InvaderImage, 143, 3, 10, 12 },
    { InvaderImage, 163, 3, 10, 12 },
    { InvaderImage, 183, 3, 10, 12 }
  };
  Sprite  Defender(DefenderImage, SCREEN_WIDTH/2, SCREEN_HEIGHT-30, 10, 6);
#define MAX_DEFENDER_MISSILES 5
  Sprite DefenderMissiles[MAX_DEFENDER_MISSILES] = {
    { DefenderMissileImage, 0, 0, 5, 8  },
    { DefenderMissileImage, 0, 0, 5, 8  },
    { DefenderMissileImage, 0, 0, 5, 8  },
    { DefenderMissileImage, 0, 0, 5, 8  },
    { DefenderMissileImage, 0, 0, 5, 8  },
  };
#define MAX_ATTACKER_MISSILES (MAX_INVADERS)
  Sprite AttackerMissiles[MAX_ATTACKER_MISSILES] = {
    { AttackerMissileImage, 0, 0, 5, 8  },
    { AttackerMissileImage, 0, 0, 5, 8  },
    { AttackerMissileImage, 0, 0, 5, 8  },
    { AttackerMissileImage, 0, 0, 5, 8  },
    { AttackerMissileImage, 0, 0, 5, 8  },
    { AttackerMissileImage, 0, 0, 5, 8  },
    { AttackerMissileImage, 0, 0, 5, 8  },
    { AttackerMissileImage, 0, 0, 5, 8  },
    { AttackerMissileImage, 0, 0, 5, 8  },
    { AttackerMissileImage, 0, 0, 5, 8  }
  };
  uint8_t GameOver = 0;
  uint8_t DefenderMissileCounter = 0;
  uint8_t AttackerMissileCounter = 0;
  uint8_t InvaderCounter = 0;
  uint8_t DefenderCount = 5;
  int XDirectionChange = 0;
  int YDirectionChange = 0;
  int LevelComplete = 0;
  int Level = MAX_INVADER_LEVELS;
  uint16_t x_step = 2;
  uint16_t y_step = 1;
  uint8_t ActiveAttackerMissiles = 0;

  // Initialization for the Invaders game
  int Index;
  int Toggle = 1;

  console.fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_WIDTH, 0);
  console.putText("MCU Galaga", 10, 10, 40, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
  console.putText("Only 5 missiles", 15, 10, 60, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
  console.putText("can be in flight", 16, 10, 80, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
  console.putText("at one time", 11, 10, 100, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
  console.sleep(1000);
  while (Level > 0)
  {
    LevelComplete = 0;
    GameOver = 0;
    console.fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    for (Index = 0; Index < MAX_INVADERS-Level; Index++)
    {
      Invaders[Index].show();
    }     
    // Position the invaders at the top of the screen
    Invaders[0].move(0, 3);
    Invaders[1].move(15, 3);
    Invaders[2].move(30, 3);
    Invaders[3].move(45, 3);
    Invaders[4].move(60, 3);
    Invaders[5].move(75, 3);
    Invaders[6].move(90, 3);
    Invaders[7].move(105, 3);
    Invaders[8].move(120, 3);
    Invaders[9].move(135, 3);
    for (Index = 0; Index < MAX_ATTACKER_MISSILES;Index++)
      AttackerMissiles[Index].hide();    
    for (Index = 0; Index < MAX_DEFENDER_MISSILES;Index++)
      DefenderMissiles[Index].hide();          
    console.putText("Level", 5, 5, SCREEN_HEIGHT-10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
    console.putNumber(MAX_INVADER_LEVELS - Level + 1, 60, SCREEN_HEIGHT-10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));    
    for (Index = DefenderCount; Index > 0; Index--) //  draw remaining lives
        console.fillRectangle(SCREEN_WIDTH - Index * 15, SCREEN_HEIGHT-10, 10, 10, RGBToWord(0xff, 0xf, 0xf));
    GameOver = 0;
    DefenderMissileCounter = 0;
    InvaderCounter = 0;
    XDirectionChange = 0;
    LevelComplete = 0;
    x_step = 2;    
    Defender.show();   
    while (!LevelComplete )
    {
      // Show the defender
      Defender.redraw();
      if (console.RightPressed())
      {
        // Move right
        if (Defender.getX() < (SCREEN_WIDTH - 11))
        {
          Defender.move(Defender.getX() + 2, Defender.getY());
        }
      }

      if (console.LeftPressed())
      {
        // Move left
        if (Defender.getX() > 1)
        {
          Defender.move(Defender.getX() - 2, Defender.getY());
        }
      }
      if (console.FirePressed() )
      {
        // User pressed Fire!
        // See if there is an empty missile-in-flight slot
        for (DefenderMissileCounter = 0; DefenderMissileCounter < MAX_DEFENDER_MISSILES; DefenderMissileCounter++)
        {
          if (DefenderMissiles[DefenderMissileCounter].visible() == 0)
          {
            // Found an empty slot, place a missile just above the centre of the defender
            DefenderMissiles[DefenderMissileCounter].show();
            DefenderMissiles[DefenderMissileCounter].move(Defender.getX() + Defender.getWidth() / 2 - DefenderMissiles[DefenderMissileCounter].getWidth() / 2, Defender.getY() - DefenderMissiles[DefenderMissileCounter].getHeight() / 2);
            break; // exit remaining 'for' loop cycles
          }
        }
      }
      // Update defender missiles
      for (DefenderMissileCounter = 0; DefenderMissileCounter < MAX_DEFENDER_MISSILES; DefenderMissileCounter++)
      {
        if ((DefenderMissiles[DefenderMissileCounter].getY() > 0) && (DefenderMissiles[DefenderMissileCounter].visible()))
        {
          DefenderMissiles[DefenderMissileCounter].move(DefenderMissiles[DefenderMissileCounter].getX(), DefenderMissiles[DefenderMissileCounter].getY() - 2);
          // Did any missile hit an invader?
          for (InvaderCounter = 0; InvaderCounter < MAX_INVADERS; InvaderCounter++)
          {
            if (Invaders[InvaderCounter].within(DefenderMissiles[DefenderMissileCounter].getX(), DefenderMissiles[DefenderMissileCounter].getY()))
            {
              // Missile meets invader!
              Invaders[InvaderCounter].hide();
              DefenderMissiles[DefenderMissileCounter].hide();
              // Do an explosion at that location
              Explode(DefenderMissiles[DefenderMissileCounter].getX(), DefenderMissiles[DefenderMissileCounter].getY());
            }
          }
        }
        else
        {
          if (DefenderMissiles[DefenderMissileCounter].visible())
            DefenderMissiles[DefenderMissileCounter].hide();
        }

      }

      // Update attacker missiles
      ActiveAttackerMissiles = MAX_ATTACKER_MISSILES;
      for (AttackerMissileCounter = 0; AttackerMissileCounter < MAX_ATTACKER_MISSILES; AttackerMissileCounter++)
      {
        if ((AttackerMissiles[AttackerMissileCounter].getY() < SCREEN_HEIGHT-20) && (AttackerMissiles[AttackerMissileCounter].visible()))
        {
          AttackerMissiles[AttackerMissileCounter].move(AttackerMissiles[AttackerMissileCounter].getX(), AttackerMissiles[AttackerMissileCounter].getY() + 2);

          // Did any attacker missile hit the defender?
          if (Defender.within(AttackerMissiles[AttackerMissileCounter].getX() + AttackerMissiles[AttackerMissileCounter].getWidth() / 2, AttackerMissiles[AttackerMissileCounter].getY() + AttackerMissiles[AttackerMissileCounter].getHeight() / 2))
          {
            Explode(Defender.getX(), Defender.getY());
            DefenderCount --;
            AttackerMissiles[AttackerMissileCounter].hide(); // That missile has exploded
            if (DefenderCount == 0) 
            {   // All defenders are dead :(
                console.fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
                console.putText("GAME OVER", 9, 40, 100, RGBToWord(0xff, 0xff, 0xff), 0);
                console.putText("Fire to restart", 15, 8, 120, RGBToWord(0xff, 0xff, 0), RGBToWord(0, 0, 0));
                while (!console.FirePressed())
                    console.sleep(10);
                return;
            }
            console.fillRectangle(SCREEN_WIDTH-5*15, SCREEN_HEIGHT-10, 120, 10, 0);  // wipe out the remaining lives area
            for (Index = DefenderCount; Index > 0; Index--) //  draw remaining lives
                console.fillRectangle(SCREEN_WIDTH - Index * 15, SCREEN_HEIGHT-10, 10, 10, RGBToWord(0xff, 0xf, 0xf));
          }

        }
        else
        {
          if (AttackerMissiles[AttackerMissileCounter].visible())
            AttackerMissiles[AttackerMissileCounter].hide();
          if (ActiveAttackerMissiles > 0)
            ActiveAttackerMissiles--;

        }
      }
      // Update invaders
      XDirectionChange = 0;
      YDirectionChange = 0;
      LevelComplete = 1;
      for (InvaderCounter = 0; InvaderCounter < MAX_INVADERS; InvaderCounter++)
      {
        if (Invaders[InvaderCounter].visible())
        {
          if (ActiveAttackerMissiles < MAX_ATTACKER_MISSILES - Level)
          {
            // Spawn a new attacker missile
            AttackerMissileCounter = 0;
            uint8_t MissileFired = 0;
            while ( (AttackerMissileCounter < MAX_ATTACKER_MISSILES) && (!MissileFired))
            {
              if ((AttackerMissiles[AttackerMissileCounter].visible() == 0) && (console.random(0,50)==0))
              {
                ActiveAttackerMissiles++;
                AttackerMissiles[AttackerMissileCounter].show();
                AttackerMissiles[AttackerMissileCounter].move(Invaders[InvaderCounter].getX() + Invaders[InvaderCounter].getWidth() / 2 - DefenderMissiles[DefenderMissileCounter].getWidth() / 2, Invaders[InvaderCounter].getY() + DefenderMissiles[DefenderMissileCounter].getHeight() / 2);
                MissileFired = 1;
              }
              AttackerMissileCounter++;
            }
          }
          LevelComplete = 0; // didn't win yet, there are still invaders
          Invaders[InvaderCounter].move(Invaders[InvaderCounter].getX() + x_step, Invaders[InvaderCounter].getY() + y_step);
          if (Invaders[InvaderCounter].getX() >= (SCREEN_WIDTH - Invaders[InvaderCounter].getWidth()))
            XDirectionChange = 1;
          if (Invaders[InvaderCounter].getX() == 0)
            XDirectionChange = 1;
          if (Invaders[InvaderCounter].getY() > SCREEN_HEIGHT - 100)
          {
            YDirectionChange = 1;

            if (Invaders[InvaderCounter].getY() < 3)
            {
              YDirectionChange = 1;
            }

          }
        }
      }
      if (XDirectionChange) // Did an invader hit either edge?
        x_step = -x_step; // if so, then reverse direction
      if (YDirectionChange) // Did an invader hit either edge?
        y_step = -y_step; // if so, then reverse direction
        
      console.sleep(15);  // slow the game to make it playable
      
    } // End of while (!LevelComplete )
    if ((LevelComplete == 1) && (Level > 0))
    {
      Level--;
      console.putText("Level", 5, 5, SCREEN_HEIGHT-10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));
      console.putNumber(MAX_INVADER_LEVELS - Level + 1, 60, SCREEN_HEIGHT-10, RGBToWord(0xff, 0xff, 0xff), RGBToWord(0, 0, 0));

    }
  } // end of while (Level > 0)
  console.fillRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, RGBToWord(0, 0, 0xff));
  console.putText("VICTORY!", 8, 40, 100, RGBToWord(0xff, 0xff, 0), RGBToWord(0, 0, 0xff));
  console.putText("Fire to restart", 15, 8, 120, RGBToWord(0xff, 0xff, 0), RGBToWord(0, 0, 0xff));
  while (!console.FirePressed())
      console.sleep(10);
  return;
}
void Explode(uint16_t X, uint16_t Y)
{
  Sprite Bang(ExplosionImage, X - 5, Y - 6, 10, 12);
  Bang.show();
  console.playTone(2000,50);
  console.sleep(50);
  console.playTone(500,50);
  console.sleep(50);
  Bang.hide();
}



