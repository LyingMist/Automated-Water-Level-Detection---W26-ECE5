#include <Arduino.h>
#include <Wire.h>
#include <math.h>
#include "DisplayDriver.h"
#include "DistanceSensor.h"
#include "TiltSensor.h"

namespace {
constexpr uint8_t PIN_I2C_SDA = D4;
constexpr uint8_t PIN_I2C_SCL = D5;
constexpr uint8_t PIN_TFT_CS = D2;
constexpr uint8_t PIN_TFT_DC = D1;
constexpr uint8_t PIN_TFT_RST = D0;

const int MIN_DISTANCE_MM = 50;
const int MAX_DISTANCE_MM = 850;
constexpr unsigned long SAMPLE_INTERVAL_MS = 200;

DistanceSensor distanceSensor;
TiltSensor tiltSensor;
DisplayDriver display(PIN_TFT_CS, PIN_TFT_DC, PIN_TFT_RST);
unsigned long lastSampleMs = 0;
bool hasStablePercent = false;
float stablePercent = 0.1f;
}  // namespace

void setup() {
  delay(1500);
  Serial.begin(115200);
  delay(200);
  Serial.println("XIAO ESP32C3 Water Level Monitor booting...");

  Wire.begin(PIN_I2C_SDA, PIN_I2C_SCL);
  Wire.setClock(100000);

  if (!display.begin()) {
    Serial.println("Display init failed");
  }
  display.showSplash("Water Level", "Initializing...");

  if (!distanceSensor.begin(Wire)) {
    Serial.println("VL53L4CD init failed");
    display.showError("Sensor init failed");
    return;
  }

  if (!tiltSensor.begin(&Wire)) {
    Serial.println("MPU6050 unavailable; tilt lock disabled");
  }

  display.showSplash("Water Level", "Sensor ready");
}

void loop() {
  const unsigned long now = millis();
  if (now - lastSampleMs < SAMPLE_INTERVAL_MS) {
    return;
  }
  lastSampleMs = now;

  // I2C priority: always read VL53L4CD first.
  const int distance = distanceSensor.readDistance();
  if (distance < 0) {
    Serial.println("Distance read failed");
    display.showError("Read failed");
    return;
  }

  bool tilted = false;
  bool sloshing = false;
  float ax = 0.0f;
  float ay = 0.0f;
  float az = 0.0f;
  bool accelAvailable = tiltSensor.readAcceleration(ax, ay, az);
  if (accelAvailable) {
    tilted = tiltSensor.isTilted();
    sloshing = tiltSensor.isSloshing();
  }

  const float compensatedDistance = static_cast<float>(distance);
  float percent = 100.0f * (1.0f - ((compensatedDistance - MIN_DISTANCE_MM) /
                                    (MAX_DISTANCE_MM - MIN_DISTANCE_MM)));
  if (percent < 0.0f) {
    percent = 0.0f;
  } else if (percent > 100.0f) {
    percent = 100.0f;
  }

  const bool sloshHold = accelAvailable && sloshing;

  float displayPercent = percent;
  if ((sloshHold || tilted )&& hasStablePercent) {
    displayPercent = stablePercent;
  } else {
    stablePercent = percent;
    hasStablePercent = true;
  }

  display.updateDisplay(distance, displayPercent, tilted, sloshHold);

  Serial.print("Distance (mm): ");
  Serial.print(distance);
  Serial.print(" | Comp(mm): ");
  Serial.print(compensatedDistance, 1);
  Serial.print(" | Percent (%): ");
  Serial.print(displayPercent, 1);
  Serial.print(" | Tilt:");
  Serial.print(tilted ? "Y" : "N");
  Serial.print(" | Slosh:");
  Serial.print(sloshHold ? "Y" : "N");
  if (accelAvailable) {
    Serial.print(" | Accel (m/s^2) X:");
    Serial.print(ax, 2);
    Serial.print(" Y:");
    Serial.print(ay, 2);
    Serial.print(" Z:");
    Serial.println(az, 2);
  } else {
    Serial.println(" | Accel: N/A");
  }
}