#include <Arduino.h>

#include "Screen.h"
#include "pins.h"
#include <ESP32Encoder.h>

ESP32Encoder encoder;

void initInputs() {
  encoder.attachSingleEdge(ROTARY_DT_PIN, ROTARY_CLK_PIN);
  encoder.setCount(0);

  pinMode(ROTARY_SW_PIN, INPUT_PULLUP);
  pinMode(SW2_PIN, INPUT_PULLUP);
  pinMode(SW3_PIN, INPUT_PULLUP);
  pinMode(SW4_PIN, INPUT_PULLUP);
}

InputEvent checkEncoder() {
  int64_t count = encoder.getCount();
  if (count == 0) {
    return InputEvent::NoneInput;
  } else if (count > 0) {
    encoder.setCount(0); // Reset count after reading
    return InputEvent::RotaryCw;
  } else {
    encoder.setCount(0); // Reset count after reading
    return InputEvent::RotaryCcw;
  }
}

void encoderTask(void *pvParameters) {
  QueueHandle_t inputQueue = static_cast<QueueHandle_t>(pvParameters);

  unsigned long lastEncoderMillis = 0;
  unsigned long encoderDebounce = 250;

  for (;;) {
    if (lastEncoderMillis + encoderDebounce > millis()) { // check whether enough time has passed since last input
      vTaskDelay(pdMS_TO_TICKS(10));
      continue;
    }
    lastEncoderMillis = millis();

    InputEvent event = checkEncoder();
    if (event != InputEvent::NoneInput) {
      xQueueSendToBack(inputQueue, &event, portMAX_DELAY);
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void switchTask(void *pvParameters) {
  QueueHandle_t inputQueue = static_cast<QueueHandle_t>(pvParameters);

  unsigned long lastRotarySWMillis = 0;
  unsigned long lastSW2Millis = 0;
  unsigned long lastSW3Millis = 0;
  unsigned long lastSW4Millis = 0;
  unsigned long switchDebounce = 200;

  for (;;) {
    unsigned long currentMillis = millis();

    // Rotary switch
    if (digitalRead(ROTARY_SW_PIN) == LOW) {
      if (lastRotarySWMillis + switchDebounce <= currentMillis) {
        InputEvent event = InputEvent::SW1;
        xQueueSendToBack(inputQueue, &event, portMAX_DELAY);
        lastRotarySWMillis = currentMillis;
      }
    }

    // SW2
    if (digitalRead(SW2_PIN) == LOW) {
      if (lastSW2Millis + switchDebounce <= currentMillis) {
        InputEvent event = InputEvent::SW2;
        xQueueSendToBack(inputQueue, &event, portMAX_DELAY);
        lastSW2Millis = currentMillis;
      }
    }

    // SW3
    if (digitalRead(SW3_PIN) == LOW) {
      if (lastSW3Millis + switchDebounce <= currentMillis) {
        InputEvent event = InputEvent::SW3;
        xQueueSendToBack(inputQueue, &event, portMAX_DELAY);
        lastSW3Millis = currentMillis;
      }
    }

    // SW4
    if (digitalRead(SW4_PIN) == LOW) {
      if (lastSW4Millis + switchDebounce <= currentMillis) {
        InputEvent event = InputEvent::SW4;
        xQueueSendToBack(inputQueue, &event, portMAX_DELAY);
        lastSW4Millis = currentMillis;
      }
    }

    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void startInputTasks(QueueHandle_t inputQueue) {
  // Start encoder task
  xTaskCreatePinnedToCore(encoderTask, "EncoderTask", 2048, inputQueue, 1,
                          nullptr, 1);

  // Start switch task
  xTaskCreatePinnedToCore(switchTask, "SwitchTask", 2048, inputQueue, 1,
                          nullptr, 1);
}