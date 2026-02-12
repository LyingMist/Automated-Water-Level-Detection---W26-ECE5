#include "DisplayDriver.h"

#include <Arduino.h>

namespace {
constexpr uint16_t SCREEN_W = 160;
constexpr uint16_t SCREEN_H = 80;
}  // namespace

DisplayDriver::DisplayDriver(uint8_t csPin, uint8_t dcPin, uint8_t rstPin)
    : tft_(csPin, dcPin, rstPin) {}

bool DisplayDriver::begin() {
  tft_.initR(INITR_MINI160x80);
  tft_.setRotation(3);
  tft_.fillScreen(ST77XX_BLACK);
  initialized_ = true;
  return true;
}

void DisplayDriver::drawBaseFrame() {
  if (!initialized_) {
    return;
  }

  tft_.fillScreen(ST77XX_BLACK);
  tft_.drawRect(0, 0, SCREEN_W, SCREEN_H, ST77XX_WHITE);
  tft_.setTextWrap(false);
}

void DisplayDriver::showSplash(const char* title, const char* subtitle) {
  if (!initialized_) {
    return;
  }

  drawBaseFrame();

  tft_.setTextColor(ST77XX_CYAN);
  tft_.setTextSize(1);
  tft_.setCursor(6, 20);
  tft_.print(title);

  tft_.setTextColor(ST77XX_WHITE);
  tft_.setTextSize(1);
  tft_.setCursor(6, 40);
  tft_.print(subtitle);
}

void DisplayDriver::showMeasurement(int distanceMm, bool levelLow) {
  if (!initialized_) {
    return;
  }

  drawBaseFrame();

  tft_.setTextSize(1);
  tft_.setTextColor(ST77XX_WHITE);
  tft_.setCursor(6, 6);
  tft_.print("Distance");

  tft_.setTextColor(ST77XX_YELLOW);
  tft_.setTextSize(2);
  tft_.setCursor(6, 22);
  tft_.print(distanceMm);
  tft_.print("mm");

  tft_.setTextSize(1);
  tft_.setCursor(6, 58);
  if (levelLow) {
    tft_.setTextColor(ST77XX_RED);
    tft_.print("Status: LOW");
  } else {
    tft_.setTextColor(ST77XX_GREEN);
    tft_.print("Status: OK");
  }
}

void DisplayDriver::showError(const char* message) {
  if (!initialized_) {
    return;
  }

  drawBaseFrame();
  tft_.setTextColor(ST77XX_RED);
  tft_.setTextSize(1);
  tft_.setCursor(6, 20);
  tft_.print("Error");
  tft_.setTextSize(1);
  tft_.setCursor(6, 40);
  tft_.print(message);
}
