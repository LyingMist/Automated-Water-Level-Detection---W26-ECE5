# XIAO ESP32C3 Water Level Monitor (Current Version: v1.0.4)

PlatformIO project for Seeed Studio XIAO ESP32C3 that measures water distance with a
VL53L4CD time-of-flight sensor and renders live status on a 0.96" IPS LCD (80x160)
driven by ST7735 over SPI.

**Current Version:** `v1.0.4`

## Currently Working On

- Implementing the sensor lens/housing and performing actual water level detection tests to calibrate the minimum and maximum distance values.

## Hardware

- **MCU:** Seeed Studio XIAO ESP32C3
- **Distance Sensor:** VL53L4CD (I2C)
- **Accelerometer:** MPU-6050 (I2C)
- **Display:** 0.96" IPS LCD, ST7735 controller, 80x160

## Software Architecture

- `src/DistanceSensor.h/.cpp`: VL53L4CD initialization and distance reads.
- `src/DisplayDriver.h/.cpp`: ST7735 initialization and UI rendering.
- `src/main.cpp`: board pin mapping, app loop, and integration logic.

Display initialization is configured specifically for this panel:

- `tft.initR(INITR_MINI160x80);`
- `tft.setRotation(3);`

## Project Structure

```text
ECE5_Xiao_Display/
├─ src/
│  ├─ main.cpp
│  ├─ DistanceSensor.h
│  ├─ DistanceSensor.cpp
│  ├─ TiltSensor.h
│  ├─ TiltSensor.cpp
│  ├─ DisplayDriver.h
│  └─ DisplayDriver.cpp
├─ images/
│  ├─ progress_pictures/
│  │  ├─ .gitkeep
│  │  └─ breadboard_wiring/
│  │     └─ FullComp.jpg
│  └─ pinouts/
│     ├─ .gitkeep
│     └─ XIAO ESP32C3 .png
├─ platformio.ini
├─ .gitignore
└─ README.md
```

## Hardware Wiring Map

### XIAO ESP32C3 <-> VL53L4CD (I2C)

| VL53L4CD Pin | XIAO ESP32C3 Pin | Notes |
|---|---|---|
| `VIN` | `3V3` | Power |
| `GND` | `GND` | Ground |
| `SCL` | `D5` | I2C clock |
| `SDA` | `D4` | I2C data |

Note: `XSHUT` and `GPIO` are intentionally left disconnected.

### XIAO ESP32C3 <-> MPU-6050 (I2C)

| MPU-6050 Pin | XIAO ESP32C3 Pin | Notes |
|---|---|---|
| `VCC` | `3V3` | Power |
| `GND` | `GND` | Ground |
| `SDA` | `D4` | Shared I2C data bus |
| `SCL` | `D5` | Shared I2C clock bus |

### XIAO ESP32C3 <-> ST7735 Display (SPI)

| ST7735 Pin | XIAO ESP32C3 Pin | Notes |
|---|---|---|
| `GND` | `GND` | Ground |
| `VCC` | `3V3` | Power |
| `SCL` | `D8` | SPI clock |
| `SDA` | `D10` | SPI MOSI |
| `RES` | `D0` | Reset |
| `DC` | `D1` | Data/command |
| `CS` | `D2` | Chip select |

Note: `BLK` is left disconnected as the display defaults to ON.

Reference images for this wiring are available in the `images/` folder.

## Build and Flash

1. Clone repository:

```bash
git clone https://github.com/LyingMist/Automated-Water-Level-Detection---W26-ECE5.git
cd Automated-Water-Level-Detection---W26-ECE5
```

2. Build firmware:

```bash
pio run
```

3. Upload firmware:

```bash
pio run --target upload
```

4. Monitor logs:

```bash
pio device monitor -b 115200
```

## PlatformIO Notes

- USB CDC output on boot is enabled in `platformio.ini` with:
  `-DARDUINO_USB_CDC_ON_BOOT=1`
- Required libraries are pinned in `platformio.ini`:
  - STM32duino VL53L4CD
  - Adafruit GFX Library
  - Adafruit ST7735 and ST7789 Library

## Project Images

Physical build progress photos are stored in `images/progress_pictures/`.
Microcontroller pinout references are stored in `images/pinouts/`.
Both folders currently include `.gitkeep` placeholders so the structure is tracked in Git.

## Version History

### V1.0.4 (Current)

- **Code Improvements:** Successfully wired and integrated the MPU-6050 accelerometer onto the shared I2C bus alongside the VL53L4CD. Implemented initial `TiltSensor` class to detect angled readings. Fixed severe ST7735 display bugs by removing unreliable dynamic bounds calculations (`getTextBounds`) in favor of stable, hardcoded absolute cursor positioning.
- **Logistics & Formatting Changes:** Added Wiring Map for MPU-6050 and updating the breadboard_wiring pictures.

### V1.0.3

- **Code Improvements:** Successfully integrated sensor data with the display. Implemented 'Water Bottle Logic' (inverted distance calculation where closer = higher percentage). Added logic to clamp values between 0-100%.
- **Logistics & Formatting Changes:** Identified and fixed BGR vs RGB color mapping for the ST7735 panel. Adjusted typography to use large, readable fonts with tightened spacing for the '%' symbol. Removed unused default directories (`lib/`, `include/`).

### V1.0.2

- **Code Improvements:** Replaced the incompatible Adafruit VL53L4CD library with the official `stm32duino/STM32duino VL53L4CD` library. Refactored the sensor header and source files to utilize the correct STM32 C++ API.
- **Logistics & Formatting Changes:** Added `images/` folder structure for physical build references and added a verified hardware pin mapping guide to the README.

### V1.0.1

- **Code Improvements:** Changed display output from raw millimeters to water level percentage with 3-significant-figure precision.
- **Logistics & Formatting Changes:** Updated README details for display behavior and version tracking.

### V1.0.0

- **Code Improvements:** Initial release with basic distance detection and sensor/display integration.
- **Logistics & Formatting Changes:** Established initial project structure and baseline documentation.
