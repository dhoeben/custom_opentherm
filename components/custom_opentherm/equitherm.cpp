#include "equitherm.h"

#include <algorithm>
#include <cmath>

namespace custom_opentherm {

void EquithermController::set_parameters(float fb_gain, float k, float n, float t) {
    fb_gain_ = fb_gain;
    k_       = k;
    n_       = n;
    t_       = t;
}

void EquithermController::set_limits(float min_c, float max_c) {
    min_c_ = min_c;
    max_c_ = max_c;
}

void EquithermController::set_outside_temp(float outside_c) {
    outside_c_     = outside_c;
    outside_valid_ = true;
}

void EquithermController::set_room_temp(float room_c) {
    room_c_     = room_c;
    room_valid_ = true;
}

void EquithermController::set_room_setpoint(float setpoint_c) {
    setpoint_c_      = setpoint_c;
    setpoint_valid_  = true;
}

float EquithermController::compute_target(float boiler_limit_c) const {
    if (!outside_valid_) {
        return min_c_;
    }

    float target = t_ + k_ * std::pow((t_ - outside_c_), n_);

    if (room_valid_ && setpoint_valid_) {
        const float error = setpoint_c_ - room_c_;
        target += fb_gain_ * error;
    }

    target = std::clamp(target, min_c_, std::min(max_c_, boiler_limit_c));
    return target;
}

}
