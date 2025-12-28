#include "LightManager.h"

#include <Arduino.h>

#include "pins.h"

LightManager::LightManager() { lightLevel = 125; }

void LightManager::init() {
  pinMode(LED_PIN, OUTPUT);
  analogWriteFrequency(15000); // keep PWM high to avoid visible flicker
  setLightLevel(lightLevel);
}

void LightManager::setLightLevel(int level) {
  level = constrain(level, 0, 255);
  lightLevel = level;
  analogWrite(LED_PIN, lightLevel); // direct mapping: 0=off, 255=full
}

int LightManager::getLightLevel() {
  return lightLevel;
}

LightManager lightManager;
