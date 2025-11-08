#ifndef PINS_H
#define PINS_H

#include <Arduino.h>

// Display
constexpr int BACKLIGHT_PIN = 17;

constexpr int TFT_DC = 9;
constexpr int TFT_CS = 10;
constexpr int TFT_MOSI = 11;
constexpr int TFT_CLK = 12;
constexpr int TFT_RST = 8;
constexpr int TFT_MISO = 13;

// Mechanical switches / kbd
constexpr int SW2_PIN = 1, SW3_PIN = 2, SW4_PIN = 3; 

// Buzzer
constexpr int BUZZER_PIN = 7;

// Rotary encoder
constexpr int ROTARY_CLK_PIN = 4;
constexpr int ROTARY_DT_PIN = 5;
constexpr int ROTARY_SW_PIN = 6;

// Light
constexpr int LED_PIN = 14;
constexpr int BUILD_IN_LED_PIN = 48;

#endif