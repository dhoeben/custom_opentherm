#pragma once
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/number/number.h"
#include "esphome/components/climate/climate.h"

namespace opentherm {

class EquithermModule {
 public:
  float calculate_target(float max_boiler_temp);

  // Setters
  void set_outdoor_sensor(esphome::sensor::Sensor *s) { outdoor_ = s; }
  void set_indoor_sensor(esphome::sensor::Sensor *s) { indoor_ = s; }
  void set_climate(esphome::climate::Climate *c) { climate_ = c; }
  
  void set_params(esphome::number::Number *n, esphome::number::Number *k, 
                  esphome::number::Number *t, esphome::number::Number *fb) {
      n_ = n; k_ = k; t_ = t; fb_ = fb;
  }

 private:
  esphome::sensor::Sensor *outdoor_{nullptr};
  esphome::sensor::Sensor *indoor_{nullptr};
  esphome::climate::Climate *climate_{nullptr};
  
  esphome::number::Number *n_{nullptr};
  esphome::number::Number *k_{nullptr};
  esphome::number::Number *t_{nullptr};
  esphome::number::Number *fb_{nullptr};
};

} // namespace opentherm