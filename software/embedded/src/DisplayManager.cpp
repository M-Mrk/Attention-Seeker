#include <Arduino.h>
#include "pins.h"

#include <TFT_eSPI.h>

#include "Screen.h"
#include "DisplayManager.h"

DisplayManager::DisplayManager()
{
    display = new TFT_eSPI();
    currentScreen = nullptr;
}

void DisplayManager::init()
{
    display->begin();
    display->setRotation(3);
    pinMode(BACKLIGHT_PIN, OUTPUT);
    analogWriteFrequency(2000);
    setBrightness(125);
}

void DisplayManager::setScreen(Screen *newScreen)
{
    if (currentScreen != nullptr)
    {
        currentScreen->end();
    }

    currentScreen = newScreen;
    if (currentScreen != nullptr)
    {
        display->fillScreen(0x0); // Clear screen before switching
        currentScreen->init(display);
    }
}

void DisplayManager::draw()
{
    if (currentScreen != nullptr)
    {
        currentScreen->draw(display);
    }
}

void DisplayManager::handleInput(INPUT_EVENT event)
{
    if (currentScreen != nullptr)
    {
        currentScreen->handleInput(event);
    }
}

void DisplayManager::setBrightness(int level)
{
    level = constrain(level, 0, 255);
    analogWrite(BACKLIGHT_PIN, level);
}