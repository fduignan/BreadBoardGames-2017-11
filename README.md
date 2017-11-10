<img src=sitelogo.jpeg>
Breadboard games Version 2017-11 (Wexford)
This version of Breadboard games was developed for Wexford Science week 2017.  For further information see 
http://ioprog.com/bbg

The Console API
    void begin()
        Initialize the console hardware and software.  This should be called once during the start-up sequencey
    
    void putPixel(uint16_t x, uint16_t y, uint16_t colour);
        Draw a pixel at the specified x,y location with the specified colour

    void putImage(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint16_t *Image);
        Put an image of a specific width and height at a particular location.  The x,y variables specify the top left corner
        of the target image location

    void fillRectangle(uint16_t x, uint16_t y, uint16_t width, uint16_t height, uint16_t Colour);
        Draw a filled rectangle of a specific width and height at a particular location.  The x,y variables specify the top 
        left corner of the target rectangle location

    void putText(const char *Text, uint16_t len, uint16_t x, uint16_t y, uint16_t ForeColour, uint16_t BackColour);
        Write the specied text with len characters within a rectangle whose top left corner is specified by x and y.        
        The text will have the specified forecolour and will be printed in a box filled with BackColour

    void putNumber(uint16_t Number, uint16_t x, uint16_t y, uint16_t ForeColour, uint16_t BackColour);    
        Similar to putText except the number is printed.  The number range is 0 to 65535.  The number is always
        printed with 5 digits so leading zeros are printed

    int LeftPressed()
        returns 1 if the left button was pressed, otherwise 0
        
    int RightPressed();
        returns 1 if the right button was pressed, otherwise 0
        
    int FirePressed();
        returns 1 if the fire button was pressed, otherwise 0
    
    void playTone(uint32_t Frequency, uint32_t ms);
        Outputs the tone of the specied frequency for the specified milliseconds.  The tone is played 
        in the background (the function returns without waiting for the tone to finish)
        
    void stopTone()
        Halts sound playback
        
    int  SoundPlaying();
        return 1 if there is a tone currently playing 
    void playMelody(const uint32_t *Tones,const uint32_t *Times,int len);
        Plays a melody consisting of 'len' notes.  The duration of each note is specified in the array 'Times'
        
    void sleep(uint32_t ms);
        Pause (and save power) for the number of milliseconds specified.
        
    uint32_t random(uint32_t lower,uint32_t upper);   
        return a pseudo random number in the range lower to upper-1
The Sprite API
    Sprite(const uint16_t *image, uint16_t x, uint16_t y, uint8_t w, uint8_t h);
    Constuctor for a sprite object.  The image (an array of 16 bit colour values) is used
    as a sprite image.  The initial location (x,y) and width,height (w,h) are also specified
      
    void show();
        Display the sprite at its current location
      
    void redraw();
        Redraw the sprite if is marked visible
  
    void hide();
        Hide the sprite - The sprite is wiped out with a black rectangle and marked as invisible
        
    uint16_t getX();
        return the current x ordinate
    
    uint16_t getY(); 
        return the current y ordinate
        
    uint8_t getWidth(); 
        return the current width (range 0 to 255)
    
    uint8_t getHeight();
        return the current height (range 0 to 255)  
        
    void move(uint16_t x, uint16_t y);
        move the sprite to the new location (x,y).  Invisible
        sprites are not made to reappear.
        
    uint16_t visible();
        returns '1' if the sprite is marked as visible
    
    uint16_t within(uint16_t x, uint16_t y)
        returns '1' if the specified point (x,y) is within the bounding
        rectangle of the sprite
  
    uint16_t touching(uint16_t x, uint16_t y)
        This function returns a non zero value if the object at x,y touches the sprite
        The sprite is assumed to be rectangular and returns a value as follows:
        0 : not hit (or sprite is invisible)
        1 : touching on top face (lesser Y value)
        2 : touching on left face (lesser X value)
        3 : touching on bottom face (greater Y value)    
        4 : touching on right face (greater X value)
