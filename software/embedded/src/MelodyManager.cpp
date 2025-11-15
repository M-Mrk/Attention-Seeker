#include "MelodyManager.h"

#define NOTE_C4 262
#define NOTE_D4 294
#define NOTE_E4 330
#define NOTE_F4 349
#define NOTE_G4 392
#define NOTE_A4 440
#define NOTE_B4 494
#define NOTE_C5 523
#define NOTE_D5 587
#define NOTE_E5 659
#define NOTE_F5 698
#define NOTE_G5 784

void MelodyManager::init(int buzzerPin)
{
    _buzzerPin = buzzerPin;
    _initialized = true;

    pinMode(_buzzerPin, OUTPUT);
    digitalWrite(_buzzerPin, LOW);
}

void MelodyManager::playTone(int frequency, int duration)
{
    if (!_initialized)
    {
        Serial.println("MelodyManager not initialized!");
        return;
    }

    if (frequency > 0)
    {
        // Use ledcWrite for ESP32 tone generation
        ledcSetup(0, frequency, 8); // channel 0, frequency, 8-bit resolution
        ledcAttachPin(_buzzerPin, 0);
        ledcWrite(0, 128); // 50% duty cycle
        delay(duration);
        ledcWrite(0, 0); // Stop tone
    }
    else
    {
        delay(duration);
    }
}

void MelodyManager::playMelody(const int melody[], const int durations[], int noteCount)
{
    if (!_initialized)
    {
        Serial.println("MelodyManager not initialized!");
        return;
    }

    for (int i = 0; i < noteCount; i++)
    {
        playTone(melody[i], durations[i]);
        // Small pause between notes
        delay(10);
    }

    // Ensure buzzer is off after melody
    stop();
}

void MelodyManager::playInfo()
{
    // Single medium-pitched beep
    const int melody[] = {NOTE_A4};
    const int durations[] = {150};

    playMelody(melody, durations, 1);
}

void MelodyManager::playSuccess()
{
    // Ascending three note melody
    const int melody[] = {NOTE_C5, NOTE_E5, NOTE_G5};
    const int durations[] = {100, 100, 200};

    playMelody(melody, durations, 3);
}

void MelodyManager::playError()
{
    // Descending three note melody with lower tone
    const int melody[] = {NOTE_F4, NOTE_D4, NOTE_C4};
    const int durations[] = {150, 300, 150};

    playMelody(melody, durations, 3);
}

void MelodyManager::stop()
{
    if (!_initialized)
    {
        return;
    }

    ledcWrite(0, 0);
    digitalWrite(_buzzerPin, LOW);
}
