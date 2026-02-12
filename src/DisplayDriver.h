#pragma once

#include <stdint.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

class DisplayDriver {
 public:
  DisplayDriver(uint8_t csPin, uint8_t dcPin, uint8_t rstPin);
  bool begin();
  void showSplash(const char* title, const char* subtitle);
  void showMeasurement(int distanceMm);
  void showError(const char* message);

 private:
  Adafruit_ST7735 tft_;
  bool initialized_ = false;
  bool hasLastPercentage_ = false;
  float lastPercentage_ = 0.0f;

  void drawBaseFrame();
};
