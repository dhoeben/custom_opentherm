#pragma once
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/number/number.h"
#include "esphome/components/climate/climate.h"
#include "opentherm_defs.h"

namespace opentherm {

class OpenThermComponent;

enum class DHWMode { OFF, ECO, HEAT };

class DHWModule {
 public:
  void setup();
  void update(OpenThermComponent *ot);
  bool process_message(uint8_t id, uint16_t data, float value);

  // Setters
  void set_temp_sensor(esphome::sensor::Sensor *s) { temp_sensor_ = s; }
  void set_setpoint_sensor(esphome::sensor::Sensor *s) { setpoint_sensor_ = s; }
  void set_limit_number(esphome::number::Number *n) { limit_number_ = n; }
  void set_climate(esphome::climate::Climate *c) { climate_ = c; }

  // Control
  void set_forced(bool forced) { forced_ = forced; }
  void set_mode(DHWMode mode) { mode_ = mode; }
  
  float get_limit_temp() const;
  bool is_active() const { return comfort_mode_enabled_; } // Of een andere status flag

 private:
  esphome::sensor::Sensor *temp_sensor_{nullptr};
  esphome::sensor::Sensor *setpoint_sensor_{nullptr};
  esphome::number::Number *limit_number_{nullptr};
  esphome::climate::Climate *climate_{nullptr};

  bool forced_{false};
  DHWMode mode_{DHWMode::HEAT};
  bool comfort_mode_enabled_{true};

  void send_comfort_setting(OpenThermComponent *ot, bool enable);
};

} // namespace opentherm