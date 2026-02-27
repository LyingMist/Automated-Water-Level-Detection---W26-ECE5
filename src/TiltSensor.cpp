#include "TiltSensor.h"

#include <Arduino.h>
#include <math.h>

namespace {
constexpr float kTiltThresholdDegrees = 15.0f;
constexpr float kSloshThresholdMs2 = 1.5f;
constexpr int kCalibrationSamples = 20;
constexpr unsigned long kCalibrationSampleDelayMs = 10;
constexpr float kRadToDeg = 57.2957795f;
}

bool TiltSensor::begin(TwoWire* wireBus) {
  if (wireBus == nullptr || !mpu_.begin(0x68, wireBus)) {
    initialized_ = false;
    return false;
  }

  float sumX = 0.0f;
  float sumY = 0.0f;
  float sumZ = 0.0f;
  int validSamples = 0;
  for (int i = 0; i < kCalibrationSamples; ++i) {
    sensors_event_t accel;
    sensors_event_t gyro;
    sensors_event_t temp;
    mpu_.getEvent(&accel, &gyro, &temp);
    sumX += accel.acceleration.x;
    sumY += accel.acceleration.y;
    sumZ += accel.acceleration.z;
    ++validSamples;
    delay(kCalibrationSampleDelayMs);
  }
  if (validSamples <= 0) {
    initialized_ = false;
    return false;
  }

  baselineX_ = sumX / static_cast<float>(validSamples);
  baselineY_ = sumY / static_cast<float>(validSamples);
  baselineZ_ = sumZ / static_cast<float>(validSamples);

  hasPreviousAxes_ = false;
  initialized_ = true;
  return true;
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

void TiltSensor::calculateRollPitchDegrees(float ax,
                                           float ay,
                                           float az,
                                           float& rollDeg,
                                           float& pitchDeg) {
  rollDeg = atan2f(ay, az) * kRadToDeg;
  pitchDeg = atan2f(-ax, sqrtf((ay * ay) + (az * az))) * kRadToDeg;
}

float TiltSensor::smallestAngleDeltaDegrees(float aDeg, float bDeg) {
  float delta = aDeg - bDeg;
  while (delta > 180.0f) {
    delta -= 360.0f;
  }
  while (delta < -180.0f) {
    delta += 360.0f;
  }
  return fabsf(delta);
}

bool TiltSensor::isTilted() {
  float ax = 0.0f;
  float ay = 0.0f;
  float az = 0.0f;
  if (!readAcceleration(ax, ay, az)) {
    return false;
  }

  float baselineRoll = 0.0f;
  float baselinePitch = 0.0f;
  calculateRollPitchDegrees(baselineX_, baselineY_, baselineZ_, baselineRoll, baselinePitch);

  float currentRoll = 0.0f;
  float currentPitch = 0.0f;
  calculateRollPitchDegrees(ax, ay, az, currentRoll, currentPitch);

  const float rollDelta = smallestAngleDeltaDegrees(currentRoll, baselineRoll);
  const float pitchDelta = smallestAngleDeltaDegrees(currentPitch, baselinePitch);
  return (rollDelta > kTiltThresholdDegrees) || (pitchDelta > kTiltThresholdDegrees);
}

bool TiltSensor::isSloshing() {
  float ax = 0.0f;
  float ay = 0.0f;
  float az = 0.0f;
  if (!readAcceleration(ax, ay, az)) {
    return false;
  }

  if (!hasPreviousAxes_) {
    previousX_ = ax;
    previousY_ = ay;
    previousZ_ = az;
    hasPreviousAxes_ = true;
    return false;
  }

  const float dx = fabsf(ax - previousX_);
  const float dy = fabsf(ay - previousY_);
  const float dz = fabsf(az - previousZ_);
  const float previousMagnitude =
      sqrtf((previousX_ * previousX_) + (previousY_ * previousY_) + (previousZ_ * previousZ_));
  const float currentMagnitude = sqrtf((ax * ax) + (ay * ay) + (az * az));
  const float magnitudeDelta = fabsf(currentMagnitude - previousMagnitude);

  previousX_ = ax;
  previousY_ = ay;
  previousZ_ = az;

  return dx > kSloshThresholdMs2 || dy > kSloshThresholdMs2 || dz > kSloshThresholdMs2 ||
         magnitudeDelta > kSloshThresholdMs2;
}

bool TiltSensor::available() const { return initialized_; }
