#include "TiltSensor.h"

#include <math.h>

namespace {
constexpr float kTiltThresholdMs2 = 2.0f;
}

bool TiltSensor::begin(TwoWire* wireBus) {
  if (wireBus == nullptr || !mpu_.begin(0x68, wireBus)) {
    initialized_ = false;
    return false;
  }

  initialized_ = true;
  return true;
}

bool TiltSensor::isTilted() {
  if (!initialized_) {
    return false;
  }

  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  mpu_.getEvent(&accel, &gyro, &temp);

  return fabsf(accel.acceleration.x) > kTiltThresholdMs2 ||
         fabsf(accel.acceleration.y) > kTiltThresholdMs2;
}

bool TiltSensor::readAcceleration(float& ax, float& ay, float& az) {
  if (!initialized_) {
    return false;
  }

  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  mpu_.getEvent(&accel, &gyro, &temp);

  ax = accel.acceleration.x;
  ay = accel.acceleration.y;
  az = accel.acceleration.z;
  return true;
}

bool TiltSensor::available() const { return initialized_; }
