#include <Arduino.h>
#include <ESP32Time.h>

#include "pins.h"

#include "DisplayManager.h"
#include "screens/TimeScreen.h"

ESP32Time rtc(3600);


DisplayManager displayManager;
TimeScreen timeScreen;
TFT_eSPI tft = TFT_eSPI();

void displayDrawTask(void *pvParameters) {
    for (;;) {
        displayManager.draw();
        vTaskDelay(pdMS_TO_TICKS(16)); // Approx ~60 FPS
    };
};


void aliveTask(void *pvParameters) {
    for (;;) {
        Serial.println("Alive");
        vTaskDelay(pdMS_TO_TICKS(5000));
    }
};

void setup(void)
{
    Serial.begin(115200);
    Serial.println("Attention Seeker starting...");

    // Display
    displayManager.init();
    displayManager.setScreen(&timeScreen);
    xTaskCreatePinnedToCore( // Start display drawing task
        displayDrawTask,
        "DisplayDrawTask",
        4096,
        nullptr,
        1,
        nullptr,
        1
    );

    // RTC
    rtc.setTime(30, 24, 15, 17, 1, 2021); // 15:24:30 17.01.2021
    // TODO: remember time somehow across reboots

    // Alive Task
    xTaskCreatePinnedToCore(
        aliveTask,
        "AliveTask",
        2048,
        nullptr,
        1,
        nullptr,
        1
    );

    Serial.println("Attention Seeker initialized");
}

void loop()
{
}