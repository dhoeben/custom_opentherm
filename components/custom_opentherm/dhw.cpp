#include "dhw.h"

#include "definitions.h"

namespace custom_opentherm {

void DhwController::reset() {
    temperature_c_      = 0.0f;
    flow_rate_l_min_    = 0.0f;

    has_temp_           = false;
    has_flow_rate_      = false;

    preheat_active_     = false;
}

bool DhwController::process_message(uint8_t did, uint16_t raw, float value_c) {
    switch (did) {
        case OT_MSG_DHW_TEMP:
            temperature_c_ = value_c;
            has_temp_      = true;
            return true;

        case OT_MSG_DHW_FLOW_RATE:
            flow_rate_l_min_ = value_c;
            has_flow_rate_   = true;
            return true;

        case OT_MSG_STATUS: {
            const uint8_t lb = static_cast<uint8_t>(raw & 0xFF);

            const bool dhw_active = ((lb >> 2) & 0x01u) != 0;
            const bool flame_on   = ((lb >> 3) & 0x01u) != 0;

            preheat_active_ = dhw_active && !tap_flow_active() && flame_on;
            return true;
        }

        default:
            return false;
    }
}

float DhwController::temperature_c() const {
    return temperature_c_;
}

float DhwController::flow_rate_l_min() const {
    return flow_rate_l_min_;
}

bool DhwController::tap_flow_active() const {
    return has_flow_rate_ && flow_rate_l_min_ >= tap_flow_threshold_;
}

bool DhwController::preheat_active() const {
    return preheat_active_;
}

bool DhwController::has_temp() const {
    return has_temp_;
}

bool DhwController::has_flow_rate() const {
    return has_flow_rate_;
}

void DhwController::set_tap_flow_threshold_l_min(float threshold) {
    tap_flow_threshold_ = threshold;
}

}  // namespace custom_opentherm
