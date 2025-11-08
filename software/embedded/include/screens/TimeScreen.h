#ifndef TIME_SCREEN_H
#define TIME_SCREEN_H

#include "../Screen.h"

class TimeScreen : public Screen {
public:
    void init();
    void draw();
    void end();
    void input(INPUT_EVENT input);
};

#endif