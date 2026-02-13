#include "DistanceSensor.h"

DistanceSensor::DistanceSensor() : sensor(&Wire, -1) {}

bool DistanceSensor::begin(TwoWire& wirePort) {
  (void)wirePort;

  Wire.begin();

  if (sensor.begin() != 0) {
    initialized_ = false;
    return false;
  }

  if (sensor.InitSensor() != 0) {
    initialized_ = false;
    return false;
  }

  if (sensor.VL53L4CD_StartRanging() != 0) {
    initialized_ = false;
    return false;
  }

  initialized_ = true;
  return true;
}

int DistanceSensor::readDistanceMm() {
  if (!initialized_) {
    return -1;
  }

  uint8_t newDataReady = 0;
  if (sensor.VL53L4CD_CheckForDataReady(&newDataReady) != 0) {
    return -1;
  }

  if (!newDataReady) {
    return -1;
  }

  VL53L4CD_Result_t results;
  if (sensor.VL53L4CD_GetResult(&results) != 0) {
    return -1;
  }

  if (sensor.VL53L4CD_ClearInterrupt() != 0) {
    return -1;
  }

  return static_cast<int>(results.distance_mm);
}
