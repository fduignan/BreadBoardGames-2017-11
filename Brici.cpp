// A game inspired by the great game Breakout
#include <stdint.h>
#include "cortexm3.h"
#include "Sprite.h"
#include "console.h"
#include "images.h"

void PlayBrici()
{
#define MAX_BRICI_LEVELS 4
  int Level = MAX_BRICI_LEVELS;
  int LevelComplete = 0;
  int8_t BallCount = 5;
  uint8_t Index;
  int8_t BallXVelocity = 1;
  int8_t BallYVelocity = 1;  
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
