#ifndef DISPLAYMANAGER_H
#define DISPLAYMANAGER_H

#include <Adafruit_ILI9341.h>
#include "Screen.h"

class DisplayManager {
private:
    Adafruit_ILI9341 display;  // Display object as member variable
    Screen* screens[5];
    int numScreens;
    Screen* activeScreen;

public:
    DisplayManager();
    void init();
    void addScreen(Screen* screen);
    void switchTo(int index);
    Screen* current();
    Adafruit_ILI9341* getDisplay() { return &display; }
};

#endif