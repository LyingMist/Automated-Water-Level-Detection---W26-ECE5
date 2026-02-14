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
constexpr uint8_t kMainValueTextSize = 4;
constexpr int16_t kPercentTightenPx = 4;
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

void DisplayDriver::updateDisplay(int distanceMM, float percentage) {
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
  if (hasLastPercentage_ && displayedTenths == lastDisplayedTenths_) {
    return;
  }

  lastDisplayedTenths_ = displayedTenths;
  hasLastPercentage_ = true;

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

  // Build helper strings only for centering calculations.
  char numberText[12];
  snprintf(numberText, sizeof(numberText), "%.1f", percentage);
  const char* percentSuffix = " %";

  tft_.setTextWrap(false);
  tft_.setTextSize(kMainValueTextSize);
  tft_.setTextColor(valueColor, kBackgroundColor);

  int16_t xNum = 0;
  int16_t yNum = 0;
  uint16_t wNum = 0;
  uint16_t hNum = 0;
  tft_.getTextBounds(numberText, 0, 0, &xNum, &yNum, &wNum, &hNum);

  int16_t xPct = 0;
  int16_t yPct = 0;
  uint16_t wPct = 0;
  uint16_t hPct = 0;
  tft_.getTextBounds(percentSuffix, 0, 0, &xPct, &yPct, &wPct, &hPct);

  const uint16_t combinedWidth = static_cast<uint16_t>(wNum + wPct - kPercentTightenPx);
  const uint16_t combinedHeight = (hNum > hPct) ? hNum : hPct;
  const int16_t x = static_cast<int16_t>((SCREEN_W - combinedWidth) / 2);
  const int16_t y = static_cast<int16_t>((SCREEN_H - combinedHeight) / 2 - yNum);

  tft_.setCursor(x, y);
  tft_.print(percentage, 1);

  const int16_t yPos = tft_.getCursorY();
  tft_.setCursor(tft_.getCursorX() - kPercentTightenPx, yPos);
  tft_.print(" %");
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
