#include "screens/TimeScreen.h"
#include "DisplayManager.h"

#include <ESP32Time.h>

void updateTimeTask(void *pvParameters) {
  TimeScreen *timeScreen = static_cast<TimeScreen *>(pvParameters);
  for (;;) {
    timeScreen->updateTime();
    vTaskDelay(pdMS_TO_TICKS(60000)); // Update every minute
  }
}

void TimeScreen::init(TFT_eSPI *display) {
  display->fillScreen(0x0);
  timeRedraw = true;
  dateRedraw = true;

  lastMinutes = -1;
  lastDay = -1;
  updateTime();

  xTaskCreatePinnedToCore(updateTimeTask, "TimeUpdateTask", 2048, this, 1,
                          &timeUpdateTaskHandle, 1);
}

void TimeScreen::draw(TFT_eSPI *display) {
  if (display) {
    if (!timeRedraw && !dateRedraw)
      return;

    if (timeRedraw) {
      display->fillRect(39, 91, 245, 83, 0x0);

      display->setTextColor(0xFFFF);
      display->setTextSize(3);
      display->setTextWrap(false);
      display->setFreeFont(
          &FreeSerif18pt7b); // ignore error. Font is included in TFT_eSPI
      display->setCursor(41, 166);
      display->print(hoursText);
      timeRedraw = false;
    }

    if (dateRedraw) {
      display->fillRect(77, 40, 173, 33, 0x0);

      display->setTextSize(1);
      display->setCursor(83, 67);
      display->print(dateText);
      dateRedraw = false;
    }
  } else {
    displayManager.showError("Display pointer is null");
  }
}

void TimeScreen::end() { vTaskDelete(timeUpdateTaskHandle); }

void TimeScreen::updateTime() {
  ESP32Time rtc;
  int minutes = rtc.getMinute();

  if (minutes == lastMinutes)
    return;
  lastMinutes = minutes;
  timeRedraw = true;

  int hours = rtc.getHour(true);

  char buffer[6];
  snprintf(buffer, sizeof(buffer), "%02d:%02d", hours, minutes);
  hoursText = String(buffer);

  int day = rtc.getDay();
  if (day == lastDay)
    return;
  lastDay = day;
  dateRedraw = true;

  int month = rtc.getMonth() + 1; // rtc months are 0-based
  int year = rtc.getYear();

  char dateBuffer[12];
  snprintf(dateBuffer, sizeof(dateBuffer), "%02d/%02d/%04d", day, month, year);
  dateText = String(dateBuffer);
}