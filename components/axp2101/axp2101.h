#pragma once

// ALDO rails and battery status only, from the AXP2101 register map. Voltage and
// enabled rails come from board wiring. Charging policy remains at factory defaults.
// docs/WAVESHAREC6216.md documents the first board using it and hardware acceptance.
#include "esphome/core/component.h"
#include <cmath>
#include "esphome/components/i2c/i2c.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"

namespace esphome::axp2101 {
class AXP2101 : public PollingComponent, public i2c::I2CDevice {
 public:
  void set_aldo_voltage(uint16_t mv) { voltage_ = (mv - 500) / 100; }
  void set_aldo_mask(uint8_t mask) { mask_ = mask; }
  void set_reset_mask(uint8_t mask) { reset_mask_ = mask; }
  bool can_proceed() override { return ready_ || is_failed(); }
  void set_battery_level(sensor::Sensor *s) { battery_ = s; }
  void set_battery_charging(binary_sensor::BinarySensor *s) { charging_ = s; }
  float get_setup_priority() const override { return setup_priority::IO; }
  void setup() override {
    uint8_t value;
    for (uint8_t i = 0; i < 4; ++i) {
      if (!(mask_ & (1 << i))) continue;
      if (!read_byte(0x92 + i, &value) || !write_byte(0x92 + i, (value & 0xE0) | voltage_)) {
        mark_failed();
        return;
      }
    }
    if (!read_byte(0x90, &value) || !write_byte(0x90, value | mask_)) {
      mark_failed();
      return;
    }
    if (!reset_mask_) { ready_ = true; return; }
    // Supply reset: on, off, on, each held for 100 ms. Lower-priority
    // components wait until the panel's power is settled.
    set_timeout(100, [this] {
      if (!set_reset_rail_(false)) return;
      set_timeout(100, [this] {
        if (!set_reset_rail_(true)) return;
        set_timeout(100, [this] { ready_ = true; });
      });
    });
  }
  void update() override {
    uint8_t status, percent;
    if (!read_byte(0x00, &status)) {
      status_set_warning();
      if (battery_) battery_->publish_state(NAN);
      if (charging_) charging_->invalidate_state();
      return;
    }
    if (battery_) {
      const bool valid = (status & 8) && read_byte(0xA4, &percent) && percent <= 100;
      battery_->publish_state(valid ? percent : NAN);
    }
    if (charging_) {
      if (read_byte(0x01, &status)) charging_->publish_state((status >> 5) == 1);
      else charging_->invalidate_state();
    }
    status_clear_warning();
  }
 protected:
  bool set_reset_rail_(bool enabled) {
    uint8_t value;
    if (!read_byte(0x90, &value) ||
        !write_byte(0x90, enabled ? value | reset_mask_ : value & ~reset_mask_)) {
      mark_failed();
      return false;
    }
    return true;
  }
  uint8_t voltage_{}, mask_{}, reset_mask_{};
  bool ready_{};
  sensor::Sensor *battery_{};
  binary_sensor::BinarySensor *charging_{};
};
}  // namespace esphome::axp2101
