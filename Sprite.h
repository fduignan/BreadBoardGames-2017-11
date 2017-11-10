#include <stdint.h>
#include "console.h"
extern Console console;
class Sprite {
  public:
    Sprite(const uint16_t *image, uint16_t x, uint16_t y, uint8_t w, uint8_t h) {
      X = x;
      Y = y;
      Width = w;
      Height = h;
      Image = image;
      Visible = 0;      
    }
  void show() {
    if (!Visible)
    {
      console.putImage(X,Y,Width,Height,Image);
    }
    Visible=1;
  }
  void redraw() 
  {
    if (Visible)
      console.putImage(X,Y,Width,Height,Image);    
  }
  void hide() {
    if (Visible)
    {
        // assumes a black backgound (colour = 0)
        console.fillRectangle(X,Y,Width,Height,0);
    }
    Visible = 0;
  }
  inline uint16_t getX() {
    return X;
  }
  inline uint16_t getY() {
    return Y;
  }
  inline uint8_t getWidth() {
    return Width;
  }
  inline uint8_t getHeight() {
    return Height;
  }
  
  void move(uint16_t x, uint16_t y) {
    if (Visible)
    {
      // Hide the object first
      console.fillRectangle(X,Y,Width,Height,0);    
      X = x;
      Y = y;
      // Now show it
      console.putImage(X,Y,Width,Height,Image);
    }
    else
    {
      X = x;
      Y = y;
    }
  }
  inline uint16_t visible() {    
    return Visible;
  }
  uint16_t within(uint16_t x, uint16_t y)
  {
    if (Visible == 0)
        return 0;
    if ( (x>=X) && ( x < (X+Width) ) )
    {
        if ( (y>=Y) && ( y < (Y+Height) ) )
            return 1;
        else
            return 0;
                  
    }
    else
        return 0;

  }
  uint16_t touching(uint16_t x, uint16_t y)
  {
    // This function returns a non zero value if the object at x,y touches the sprite
    // The sprite is assumed to be rectangular and returns a value as follows:
    // 0 : not hit
    // 1 : touching on top face (lesser Y value)
    // 2 : touching on left face (lesser X value)
    // 3 : touching on bottom face (greater Y value)    
    // 4 : touching on right face (greater X value)
    if (Visible == 0)
        return 0;
    if ( y == Y  )
    {  // top face?
      if ( (x>=X) && ( x < (X+Width) ) )
        return 1;      
    }
    if ( x == X )
    {
      // left face
      if ( (y>=Y) && ( y < (Y+Height) ) )
        return 2;
    }
    if ( y == (Y+Height-1)  )
    {  // bottom face?
      if ( (x>=X) && ( x < (X+Width) ) )
        return 3;      
    }
    if ( x == (X + Width-1) )
    {
      // right face
      if ( (y>=Y) && ( y < (Y+Height) ) )
        return 4;
    }

    return 0; // not touching
  }
  private:   
    const uint16_t *Image;
    uint8_t Width,Height;  // shouldn't have sprite wider,taller than 255 pixels
    uint16_t X,Y;
    uint8_t Visible;    
};
