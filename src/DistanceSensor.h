#pragma once

#include <stdint.h>
#include <Adafruit_VL53L4CD.h>
#include <Wire.h>

class DistanceSensor {
 public:
  bool begin(TwoWire& wirePort);
  int readDistanceMm();

 private:
  Adafruit_VL53L4CD sensor_;
  bool initialized_ = false;
};
