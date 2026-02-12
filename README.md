# XIAO ESP32C3 Water Level Monitor (Current Version: V1.0.1)

PlatformIO project for Seeed Studio XIAO ESP32C3 that measures water distance with a
VL53L4CD time-of-flight sensor and renders live status on a 0.96" IPS LCD (80x160)
driven by ST7735 over SPI.

**Current Version:** `V1.0.1`

## Hardware

- **MCU:** Seeed Studio XIAO ESP32C3
- **Distance Sensor:** VL53L4CD (I2C)
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
│  ├─ DisplayDriver.h
│  └─ DisplayDriver.cpp
├─ platformio.ini
├─ .gitignore
└─ README.md
```

## Wiring Table

### XIAO ESP32C3 <-> VL53L4CD (I2C)

| XIAO Pin | VL53L4CD Pin | Purpose |
|---|---|---|
| `3V3` | `VIN` | Power |
| `GND` | `GND` | Ground |
| `D4 (GPIO6)` | `SDA` | I2C Data |
| `D5 (GPIO7)` | `SCL` | I2C Clock |

### XIAO ESP32C3 <-> ST7735 Display (SPI)

| XIAO Pin | Display Pin | Purpose |
|---|---|---|
| `3V3` | `VCC` | Power |
| `GND` | `GND` | Ground |
| `D8 (GPIO8)` | `SCLK` | SPI Clock |
| `D10 (GPIO10)` | `MOSI` | SPI Data |
| `D2 (GPIO2)` | `CS` | Chip Select |
| `D1 (GPIO1)` | `DC` | Data/Command |
| `D0 (GPIO0)` | `RST` | Display Reset |

## Build and Flash

1. Clone repository:

```bash
git clone https://github.com/<your-username>/ECE5_Xiao_Display.git
cd ECE5_Xiao_Display
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
  - Adafruit VL53L4CD
  - Adafruit GFX Library
  - Adafruit ST7735 and ST7789 Library

## Version History

- **V1.0.0 - Initial Release:** Basic distance detection and hardware setup (Sensor + Display).
- **V1.0.1 (Current) - Feature Update:** Changed display output from raw millimeters to Water Level Percentage with 3-significant-figure precision.
