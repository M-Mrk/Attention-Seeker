#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <TFT_eSPI.h>
#include "Screen.h"

class DisplayManager
{
private:
    TFT_eSPI *display;
    Screen *currentScreen;
    int currentScreenIndex;
    static const int MAX_SCREENS = 2;
    Screen *screens[MAX_SCREENS];
    int brightness;
    bool errorShown = false;

public:
    DisplayManager();

    void init();
    void registerScreen(Screen *screen, int index);
    void setScreen(int index);
    void draw();
    void handleInput(InputEvent event); // pass to current screen or automatically switch screens
    void setBrightness(int level);
    int getBrightness() { return brightness; }
    void showError(String message);

    // Get display pointer if needed elsewhere
    TFT_eSPI *getDisplay() { return display; }
};

extern DisplayManager displayManager; // make instance available globally

#endif