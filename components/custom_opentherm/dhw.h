#pragma once

#include <cstdint>

namespace custom_opentherm {

class DhwController {
 public:
    DhwController() = default;

    void reset();

    bool process_message(uint8_t did, uint16_t raw, float value_c);

    float temperature_c() const;
    float flow_rate_l_min() const;

    bool tap_flow_active() const;
    bool preheat_active() const;

    bool has_temp() const;
    bool has_flow_rate() const;

    void set_tap_flow_threshold_l_min(float threshold);

 private:
    float temperature_c_      = 0.0f;
    float flow_rate_l_min_    = 0.0f;

    bool has_temp_            = false;
    bool has_flow_rate_       = false;

    bool preheat_active_      = false;

    float tap_flow_threshold_ = 0.2f;
};

}  // namespace custom_opentherm
