#include <Arduino.h>
#include "pins.h"

#include <Adafruit_ILI9341.h>

#include "DisplayManager.h"

DisplayManager::DisplayManager() : display(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO) {
    numScreens = 0;
    activeScreen = nullptr;
}

void DisplayManager::addScreen(Screen* screen) {
    if (numScreens < 5) {
    screens[numScreens] = screen;
    // Provide the screen with the display instance
    screen->setDisplay(&display);
        numScreens++;
    }
}

void DisplayManager::switchTo(int index) {
    if (index >= 0 && index < numScreens) {
        if (activeScreen != nullptr) {
            activeScreen->end();
        }
        activeScreen = screens[index];
        activeScreen->init();
    }
}

Screen* DisplayManager::current() {
    return activeScreen;
}

void DisplayManager::init() {
    display.begin();
    display.setRotation(3);
    display.fillScreen(ILI9341_BLACK);
}