#include "DistanceSensor.h"

DistanceSensor::DistanceSensor() = default;

DistanceSensor::~DistanceSensor() {
  if (sensor_ != nullptr) {
    delete sensor_;
    sensor_ = nullptr;
  }
}

bool DistanceSensor::begin(TwoWire& wirePort) {
  if (sensor_ != nullptr) {
    delete sensor_;
    sensor_ = nullptr;
  }

  sensor_ = new VL53L4CD(&wirePort, -1);
  if (sensor_ == nullptr) {
    initialized_ = false;
    return false;
  }

  if (sensor_->begin() != 0) {
    initialized_ = false;
    return false;
  }

  if (sensor_->InitSensor() != 0) {
    initialized_ = false;
    return false;
  }

  if (sensor_->VL53L4CD_StartRanging() != 0) {
    initialized_ = false;
    return false;
  }

  initialized_ = true;
  return true;
}

int DistanceSensor::readDistance() {
  if (!initialized_ || sensor_ == nullptr) {
    return -1;
  }

  uint8_t newDataReady = 0;
  if (sensor_->VL53L4CD_CheckForDataReady(&newDataReady) != 0) {
    return -1;
  }

  if (!newDataReady) {
    return -1;
  }

  VL53L4CD_Result_t results;
  if (sensor_->VL53L4CD_GetResult(&results) != 0) {
    return -1;
  }

  if (sensor_->VL53L4CD_ClearInterrupt() != 0) {
    return -1;
  }

  return static_cast<int>(results.distance_mm);
}
