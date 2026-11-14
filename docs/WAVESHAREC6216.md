# Waveshare ESP32-C6-Touch-AMOLED-2.16, experimental

This profile is for the ESP32-C6-Touch-AMOLED-2.16 (SKU 34202) and the battery-free
ESP32-C6-Touch-AMOLED-2.16-EN (SKU 34201). Both have a square 480 × 480 CO5300 AMOLED
panel and CST9220 capacitive touch, 16 MB flash and no PSRAM. It is experimental:
physical display, touch, brightness and battery acceptance have not been performed.

## Installation and layout

Choose this model in New screen on the dev channel and follow [Easy setup](EASY_SETUP.md).
The remote entry is `packages/wavesharec6216.yaml`, the checkout entry is
`checkout/wavesharec6216.yaml`, and the hardware is `packages/boards/wavesharec6216.yaml`.
ESPHome 2026.9.0 or newer is required. The default compact grid is one column and two
rows, with up to eight pages and 64 tiles subject to the screen's memory budget.
The diagonal gives 314.270 dpi and about 38.8 mm per side. The LVGL draw buffer is
an eighth of an RGB565 frame, 57,600 bytes. These are calculated sizes, not heap measurements.

The panel takes brightness through its DCS command rather than a PWM pin. The
shared brightness, standby, night mode and alert controls use the same output
contract as other screens. Zero brightness needs to be checked on glass. No PSRAM
means camera pictures and live video are not offered. Audio, the IMU, RTC, SD card
and the optional external speaker are not enabled in this initial profile.

## Hardware references

- [Product](https://www.waveshare.com/esp32-c6-touch-amoled-2.16.htm).
- [Manufacturer documentation and schematic](https://docs.waveshare.com/ESP32-C6-Touch-AMOLED-2.16/Resources-And-Documents).
- [Manufacturer examples](https://github.com/waveshareteam/ESP32-C6-Touch-AMOLED-2.16),
  `02_Example/Arduino-v3.3.3/09_LVGL_V9_Test`: `user_config.h`, `lcd_touch.cpp`
  `bsp_lvgl_port.cpp` and `src/port_bsp/axp2101_bsp.cpp`.

QSPI uses clock GPIO0, data GPIO1 through GPIO4 and chip select GPIO15. The display
has no reset or PWM backlight pin. I2C uses SDA GPIO8 and SCL GPIO7, shared by the
AXP2101 at 0x34 and touch at 0x5A. Touch reset is GPIO11; GPIO5 interrupt is unused
because contacts are polled every 20 ms. ESPHome’s native CST9220 driver reads and acknowledges reports at 0xD000, validates
the controller and reports releases through the shared touch path. The board applies
the vendor’s axis swap and X mirror once before LVGL rotation.

ALDO3 is set to 3.3 V, then cycled on, off and on for 100 ms each before panel
setup, matching the manufacturer display reset. Other rails retain their settings.
The vendor tuning registers and two-pixel draw alignment supplement ESPHome’s
CO5300 initialization, while ESPHome owns the pixel format and orientation. The charging policy stays at its factory setting. Battery
percentage comes from AXP2101 register 0xA4 only while a battery is present; the
charging flag comes from register 0x01. The EN variant reports an unknown battery
level when no battery is connected.

## Hardware acceptance

Check cold boot, correct colours, the full tile layout and free heap after boot.
Tap all four corners, drag a slider and swipe at the edges, then repeat after
rotation. Check brightness across its range, zero, standby, wake by touch and
night mode. On the battery model, check percentage and charging against USB
connection and disconnection. A successful configuration check or build does not
prove these physical behaviours.
