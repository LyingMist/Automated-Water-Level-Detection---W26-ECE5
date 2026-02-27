#pragma once

#include <stdint.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ST7735.h>

class DisplayDriver {
 public:
  DisplayDriver(uint8_t csPin, uint8_t dcPin, uint8_t rstPin);
  bool begin();
  void showSplash(const char* title, const char* subtitle);
  // Renders a large centered percentage with color-threshold styling.
  void updateDisplay(int distanceMM, float percentage, bool isTilted, bool isSloshing);
  void showError(const char* message);

 private:
  Adafruit_ST7735 tft_;
  bool initialized_ = false;
  bool hasLastPercentage_ = false;
  int lastDisplayedTenths_ = -1;
  bool hasLastTiltState_ = false;
  bool lastTiltState_ = false;
  bool hasLastSloshState_ = false;
  bool lastSloshState_ = false;

  void drawBaseFrame();
};
