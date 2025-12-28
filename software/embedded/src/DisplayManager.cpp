#include "DisplayManager.h"

#include <Arduino.h>
#include <TFT_eSPI.h>

#include "Screen.h"
#include "pins.h"

DisplayManager::DisplayManager() {
  display = new TFT_eSPI();
  currentScreen = nullptr;

  // initialize registry
  for (int i = 0; i < MAX_SCREENS; ++i) {
    screens[i] = nullptr;
  }
}

void DisplayManager::init() {
  display->begin();
  display->setRotation(3);
  pinMode(BACKLIGHT_PIN, OUTPUT);
  analogWriteFrequency(15000); // set frequency to reduce seeable flicker
  setBrightness(125);
}

// register screen at index
void DisplayManager::registerScreen(Screen *screen, int index) {
  if (index < 0 || index >= MAX_SCREENS) {
    const String errorMsg = "registerScreen: invalid index " + String(index);
    showError(errorMsg);
    return;
  }
  screens[index] = screen;
}

// set screen at index to active screen and initialize it
void DisplayManager::setScreen(int index) {
  if (index < 0 || index >= MAX_SCREENS) {
    const String errorMsg = "setScreen: invalid index " + String(index);
    showError(errorMsg);
    return;
  }

  currentScreenIndex = index;
  Screen *newScreen = screens[index];
  if (newScreen == nullptr) {
    const String errorMsg =
        "setScreen: no screen registered at index " + String(index);
    showError(errorMsg);
    return;
  }

  if (currentScreen != nullptr) {
    currentScreen->end();
  }

  currentScreen = newScreen;
  if (currentScreen != nullptr) {
    display->fillScreen(0x0); // Clear screen before switching
    currentScreen->init(display);
  }
}

void DisplayManager::draw() {
  if (notificationActive) {
    if (notificationExpiryMs != 0 && millis() > notificationExpiryMs) {
      clearNotification();
    }
    return; // keep notification visible until dismissed or timed out
  }

  if (currentScreen != nullptr && !errorShown) {
    currentScreen->draw(display);
  }
}

// handle input event or switch screens
void DisplayManager::handleInput(InputEvent event) {
  if (notificationActive) {
    if (event != InputEvent::NoneInput) {
      clearNotification();
    }
    return;
  }

  if (errorShown) {
    if (currentScreen != nullptr) {
      currentScreen->init(display); // re-initialize screen after error
    }
    errorShown = false;
    return;
  }

  if (currentScreen != nullptr) {
    event = currentScreen->handleInput(event);
    if (event == InputEvent::RotaryCw) {
      if (currentScreenIndex + 1 < MAX_SCREENS) {
        setScreen(currentScreenIndex + 1);
      } else {
        setScreen(0); // wrap around
      }
    } else if (event == InputEvent::RotaryCcw) {
      if (currentScreenIndex - 1 >= 0) {
        setScreen(currentScreenIndex - 1);
      } else {
        setScreen(MAX_SCREENS - 1); // wrap around
      }
    }
  }
}

void DisplayManager::setBrightness(int level) {
  level = constrain(level, 0, 255);
  brightness = level;
  analogWrite(BACKLIGHT_PIN, brightness);
}

void DisplayManager::showError(String message) {
  playTone(900, 200);
  delay(40);
  playTone(600, 250);

  if (errorShown) {
    return; // already showing an error
  }
  errorShown = true;
  if (currentScreen != nullptr) {
    currentScreen->end();
  }
  display->drawRect(8, 8, 304, 224, 0x0);
  display->drawRect(8, 8, 304, 224, 0xA0E5);
  display->drawLine(9, 38, 310, 38, 0xC0E5);
  display->drawLine(9, 38, 310, 38, 0xC0E5);

  display->setTextColor(0xFFFF);
  display->setTextWrap(true, true);
  display->setFreeFont(&FreeSerif18pt7b);
  display->setCursor(10, 34);
  display->print("Error:");

  // error box is from (11,40) to (308, 228)
  display->setFreeFont(&FreeSerif12pt7b);

  const int boxX = 13;
  const int boxY = 62;
  const int boxWidth = 295;  // 308 - 13
  const int boxHeight = 166; // 228 - 62

  int cursorY = boxY;
  const int lineHeight = 22; // Approximate line height

  while (message.length() > 0 && cursorY + lineHeight < boxY + boxHeight) {
    int maxChars = message.length();
    int fitChars = maxChars;
    // Find how many characters fit in the box width
    while (fitChars > 0) {
      int w = display->textWidth(message.substring(0, fitChars));
      if (w <= boxWidth)
        break;
      fitChars--;
    }
    // Avoid breaking words
    if (fitChars < maxChars) {
      int lastSpace = message.substring(0, fitChars).lastIndexOf(' ');
      if (lastSpace > 0)
        fitChars = lastSpace;
    }
    String line = message.substring(0, fitChars);
    display->setCursor(boxX, cursorY);
    display->print(line);
    cursorY += lineHeight;
    message = message.substring(fitChars);
    message.trim();
  }
}

void DisplayManager::showMessage(String origin, String title, String message) {
  notificationActive = true;
  notificationExpiryMs = millis() + 5000; // auto-dismiss after 5s

  playTone(1800, 70);
  playTone(2000, 70);
  playTone(1000, 70);
  playTone(1500, 70);

  display->fillScreen(0x0);

  display->drawRect(8, 8, 304, 224, 0xE521);

  display->drawLine(9, 38, 310, 38, 0xE521);

  display->setTextColor(0xE521);
  display->setTextWrap(false);
  display->setFreeFont(&FreeSerif18pt7b);
  display->setCursor(10, 34);
  display->print(title);

  display->drawLine(8, 59, 311, 59, 0xE521);

  display->setFreeFont(&FreeSerif12pt7b);
  display->setCursor(10, 56);
  display->print("From:");

  display->setCursor(73, 57);
  display->print(origin);

  display->setTextColor(0xFFFF);
  display->setCursor(14, 84);
  display->print(message);
}

void DisplayManager::clearNotification() {
  notificationActive = false;
  notificationExpiryMs = 0;

  if (display != nullptr) {
    display->fillScreen(0x0);
  }

  // Re-render current screen to restore UI
  setScreen(currentScreenIndex);
}

void DisplayManager::playTone(uint16_t freq, uint16_t durationMs) {
  tone(BUZZER_PIN, freq, durationMs);
}