#ifndef SCREEN_H
#define SCREEN_H

#include <TFT_eSPI.h>

enum INPUT_EVENT{
    SW1 = 0,
    SW2 = 1,
    SW3 = 2,
    SW4 = 3,
    ROTARY_CW = 4,
    ROTARY_CCW = 5
};

class Screen {
public:
    // Virtual destructor - important for proper cleanup
    virtual ~Screen() {}
    
    // Called once when screen becomes active
    virtual void init(TFT_eSPI* display) = 0;
    
    // Called every loop to draw/update the screen
    virtual void draw(TFT_eSPI* display) = 0;

    // Optional: handle button presses or other input
    virtual void handleInput(INPUT_EVENT event) {}

    // Optional: called when switching away from this screen
    virtual void end() {}
};

#endif