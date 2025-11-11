#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <TFT_eSPI.h>
#include "Screen.h"

class DisplayManager {
private:
    TFT_eSPI* display;
    Screen* currentScreen;
    
public:
    DisplayManager();
    
    void init();
    void setScreen(Screen* newScreen);
    void draw();
    void handleInput(INPUT_EVENT event);
    void setBrightness(int level);
    
    // Get display pointer if needed elsewhere
    TFT_eSPI* getDisplay() { return display; }
};

#endif