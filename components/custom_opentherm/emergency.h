#pragma once
#include "esphome/components/switch/switch.h"

namespace opentherm {

class EmergencyModule {
 public:
  bool is_active() const { return active_; }
  float get_target() const { return manual_target_; }

  void enable(bool state);
  void set_target(float t) { manual_target_ = t; }

  // Setters voor de switches
  void set_switches(esphome::switch_::Switch *em, esphome::switch_::Switch *force_heat, esphome::switch_::Switch *force_dhw) {
      emergency_switch_ = em;
      force_heat_switch_ = force_heat;
      force_dhw_switch_ = force_dhw;
  }

 private:
  bool active_{false};
  float manual_target_{60.0f};

  esphome::switch_::Switch *emergency_switch_{nullptr};
  esphome::switch_::Switch *force_heat_switch_{nullptr};
  esphome::switch_::Switch *force_dhw_switch_{nullptr};
};

} // namespace opentherm