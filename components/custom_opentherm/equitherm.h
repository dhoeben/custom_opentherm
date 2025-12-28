#pragma once

#include <cstdint>

namespace custom_opentherm {

class EquithermController {
 public:
    EquithermController() = default;

    void set_parameters(float fb_gain, float k, float n, float t);
    void set_limits(float min_c, float max_c);

    void set_outside_temp(float outside_c);
    void set_room_temp(float room_c);
    void set_room_setpoint(float setpoint_c);

    float compute_target(float boiler_limit_c) const;

 private:
    float fb_gain_ = 0.5f;
    float k_       = 1.2f;
    float n_       = 0.8f;
    float t_       = 20.0f;

    float min_c_ = 0.0f;
    float max_c_ = 80.0f;

    bool outside_valid_ = false;
    bool room_valid_    = false;
    bool setpoint_valid_ = false;

    float outside_c_  = 0.0f;
    float room_c_     = 0.0f;
    float setpoint_c_ = 0.0f;
};

} //namespace custom_opentherm
