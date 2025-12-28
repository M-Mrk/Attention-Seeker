#include "screens/SettingsScreen.h"

#include <ESP32Time.h>

#include "DisplayManager.h"

void SettingsScreen::init(TFT_eSPI *display) {
  display->fillScreen(0x0);
  settingsChanged = true;
  currentSelection = SettingsSelection::SettingsNone;
}

void SettingsScreen::draw(TFT_eSPI *display) {
  constexpr uint16_t barColor = 0xFBC0;
  constexpr uint16_t highlightColor = 0xFFE0;
  constexpr int16_t bottomYBar = 214;
  constexpr int16_t topYBar = 17;
  if (display) {
    if (!settingsChanged)
      return;
    settingsChanged = false;

    static const unsigned char PROGMEM image_light_bits[] = {
        0x0c, 0x00, 0x00, 0x30, 0x0c, 0x00, 0x00, 0x30, 0x03, 0x0f, 0xf0, 0xc0,
        0x03, 0x0f, 0xf0, 0xc0, 0x00, 0x30, 0x0c, 0x00, 0x00, 0x30, 0x0c, 0x00,
        0x00, 0xc0, 0x03, 0x00, 0x00, 0xc0, 0x03, 0x00, 0xcc, 0xc3, 0x03, 0x33,
        0xcc, 0xc3, 0x03, 0x33, 0x00, 0xc3, 0xc3, 0x00, 0x00, 0xc3, 0xc3, 0x00,
        0x00, 0xc3, 0x03, 0x00, 0x00, 0xc3, 0x03, 0x00, 0x0c, 0x30, 0x0c, 0x30,
        0x0c, 0x30, 0x0c, 0x30, 0x30, 0x0c, 0x30, 0x0c, 0x30, 0x0c, 0x30, 0x0c,
        0x00, 0x03, 0x00, 0x00, 0x00, 0x03, 0x00, 0x00, 0x00, 0x0f, 0xf0, 0x00,
        0x00, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x0f, 0xf0, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x03, 0xc0, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    static const unsigned char PROGMEM image_monitor_bits[] = {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x03,
        0xcf, 0xff, 0xff, 0xf3, 0xcf, 0xff, 0xff, 0xf3, 0xcc, 0x00, 0x00, 0x33,
        0xcc, 0x00, 0x00, 0x33, 0xcc, 0x00, 0x00, 0x33, 0xcc, 0x00, 0x00, 0x33,
        0xcc, 0x00, 0x00, 0x33, 0xcc, 0x00, 0x00, 0x33, 0xcc, 0x00, 0x00, 0x33,
        0xcc, 0x00, 0x00, 0x33, 0xcf, 0xff, 0xff, 0xf3, 0xcf, 0xff, 0xff, 0xf3,
        0xc0, 0x00, 0x00, 0x03, 0xc0, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff,
        0xff, 0xff, 0xff, 0xff, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x0f, 0xf0, 0x00,
        0x00, 0x0f, 0xf0, 0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0xff, 0xff, 0x00,
        0x00, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

    int brightness = displayManager.getBrightness();
    brightness = map((brightness * -1), -255, 0, 0, 255);

    int lightBarY = map(brightness, 0, 255, bottomYBar, topYBar);
    int lightBarHeight = map(brightness, 0, 255, 0, 197);

    int displayBarY = map(brightness, 0, 255, bottomYBar, topYBar);
    int displayBarHeight = map(brightness, 0, 255, 0, 197);

    // Light level bar
    if (currentSelection == SettingsSelection::LightLevel) {
      display->drawRect(14, 14, 68, 204, highlightColor);
      display->drawRect(15, 15, 66, 202, highlightColor);
    } else {
      display->drawRect(14, 14, 68, 204, 0xFFFF);
      display->drawRect(15, 15, 66, 202, 0xFFFF);
    }
    display->drawRect(16, 16, 64, 200, 0xFFFF);
    display->fillRect(17, topYBar, 62, bottomYBar - topYBar,
                      0x0000); // clear bar area
    display->fillRect(17, lightBarY, 62, lightBarHeight, barColor);
    display->drawBitmap(33, 177, image_light_bits, 32, 32, 0xFFFF);

    // Display brightness bar
    if (currentSelection == SettingsSelection::DisplayBrightness) {
      display->drawRect(123, 14, 68, 204, highlightColor);
      display->drawRect(124, 15, 66, 202, highlightColor);
    } else {
      display->drawRect(123, 14, 68, 204, 0xFFFF);
      display->drawRect(124, 15, 66, 202, 0xFFFF);
    }
    display->drawRect(125, 16, 64, 200, 0xFFFF);
    display->fillRect(126, topYBar, 62, bottomYBar - topYBar,
                      0x0000); // clear bar area
    display->fillRect(126, displayBarY, 62, displayBarHeight, barColor);
    display->drawBitmap(141, 177, image_monitor_bits, 32, 32, 0xFFFF);
  } else {
    displayManager.showError("SettingsScreen::draw: display pointer is null");
  }
}

constexpr int changePerInput = 15;
InputEvent
SettingsScreen::handleInput(InputEvent input) // TODO: implement SW1 to switch
                                              // between off and last setting
{
  if (currentSelection == SettingsSelection::SettingsNone &&
      input != InputEvent::SW2 && input != InputEvent::SW3) {
    return input;
  } else {
    settingsChanged = true;
    switch (input) {
    case InputEvent::SW2:
      if (currentSelection == SettingsSelection::LightLevel) {
        currentSelection = SettingsSelection::SettingsNone;
      } else {
        currentSelection = SettingsSelection::LightLevel;
      }
      break;

    case InputEvent::SW3:
      if (currentSelection == SettingsSelection::DisplayBrightness) {
        currentSelection = SettingsSelection::SettingsNone;
      } else {
        currentSelection = SettingsSelection::DisplayBrightness;
      }
      break;

    case InputEvent::RotaryCw:
      if (currentSelection ==
          SettingsSelection::LightLevel) { // TODO: replace with actual light
                                           // level adjustment
        int brightness = displayManager.getBrightness();
        brightness -= changePerInput;
        displayManager.setBrightness(brightness);
      } else if (currentSelection == SettingsSelection::DisplayBrightness) {
        int brightness = displayManager.getBrightness();
        brightness -= changePerInput;
        displayManager.setBrightness(brightness);
      }
      break;

    case InputEvent::RotaryCcw:
      if (currentSelection ==
          SettingsSelection::LightLevel) { // TODO: replace with actual light
                                           // level adjustment
        int brightness = displayManager.getBrightness();
        brightness += changePerInput;
        displayManager.setBrightness(brightness);
      } else if (currentSelection == SettingsSelection::DisplayBrightness) {
        int brightness = displayManager.getBrightness();
        brightness += changePerInput;
        displayManager.setBrightness(brightness);
      }
      break;

    case InputEvent::SW1:
      if (currentSelection == SettingsSelection::LightLevel) {
        displayManager.setBrightness(255); // reset light level
      } else if (currentSelection == SettingsSelection::DisplayBrightness) {
        displayManager.setBrightness(255); // reset display brightness
      }
      break;

    default:
      settingsChanged = false;
      break;
    }

    return InputEvent::NoneInput;
  }
}

void SettingsScreen::end() {}