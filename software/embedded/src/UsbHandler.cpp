#include "UsbHandler.h"
#include "DisplayManager.h"

#include <Arduino.h>
#include <ESP32Time.h>
#include <USB.h>
#include <USBHIDKeyboard.h>
#include <cstdio>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

static TaskHandle_t usbListenTaskHandle = nullptr;
UsbHandler usbHandler;
static USBHIDKeyboard hidKeyboard;
static bool serialActive = false;
static bool hidActive = false;
static bool listenerSuspended = false;
static bool usbInitialized = false;

static void normalizeNewlines(String &text) {
  text.replace("\\r\\n", "\n");
  text.replace("\\n", "\n");
  text.replace("\\r", "\n");
  text.replace("\r", "\n");
}

static bool extractStringField(const String &json, const char *key, String &out) {
  const String pattern = "\"" + String(key) + "\"";
  const int keyPos = json.indexOf(pattern);
  if (keyPos < 0)
    return false;

  const int colonPos = json.indexOf(':', keyPos + pattern.length());
  if (colonPos < 0)
    return false;

  const int firstQuote = json.indexOf('"', colonPos);
  if (firstQuote < 0)
    return false;

  const int secondQuote = json.indexOf('"', firstQuote + 1);
  if (secondQuote < 0)
    return false;

  out = json.substring(firstQuote + 1, secondQuote);
  return true;
}

static void handleTimeSync(const String &json) {
  String timestamp;
  if (!extractStringField(json, "timestamp", timestamp)) {
    Serial.println("USB: timestamp missing in time_sync");
    return;
  }

  int year, month, day, hour, minute, second;
  const int matched = sscanf(timestamp.c_str(), "%d-%d-%dT%d:%d:%d", &year,
                             &month, &day, &hour, &minute, &second);
  if (matched != 6) {
    Serial.println("USB: invalid timestamp format");
    return;
  }

  ESP32Time rtc;
  rtc.setTime(second, minute, hour, day, month, year);
  Serial.println("USB: time synchronized");
}

static void handleNotification(const String &json) {
  String origin;
  String title;
  String body;

  extractStringField(json, "origin", origin);
  extractStringField(json, "title", title);
  extractStringField(json, "body", body);

  normalizeNewlines(body);

  if (origin.isEmpty())
    origin = "Unknown";
  if (title.isEmpty())
    title = "Notification";

  displayManager.showMessage(origin, title, body);
}

static void processJsonLine(String line) {
  line.trim();
  if (line.isEmpty())
    return;

  String type;
  if (!extractStringField(line, "type", type)) {
    Serial.println("USB: ignoring line without type");
    return;
  }

  if (type == "time_sync") {
    handleTimeSync(line);
  } else if (type == "notification") {
    handleNotification(line);
  } else {
    Serial.print("USB: unknown message type ");
    Serial.println(type);
  }
}

static void usbListenTask(void *pvParameters) {
  String buffer;
  buffer.reserve(256);

  for (;;) {
    while (Serial.available()) {
      char c = static_cast<char>(Serial.read());
      if (c == '\n') {
        processJsonLine(buffer);
        buffer = "";
      } else if (c != '\r') {
        buffer += c;
        if (buffer.length() > 512) {
          buffer.remove(0, buffer.length() - 512);
        }
      }
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

static void suspendUsbListenTask() {
  if (usbListenTaskHandle != nullptr && !listenerSuspended) {
    vTaskSuspend(usbListenTaskHandle);
    listenerSuspended = true;
  }
}

static void resumeUsbListenTask() {
  if (usbListenTaskHandle != nullptr && listenerSuspended) {
    vTaskResume(usbListenTaskHandle);
    listenerSuspended = false;
  }
}

static void startHidMode() {
  suspendUsbListenTask();

  if (serialActive) {
    Serial.end();
    serialActive = false;
    delay(50); // give USB stack time to stop CDC
  }

  if (!hidActive) {
    if (!usbInitialized) {
      USB.begin();
      usbInitialized = true;
      delay(50); // allow host to enumerate USB device
    }

    hidKeyboard.begin();
    hidActive = true;
    delay(10);
  }
}

static void stopHidStartSerial() {
  if (hidActive) {
    hidActive = false;
    delay(10);
  }

  if (!serialActive) {
    Serial.begin(115200);
    serialActive = true;

    uint32_t start = millis();
    while (!Serial && (millis() - start) < 2000) {
      delay(10);
    }
  }

  resumeUsbListenTask();
}

void UsbHandler::setup() {
  Serial.begin(115200);
  serialActive = true;
  //   hidKeyboard.begin();
  //   USB.begin();

  uint32_t start = millis();
  while (!Serial && (millis() - start) < 2000) {
    delay(10);
  }

  Serial.println("USB HID keyboard ready");
}

void UsbHandler::startListening() {
  if (usbListenTaskHandle != nullptr)
    return;

  const BaseType_t created =
      xTaskCreatePinnedToCore(usbListenTask, "UsbListenTask", 4096, nullptr, 1,
                              &usbListenTaskHandle, 0);

  if (created != pdPASS) {
    Serial.println("USB: failed to start listen task");
    usbListenTaskHandle = nullptr;
  }
}

void UsbHandler::sendKey(char key) {
  startHidMode();

  hidKeyboard.press(static_cast<uint8_t>(key));
  delay(5);
  hidKeyboard.releaseAll();
  delay(5);

  stopHidStartSerial();
}
