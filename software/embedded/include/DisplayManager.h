#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "Screen.h"
#include <TFT_eSPI.h>

class DisplayManager {
private:
  TFT_eSPI *display;
  Screen *currentScreen;
  int currentScreenIndex;
  static const int MAX_SCREENS = 3;
  Screen *screens[MAX_SCREENS];
  int brightness;
  bool errorShown = false;
  bool notificationActive = false;
  unsigned long notificationExpiryMs = 0;

  void clearNotification();
  void playTone(uint16_t freq, uint16_t durationMs);
  void drawWrappedText(int16_t x, int16_t y, int16_t boxWidth,
                       int16_t boxHeight, String text);

public:
  DisplayManager();

  void init();
  void registerScreen(Screen *screen, int index);
  void setScreen(int index);
  void draw();
  void handleInput(InputEvent event); // pass to current screen or automatically
                                      // switch screens
  void setBrightness(int level);
  int getBrightness() { return brightness; }
  void showError(String message);
  void showMessage(String origin, String title, String message);

  // Get display pointer if needed elsewhere
  TFT_eSPI *getDisplay() { return display; }
};

extern DisplayManager displayManager; // make instance available globally

#endif