#ifndef MELODY_MANAGER_H
#define MELODY_MANAGER_H

#include <Arduino.h>

class MelodyManager {
private:
    int _buzzerPin;
    bool _initialized;
    void playTone(int frequency, int duration);
    void playMelody(const int melody[], const int durations[], int noteCount);

public:
  void init(int buzzerPin);
  void playInfo();
  void playSuccess();
  void playError();
  void stop();
};

#endif // MELODY_MANAGER_H
