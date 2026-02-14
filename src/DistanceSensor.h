#pragma once

#include <stdint.h>
#include <Wire.h>
#include <vl53l4cd_class.h>

class DistanceSensor {
 public:
  DistanceSensor();
  bool begin(TwoWire& wirePort);
  int readDistance();

 private:
  VL53L4CD sensor;
  bool initialized_ = false;
};
