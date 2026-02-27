#include "DisplayDriver.h"

#include <Arduino.h>
#include <math.h>
#include <stdio.h>

namespace {
constexpr uint16_t SCREEN_W = 160;
constexpr uint16_t SCREEN_H = 80;
constexpr uint16_t kBackgroundColor = ST77XX_BLACK;
constexpr uint16_t kCustomOrangeColor = 0x45DF;  // BGR-swapped for #fdb341
constexpr uint16_t kCustomRedColor = 0x0811;     // BGR-swapped for #8f000b
constexpr uint16_t kSloshColor = 0xD5B3;         // BGR565 for #9EB9DB
constexpr uint16_t kTiltColor = 0x9EDA;          // BGR565 for #D8DB9E
constexpr uint8_t kMainValueTextSize = 3;
constexpr int16_t kPercentTightenPx = 4;
constexpr int16_t kWarnMarginRight = 4;
constexpr int16_t kWarnMarginBottom = 3;
constexpr int16_t kWarnBoxWidth = 36;   // Fits "SLOSH" at text size 1
constexpr int16_t kWarnBoxHeight = 10;  // One text row + small padding
}  // namespace

DisplayDriver::DisplayDriver(uint8_t csPin, uint8_t dcPin, uint8_t rstPin)
    : tft_(csPin, dcPin, rstPin) {}

bool DisplayDriver::begin() {
  tft_.initR(INITR_MINI160x80);
  tft_.setRotation(3);
  tft_.fillScreen(kBackgroundColor);
  initialized_ = true;
  return true;
}

void DisplayDriver::drawBaseFrame() {
  if (!initialized_) {
    return;
  }

  tft_.fillScreen(kBackgroundColor);
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

void DisplayDriver::updateDisplay(int distanceMM, float percentage, bool isTilted, bool isSloshing) {
  if (!initialized_) {
    return;
  }

  (void)distanceMM;

  if (percentage < 0.0f) {
    percentage = 0.0f;
  } else if (percentage > 100.0f) {
    percentage = 100.0f;
  }

  const int displayedTenths = static_cast<int>(lroundf(percentage * 10.0f));
  const bool hasStateHistory = hasLastTiltState_ && hasLastSloshState_;
  if (hasLastPercentage_ && displayedTenths == lastDisplayedTenths_ && hasStateHistory &&
      lastTiltState_ == isTilted && lastSloshState_ == isSloshing) {
    return;
  }

  lastDisplayedTenths_ = displayedTenths;
  hasLastPercentage_ = true;
  hasLastTiltState_ = true;
  lastTiltState_ = isTilted;
  hasLastSloshState_ = true;
  lastSloshState_ = isSloshing;

  tft_.fillScreen(kBackgroundColor);

  uint16_t valueColor = ST77XX_WHITE;
  if (percentage > 90.0f) {
    valueColor = ST77XX_GREEN;
  } else if (percentage >= 60.0f) {
    valueColor = ST77XX_WHITE;
  } else if (percentage >= 20.0f) {
    valueColor = kCustomOrangeColor;
  } else {
    valueColor = kCustomRedColor;
  }

  tft_.setTextWrap(false);
  tft_.setTextSize(kMainValueTextSize);
  tft_.setTextColor(valueColor, kBackgroundColor);
  // Keep stable value placement for predictable redraws.
  tft_.setCursor(20, 30);
  tft_.print(percentage, 1);

  const int16_t yPos = tft_.getCursorY();
  tft_.setCursor(tft_.getCursorX() - kPercentTightenPx, yPos);
  tft_.print(" %");

  // Bottom-right warning area with strict priority: SLOSH over TILT.
  const int16_t warnX = SCREEN_W - kWarnBoxWidth - kWarnMarginRight;
  const int16_t warnY = SCREEN_H - kWarnBoxHeight - kWarnMarginBottom;
  const int16_t warnTextY = warnY + 1;
  tft_.fillRect(warnX, warnY, kWarnBoxWidth, kWarnBoxHeight, kBackgroundColor);
  tft_.setTextSize(1);

  if (isSloshing) {
    tft_.setTextColor(kSloshColor, kBackgroundColor);
    tft_.setCursor(warnX, warnTextY);
    tft_.print("SLOSH");
  } else if (isTilted) {
    tft_.setTextColor(kTiltColor, kBackgroundColor);
    tft_.setCursor(warnX + 7, warnTextY);  // Right-align shorter label in same box.
    tft_.print("TILT");
  }
}

void DisplayDriver::showError(const char* message) {
  if (!initialized_) {
    return;
  }

  drawBaseFrame();
  tft_.setTextColor(kCustomRedColor, ST77XX_BLACK);
  tft_.setTextSize(1);
  tft_.setCursor(6, 20);
  tft_.print("Error");
  tft_.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
  tft_.setCursor(6, 40);
  tft_.print(message);
}
