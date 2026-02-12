#include "DistanceSensor.h"

bool DistanceSensor::begin(TwoWire& wirePort) {
  if (!sensor_.begin(0x29, &wirePort)) {
    initialized_ = false;
    return false;
  }

  sensor_.setRangeTiming(50, 0);
  sensor_.startRanging();
  initialized_ = true;
  return true;
}

int DistanceSensor::readDistanceMm() {
  if (!initialized_) {
    return -1;
  }

  if (!sensor_.dataReady()) {
    return -1;
  }

  VL53L4CD_Result_t result;
  if (!sensor_.getResult(&result)) {
    return -1;
  }

  sensor_.clearInterrupt();
  return static_cast<int>(result.distance_mm);
}
