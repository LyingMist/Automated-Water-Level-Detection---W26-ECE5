#pragma once

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

class TiltSensor {
 public:
  bool begin(TwoWire* wireBus);
  bool isTilted();
  bool readAcceleration(float& ax, float& ay, float& az);
  bool available() const;

 private:
  Adafruit_MPU6050 mpu_;
  bool initialized_ = false;
};
