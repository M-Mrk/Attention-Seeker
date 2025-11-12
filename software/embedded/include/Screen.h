#ifndef SCREEN_H
#define SCREEN_H

#include <TFT_eSPI.h>

enum InputEvent
{
    NoneInput,
    SW1, // Rotary switch
    SW2, // mechanical
    SW3, // mechanical
    SW4, // mechanical
    RotaryCw, // rotary encoder clock wise
    RotaryCcw // rotary encoder counter clock wise
};

class Screen
{
public:
    // Virtual destructor - important for proper cleanup
    virtual ~Screen() {}

    // Called once when screen becomes active
    virtual void init(TFT_eSPI *display) = 0;

    // Called every loop to draw/update the screen
    virtual void draw(TFT_eSPI *display) = 0;

    // Optional: handle button presses or other input
    virtual InputEvent handleInput(InputEvent input)
    {
        return input;
    }

    // Optional: called when switching away from this screen
    virtual void end() {}
};

#endif