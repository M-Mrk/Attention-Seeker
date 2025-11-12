#ifndef SETTINGSSCREEN_H
#define SETTINGSSCREEN_H

#include <TFT_eSPI.h>
#include "../Screen.h"

class SettingsScreen : public Screen
{
private:
    bool settingsChanged;
    enum SettingsSelection
    {
        LightLevel,
        DisplayBrightness,
        SettingsNone
    };
    SettingsSelection currentSelection;

public:
    SettingsScreen() {}

    void init(TFT_eSPI *display) override;
    void draw(TFT_eSPI *display) override;
    InputEvent handleInput(InputEvent input) override;
    void end() override;
};

#endif