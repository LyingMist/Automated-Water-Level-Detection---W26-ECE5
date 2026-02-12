#include "DisplayDriver.h"

#include <Arduino.h>
#include <math.h>
#include <stdio.h>

namespace {
constexpr uint16_t SCREEN_W = 160;
constexpr uint16_t SCREEN_H = 80;
constexpr float TANK_DEPTH_MM = 500.0f;
constexpr float SENSOR_OFFSET_MM = 50.0f;
constexpr float REDRAW_DELTA_PERCENT = 0.10f;

float clampPercent(float value) {
  if (value < 0.0f) {
    return 0.0f;
  }
  if (value > 100.0f) {
    return 100.0f;
  }
  return value;
}

float distanceToWaterPercent(int distanceMm) {
  const float depthSpan = TANK_DEPTH_MM - SENSOR_OFFSET_MM;
  if (depthSpan <= 0.0f) {
    return 0.0f;
  }

  const float normalized =
      (static_cast<float>(distanceMm) - SENSOR_OFFSET_MM) / depthSpan;
  const float waterPercent = (1.0f - normalized) * 100.0f;
  return clampPercent(waterPercent);
}

void formatThreeSignificant(float value, char* outBuffer, size_t outSize) {
  if (value >= 99.95f) {
    snprintf(outBuffer, outSize, "%.0f", value);
  } else if (value >= 9.995f) {
    snprintf(outBuffer, outSize, "%.1f", value);
  } else {
    snprintf(outBuffer, outSize, "%.2f", value);
  }
}
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

void DisplayDriver::showMeasurement(int distanceMm) {
  if (!initialized_) {
    return;
  }

  const float waterPercent = distanceToWaterPercent(distanceMm);
  if (hasLastPercentage_ &&
      fabsf(waterPercent - lastPercentage_) < REDRAW_DELTA_PERCENT) {
    return;
  }

  lastPercentage_ = waterPercent;
  hasLastPercentage_ = true;

  drawBaseFrame();

  tft_.setTextSize(1);
  tft_.setTextColor(ST77XX_WHITE);
  tft_.setCursor(6, 6);
  tft_.print("Water Level");

  tft_.setTextColor(ST77XX_YELLOW);
  tft_.setTextSize(2);
  tft_.setCursor(6, 22);
  char percentText[12];
  formatThreeSignificant(waterPercent, percentText, sizeof(percentText));
  tft_.print(percentText);
  tft_.print("%");

  tft_.setTextSize(1);
  tft_.setCursor(6, 58);
  if (waterPercent < 20.0f) {
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
