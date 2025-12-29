#include "dhw.h"
#include "definitions.h"

namespace custom_opentherm {

void DhwController::reset() {
    temperature_c_   = 0.0f;
    flow_rate_l_min_ = 0.0f;
    tap_flow_active_ = false;
    preheat_active_  = false;

    has_temperature_ = false;
    has_flow_rate_   = false;
}

bool DhwController::process_message(uint8_t did, uint16_t raw, float value) {
    switch (did) {
        case opentherm::OT_MSG_DHW_TEMP:
            temperature_c_   = value;
            has_temperature_ = true;
            return true;

        case opentherm::OT_MSG_DHW_FLOW_RATE:
            flow_rate_l_min_ = value;
            has_flow_rate_   = true;
            tap_flow_active_ = value > 0.1f;
            return true;

        case opentherm::OT_MSG_STATUS: {
            const bool dhw_active =
                (raw & (1u << 1)) != 0;

            preheat_active_ = dhw_active && !tap_flow_active_;
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
    return tap_flow_active_;
}

bool DhwController::preheat_active() const {
    return preheat_active_;
}

bool DhwController::has_recent_data() const {
    return has_temperature_ || has_flow_rate_;
}

}  // namespace custom_opentherm
