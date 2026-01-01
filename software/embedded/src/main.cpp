#include <Arduino.h>
#include <ESP32Time.h>

#include "pins.h"

// Display
#include "DisplayManager.h"
#include "LightManager.h"
#include "MelodyManager.h"
#include "UsbHandler.h"
#include "screens/HotkeyScreen.h"
#include "screens/SettingsScreen.h"
#include "screens/TimeScreen.h"

DisplayManager displayManager;
TimeScreen timeScreen;
HotkeyScreen hotkeyScreen;
SettingsScreen settingsScreen;
ESP32Time rtc(3600);

MelodyManager melodyManager;

// Input
#include "physicalInput.h"

void displayDrawTask(void *pvParameters) {
  QueueHandle_t inputQueue = static_cast<QueueHandle_t>(pvParameters);
  for (;;) {
    InputEvent event;
    if (xQueueReceive(inputQueue, &event, 0)) {
      displayManager.handleInput(event);
    }
    displayManager.draw();
    vTaskDelay(pdMS_TO_TICKS(16)); // Approx ~60 FPS
  };
};

QueueHandle_t inputQueue;

void setup(void) {
  usbHandler.setup();
  Serial.println("Attention Seeker starting...");

  // Input
  inputQueue = xQueueCreate(10, sizeof(InputEvent));
  initInputs();
  startInputTasks(inputQueue);

  // Melody
  melodyManager.init(BUZZER_PIN);

  // Light
  lightManager.init();

  // Display
  displayManager.init();
  // register screens and select settings screen
  displayManager.registerScreen(&timeScreen, 0);
  displayManager.registerScreen(&hotkeyScreen, 1);
  displayManager.registerScreen(&settingsScreen, 2);
  // use registered index for initial screen (if registration failed idxSettings
  // may be -1)
  displayManager.setScreen(0);
  xTaskCreatePinnedToCore( // Start display drawing task
      displayDrawTask, "DisplayDrawTask", 4096, inputQueue, 1, nullptr, 1);

  // RTC
  rtc.setTime(30, 24, 15, 17, 1, 2021); // 15:24:30 17.01.2021
  // TODO: remember time somehow across reboots

  usbHandler.startListening();

  Serial.println("Attention Seeker initialized");

  // Play startup sound
  melodyManager.playSuccess();

  lightManager.setLightLevel(125); // set medium light level
}

void loop() {}