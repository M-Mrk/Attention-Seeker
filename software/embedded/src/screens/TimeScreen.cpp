#include "screens/TimeScreen.h"
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>

void TimeScreen::init() {
    Serial.println("Time screen started");
}

void TimeScreen::draw() {
    Serial.println("[Time: 12:45 PM]");
    if (display_) {
        display_->fillScreen(ILI9341_BLACK);
        display_->setTextColor(ILI9341_GREEN);
        display_->setTextSize(3);
        display_->setCursor(10, 100);
        display_->print("12:45");
        display_->setTextSize(2);
        display_->print(" PM");
    }
}

void TimeScreen::end() {
    Serial.println("Time screen closed");
}

void TimeScreen::input(INPUT_EVENT input) {
    // No input handling for now
}