#include <Arduino.h>
#include "pins.h"

#include <TFT_eSPI.h>

#include "Screen.h"
#include "DisplayManager.h"

DisplayManager::DisplayManager()
{
    display = new TFT_eSPI();
    currentScreen = nullptr;

    // initialize registry
    for (int i = 0; i < MAX_SCREENS; ++i)
    {
        screens[i] = nullptr;
    }
}

void DisplayManager::init()
{
    display->begin();
    display->setRotation(3);
    pinMode(BACKLIGHT_PIN, OUTPUT);
    analogWriteFrequency(15000); // set frequency to reduce seeable flicker
    setBrightness(125);
}

// register screen at index
void DisplayManager::registerScreen(Screen *screen, int index)
{
    if (index < 0 || index >= MAX_SCREENS)
    {
        Serial.print("registerScreen: invalid index ");
        Serial.println(index);
        return;
    }
    screens[index] = screen;
}

// set screen at index to active screen and initialize it
void DisplayManager::setScreen(int index)
{
    if (index < 0 || index >= MAX_SCREENS)
    {
        Serial.print("setScreen: invalid index ");
        Serial.println(index);
        return;
    }

    currentScreenIndex = index;
    Screen *newScreen = screens[index];
    if (newScreen == nullptr)
    {
        Serial.print("setScreen: no screen registered at index ");
        Serial.println(index);
        return;
    }

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

// handle input event or switch screens
void DisplayManager::handleInput(InputEvent event)
{
    Serial.print("Input event received: ");
    Serial.println(static_cast<int>(event));
    if (currentScreen != nullptr)
    {
        event = currentScreen->handleInput(event);
        if (event == InputEvent::RotaryCw)
        {
            if (currentScreenIndex + 1 < MAX_SCREENS)
            {
                setScreen(currentScreenIndex + 1);
            }
            else
            {
                setScreen(0); // wrap around
            }
        }
        else if (event == InputEvent::RotaryCcw)
        {
            if (currentScreenIndex - 1 >= 0)
            {
                setScreen(currentScreenIndex - 1);
            }
            else
            {
                setScreen(MAX_SCREENS - 1); // wrap around
            }
        }
    }
}

void DisplayManager::setBrightness(int level)
{
    level = constrain(level, 0, 255);
    brightness = level;
    analogWrite(BACKLIGHT_PIN, brightness);
}