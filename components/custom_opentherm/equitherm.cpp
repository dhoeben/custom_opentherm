#include "equitherm.h"

#include <cmath>

namespace opentherm {

float EquithermModule::calculate_target(float max_boiler_temp) {
    float t_out = (outdoor_ && outdoor_->has_state()) ? outdoor_->state : 10.0f;
    float t_in = (indoor_ && indoor_->has_state()) ? indoor_->state : 20.0f;
    float t_set = (climate_) ? climate_->target_temperature : 21.0f;

    float n_val = (n_ && n_->has_state()) ? n_->state : 1.14f;
    float k_val = (k_ && k_->has_state()) ? k_->state : 4.0f;
    float t_val = (t_ && t_->has_state()) ? t_->state : 20.2f;
    float fb_val = (fb_ && fb_->has_state()) ? fb_->state : 2.0f;

    float base = (n_val * (t_set + k_val - t_out)) + t_val;

    float delta = t_set - t_in;
    float target = base + (delta * fb_val);

    if (target < 10.0f) target = 10.0f;
    if (target > max_boiler_temp) target = max_boiler_temp;

    return target;
}

}  // namespace opentherm