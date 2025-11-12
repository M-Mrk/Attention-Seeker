#ifndef SCREEN_H
#define SCREEN_H

#include <TFT_eSPI.h>

enum INPUT_EVENT{
    NONE,
    SW1,
    SW2,
    SW3,
    SW4,
    ROTARY_CW,
    ROTARY_CCW
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
    virtual INPUT_EVENT handleInput(INPUT_EVENT input) {
        return input;
    }

    // Optional: called when switching away from this screen
    virtual void end() {}
};

#endif