#ifndef SCREEN_H
#define SCREEN_H

#include <Arduino.h>
#include <Adafruit_ILI9341.h>

enum INPUT_EVENT {
    SW1,
    SW2,
    SW3,
    SW4,
    ROTARY_CW,
    ROTARY_CCW,
};

enum SCREENS {
    SCREEN_TIME = 0,
};

class Screen {
    protected:
        Adafruit_ILI9341* display_ = nullptr; // Set by DisplayManager
    public:
        virtual ~Screen() {}
        // Called once when screen becomes active
        virtual void init();
        // Called by manager to render content
        virtual void draw();
        // Called before switching away
        virtual void end();
        // Input events forwarded from manager
        virtual void input(INPUT_EVENT input);
        // Internal use: Provide display pointer
        void setDisplay(Adafruit_ILI9341* d) { display_ = d; }
    protected:
        // Helper for derived screens: assert display available
        bool hasDisplay() const { return display_ != nullptr; }
};

#endif