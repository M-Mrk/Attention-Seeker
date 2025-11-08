/***************************************************
  This is our GFX example for the Adafruit ILI9341 Breakout and Shield
  ----> http://www.adafruit.com/products/1651

  Check out the links above for our tutorials and wiring diagrams
  These displays use SPI to communicate, 4 or 5 pins are required to
  interface (RST is optional)
  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 ****************************************************/

#include <Arduino.h>

#include "pins.h"

#include <Adafruit_ILI9341.h>


Adafruit_ILI9341 display = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_MOSI, TFT_CLK, TFT_RST, TFT_MISO);

const int16_t
  bits        = 20,   // Fractional resolution
  pixelWidth  = 320,  // TFT dimensions
  pixelHeight = 240,
  iterations  = 128;  // Fractal iteration limit or 'dwell'
float
  centerReal  = -0.6, // Image center point in complex plane
  centerImag  =  0.0,
  rangeReal   =  3.0, // Image coverage in complex plane
  rangeImag   =  3.0;

int lastPWM = 10;

void setup(void) {
  Serial.begin(115200);
  Serial.print("Mandelbrot drawer!");

  pinMode(BACKLIGHT_PIN, OUTPUT);
  analogWrite(BACKLIGHT_PIN, lastPWM);

  display.begin();
  display.setRotation(1);
  display.fillScreen(ILI9341_BLACK);
}

void loop() {
  lastPWM += 20;
  if (lastPWM > 255) {
    lastPWM = 10;
  }
  analogWrite(BACKLIGHT_PIN, lastPWM);

  int64_t       n, a, b, a2, b2, posReal, posImag;
  uint32_t      startTime,elapsedTime;

  int32_t
    startReal   = (int64_t)((centerReal - rangeReal * 0.5)   * (float)(1 << bits)),
    startImag   = (int64_t)((centerImag + rangeImag * 0.5)   * (float)(1 << bits)),
    incReal     = (int64_t)((rangeReal / (float)pixelWidth)  * (float)(1 << bits)),
    incImag     = (int64_t)((rangeImag / (float)pixelHeight) * (float)(1 << bits));
  
  startTime = millis();
  posImag = startImag;
  for (int y = 0; y < pixelHeight; y++) {
    posReal = startReal;
    for (int x = 0; x < pixelWidth; x++) {
      a = posReal;
      b = posImag;
      for (n = iterations; n > 0 ; n--) {
        a2 = (a * a) >> bits;
        b2 = (b * b) >> bits;
        if ((a2 + b2) >= (4 << bits)) 
          break;
        b  = posImag + ((a * b) >> (bits - 1));
        a  = posReal + a2 - b2;
      }
      
      // Convert iteration count to color and draw pixel
      uint16_t color;
      if (n == 0) {
        color = ILI9341_BLACK; // Point is in the set
      } else {
        // Create a color based on iteration count
        uint8_t colorIndex = (n * 255) / iterations;
        color = display.color565(colorIndex, colorIndex/2, 255-colorIndex);
      }
      display.drawPixel(x, y, color);
      
      posReal += incReal;
    }
    posImag -= incImag;
  }
  elapsedTime = millis()-startTime;
  Serial.print("Took "); Serial.print(elapsedTime); Serial.println(" ms");

  rangeReal *= 0.95;
  rangeImag *= 0.95;
}