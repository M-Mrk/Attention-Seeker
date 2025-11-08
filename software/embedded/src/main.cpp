#include <Arduino.h>
#include "pins.h"

#include "DisplayManager.h"
#include "screens/TimeScreen.h"

void setup(void) {
  Serial.begin(115200);
  Serial.println("Attention Seeker starting...");

  pinMode(BACKLIGHT_PIN, OUTPUT);
  digitalWrite(BACKLIGHT_PIN, LOW); // Turn on backlight

  DisplayManager displayManager;
  displayManager.init();

  TimeScreen timeScreen;
  displayManager.addScreen(&timeScreen);
  displayManager.switchTo(0);
  displayManager.current()->draw();

  Serial.println("Attention Seeker initialized");
}

void loop() {

}