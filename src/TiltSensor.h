#pragma once

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

class TiltSensor {
 public:
  bool begin(TwoWire* wireBus);
  bool isTilted();
  bool isSloshing();
  bool readAcceleration(float& ax, float& ay, float& az);
  bool available() const;

 private:
  static void calculateRollPitchDegrees(float ax,
                                        float ay,
                                        float az,
                                        float& rollDeg,
                                        float& pitchDeg);
  static float smallestAngleDeltaDegrees(float aDeg, float bDeg);

  Adafruit_MPU6050 mpu_;
  bool initialized_ = false;
  float baselineX_ = 0.0f;
  float baselineY_ = 0.0f;
  float baselineZ_ = 0.0f;
  bool hasPreviousAxes_ = false;
  float previousX_ = 0.0f;
  float previousY_ = 0.0f;
  float previousZ_ = 0.0f;
};
