#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include "DisplayDriver.h"
#include "DistanceSensor.h"

namespace {
constexpr uint8_t PIN_I2C_SDA = 6;    // XIAO D4
constexpr uint8_t PIN_I2C_SCL = 7;    // XIAO D5
constexpr uint8_t PIN_SPI_SCK = 8;    // XIAO D8
constexpr uint8_t PIN_SPI_MOSI = 10;  // XIAO D10
constexpr uint8_t PIN_TFT_CS = 2;     // XIAO D2
constexpr uint8_t PIN_TFT_DC = 1;     // XIAO D1
constexpr uint8_t PIN_TFT_RST = 0;    // XIAO D0

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
  SPI.begin(PIN_SPI_SCK, -1, PIN_SPI_MOSI, PIN_TFT_CS);

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

  const int distanceMm = distanceSensor.readDistanceMm();
  if (distanceMm < 0) {
    Serial.println("Distance read failed");
    display.showError("Read failed");
    return;
  }

  display.showMeasurement(distanceMm);

  Serial.print("Distance (mm): ");
  Serial.println(distanceMm);
}