#include <Arduino.h>
#include <Wire.h>
#include "DisplayDriver.h"
#include "DistanceSensor.h"

namespace {
constexpr uint8_t PIN_I2C_SDA = D4;
constexpr uint8_t PIN_I2C_SCL = D5;
constexpr uint8_t PIN_TFT_CS = D2;
constexpr uint8_t PIN_TFT_DC = D1;
constexpr uint8_t PIN_TFT_RST = D0;

const int MIN_DISTANCE_MM = 50;
const int MAX_DISTANCE_MM = 300;
constexpr unsigned long SAMPLE_INTERVAL_MS = 500;

DistanceSensor distanceSensor;
DisplayDriver display(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);
unsigned long lastSampleMs = 0;
}  // namespace

void setup() {
  Serial.begin(115200);
  delay(200);
  Serial.println("XIAO ESP32C3 Water Level Monitor booting...");

  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);

  if (!display.begin()) {
    Serial.println("Display init failed");
  }
  display.showSplash("Water Level", "Initializing...");

  if (!distanceSensor.begin(Wire)) {
    Serial.println("VL53L4CD init failed");
    display.showError("Sensor init failed");
    return;
  }

  display.showSplash("Water Level", "Sensor ready");
}

void loop() {
  const unsigned long now = millis();
  if (now - lastSampleMs < SAMPLE_INTERVAL_MS) {
    return;
  }
  lastSampleMs = now;

  const int distance = distanceSensor.readDistance();
  if (distance < 0) {
    Serial.println("Distance read failed");
    display.showError("Read failed");
    return;
  }

  float percent = 100.0f * (1.0f - (static_cast<float>(distance - MIN_DISTANCE_MM) /
                                    (MAX_DISTANCE_MM - MIN_DISTANCE_MM)));
  if (percent < 0.0f) {
    percent = 0.0f;
  } else if (percent > 100.0f) {
    percent = 100.0f;
  }

  display.updateDisplay(distance, percent);

  Serial.print("Distance (mm): ");
  Serial.print(distance);
  Serial.print(" | Percent (%): ");
  Serial.println(percent, 1);
}