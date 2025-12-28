#pragma once

#include <cstdint>

namespace custom_opentherm {

class BoilerController {
 public:
    BoilerController() = default;

    void reset();

    bool process_message(uint8_t did, uint16_t raw, float value);

    float flow_temp_c() const;
    float return_temp_c() const;
    float modulation_percent() const;
    float pressure_bar() const;

    bool has_recent_data() const;

 private:
    float flow_temp_c_        = 0.0f;
    float return_temp_c_      = 0.0f;
    float modulation_percent_ = 0.0f;
    float pressure_bar_       = 0.0f;

    bool has_flow_temp_        = false;
    bool has_return_temp_      = false;
    bool has_modulation_       = false;
    bool has_pressure_         = false;
};

}  // namespace custom_opentherm
