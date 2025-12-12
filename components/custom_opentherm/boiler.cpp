#include "boiler.h"

#include "esphome/core/log.h"
#include "opentherm.h"

namespace opentherm {

static const char *const TAG = "ot_boiler";

void BoilerModule::setup() {
    if (limit_number_) {
        if (!limit_number_->has_state()) {
            limit_number_->publish_state(60.0f);
        }
    }
}

void BoilerModule::update(OpenThermComponent *ot) {
    ot->enqueue_request(OT_MSG_CH_WATER_TEMP);
    ot->enqueue_request(OT_MSG_RETURN_WATER_TEMP);

    ot->enqueue_request(0x18);
    ot->enqueue_request(0x19);

    ot->enqueue_request(OT_MSG_REL_MOD_LEVEL);
    ot->enqueue_request(OT_MSG_SOLAR_STORAGE);
}

bool BoilerModule::process_message(uint8_t id, float value) {
    switch (id) {
        case 0x18:
            if (temp_sensor_) temp_sensor_->publish_state(value);
            return true;
        case 0x19:
            if (return_sensor_) return_sensor_->publish_state(value);
            return true;
        case OT_MSG_REL_MOD_LEVEL:
            if (modulation_sensor_) modulation_sensor_->publish_state(value);

            if (setpoint_sensor_) setpoint_sensor_->publish_state(value);
            return true;
        case OT_MSG_SOLAR_STORAGE:

            if (modulation_sensor_) modulation_sensor_->publish_state(value);
            return true;
        default:
            return false;
    }
}

float BoilerModule::get_limit_temp() const {
    if (limit_number_ && limit_number_->has_state()) {
        return limit_number_->state;
    }
    return 60.0f;
}

}  // namespace opentherm