#ifndef TIMESCREEN_H
#define TIMESCREEN_H

#include <TFT_eSPI.h>
#include "../Screen.h"

class TimeScreen : public Screen {
public:
    TimeScreen(){}
    
    void init(TFT_eSPI* display) override;
    void draw(TFT_eSPI* display) override;
    void handleInput(INPUT_EVENT inp) override;
    void end() override;
    void updateTime();
};

#endif