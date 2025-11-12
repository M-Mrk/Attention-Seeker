#ifndef TIMESCREEN_H
#define TIMESCREEN_H

#include <TFT_eSPI.h>
#include "../Screen.h"

class TimeScreen : public Screen {
private:
    int lastMinutes;
    int lastDay;
    bool timeRedraw;
    bool dateRedraw;
    String hoursText;
    String dateText;
    TaskHandle_t timeUpdateTaskHandle = nullptr;

public:
    TimeScreen(){}
    
    void init(TFT_eSPI* display) override;
    void draw(TFT_eSPI* display) override;
    void end() override;
    void updateTime();
};

#endif