#include <Arduino.h>
#include <ESP32Time.h>

#include "pins.h"

// Display
#include "DisplayManager.h"
#include "screens/TimeScreen.h"
#include "screens/SettingsScreen.h"

DisplayManager displayManager;
TimeScreen timeScreen;
SettingsScreen settingsScreen;

ESP32Time rtc(3600);

// Input
#include "physicalInput.h"

void displayDrawTask(void *pvParameters) {
    QueueHandle_t inputQueue = static_cast<QueueHandle_t>(pvParameters);
    for (;;) {
        INPUT_EVENT event;
        if (xQueueReceive(inputQueue, &event, 0)) {
            displayManager.handleInput(event);
        }
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

QueueHandle_t inputQueue;

void setup(void)
{
    Serial.begin(115200);
    Serial.println("Attention Seeker starting...");

    // Input
    inputQueue = xQueueCreate(10, sizeof(INPUT_EVENT));
    initInputs();
    startInputTasks(inputQueue);

    // Display
    displayManager.init();
    displayManager.setScreen(&settingsScreen);
    xTaskCreatePinnedToCore( // Start display drawing task
        displayDrawTask,
        "DisplayDrawTask",
        4096,
        inputQueue,
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