#ifndef SETTINGSSCREEN_H
#define SETTINGSSCREEN_H

#include <TFT_eSPI.h>
#include "../Screen.h"

class SettingsScreen : public Screen {
public:
    SettingsScreen(){}
    
    void init(TFT_eSPI* display) override;
    void draw(TFT_eSPI* display) override;
    INPUT_EVENT handleInput(INPUT_EVENT input) override;
    void end() override;
};

#endif