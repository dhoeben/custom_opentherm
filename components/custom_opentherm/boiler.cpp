#include "boiler.h"
#include "definitions.h"

namespace custom_opentherm {

void BoilerController::reset() {
    flow_temp_c_        = 0.0f;
    return_temp_c_      = 0.0f;
    modulation_percent_ = 0.0f;
    pressure_bar_       = 0.0f;

    has_flow_temp_   = false;
    has_return_temp_ = false;
    has_modulation_  = false;
    has_pressure_    = false;
}

bool BoilerController::process_message(uint8_t did, uint16_t, float value) {
    switch (did) {
        case opentherm::OT_MSG_CH_WATER_TEMP:
            flow_temp_c_   = value;
            has_flow_temp_ = true;
            return true;

        case opentherm::OT_MSG_RETURN_WATER_TEMP:
            return_temp_c_   = value;
            has_return_temp_ = true;
            return true;

        case opentherm::OT_MSG_REL_MOD_LEVEL:
            modulation_percent_ = value;
            has_modulation_     = true;
            return true;

        case opentherm::OT_MSG_CH_WATER_PRESSURE:
            pressure_bar_ = value;
            has_pressure_ = true;
            return true;

        default:
            return false;
    }
}

float BoilerController::flow_temp_c() const {
    return flow_temp_c_;
}

float BoilerController::return_temp_c() const {
    return return_temp_c_;
}

float BoilerController::modulation_percent() const {
    return modulation_percent_;
}

float BoilerController::pressure_bar() const {
    return pressure_bar_;
}

bool BoilerController::has_recent_data() const {
    return has_flow_temp_ ||
           has_return_temp_ ||
           has_modulation_ ||
           has_pressure_;
}

}  // namespace custom_opentherm
