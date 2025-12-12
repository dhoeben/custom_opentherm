#include "dhw.h"

#include <cmath>

#include "esphome/core/log.h"
#include "opentherm.h"

namespace opentherm {

static const char *const TAG = "ot_dhw";

void DHWModule::setup() {
    if (limit_number_ && !limit_number_->has_state()) limit_number_->publish_state(60.0f);
}

void DHWModule::update(OpenThermComponent *ot) {
    if (forced_) {
        uint16_t raw = static_cast<uint16_t>(60.0f * 256.0f);
        ot->send_request(OT_MSG_WRITE_DATA, OT_MSG_DHW_SETPOINT, raw);
        return;
    }

    ot->enqueue_request(OT_MSG_DHW_COMFORT);

    float target = 60.0f;
    if (mode_ == DHWMode::OFF)
        target = 0.0f;
    else if (climate_ && !std::isnan(climate_->target_temperature))
        target = climate_->target_temperature;
    else if (limit_number_ && limit_number_->has_state())
        target = limit_number_->state;

    float limit = get_limit_temp();
    if (target > limit) target = limit;

    uint16_t raw_sp = static_cast<uint16_t>(target * 256.0f);
    ot->send_request(OT_MSG_WRITE_DATA, OT_MSG_DHW_SETPOINT, raw_sp);

    if (mode_ == DHWMode::HEAT)
        send_comfort_setting(ot, comfort_mode_enabled_);
    else if (mode_ == DHWMode::OFF)
        send_comfort_setting(ot, false);

    if (climate_) {
        if (temp_sensor_ && temp_sensor_->has_state()) climate_->current_temperature = temp_sensor_->state;

        climate_->target_temperature = target;
        climate_->mode =
            (mode_ == DHWMode::OFF) ? esphome::climate::CLIMATE_MODE_OFF : esphome::climate::CLIMATE_MODE_HEAT;
        climate_->preset =
            comfort_mode_enabled_ ? esphome::climate::CLIMATE_PRESET_COMFORT : esphome::climate::CLIMATE_PRESET_ECO;
        climate_->publish_state();
    }
}

bool DHWModule::process_message(uint8_t id, uint16_t data, float value) {
    switch (id) {
        case OT_MSG_DHW_COMFORT: {
            uint8_t mode_byte     = data >> 8;
            comfort_mode_enabled_ = (mode_byte == 2);
            ESP_LOGD(TAG, "Comfort status: %s", comfort_mode_enabled_ ? "ON" : "OFF");
            return true;
        }
        case OT_MSG_DHW_TEMP:
            if (temp_sensor_) temp_sensor_->publish_state(value);
            return true;
        default:
            return false;
    }
}

void DHWModule::send_comfort_setting(OpenThermComponent *ot, bool enable) {
    uint8_t  mode_val = enable ? 2 : 1;
    uint16_t data     = static_cast<uint16_t>(mode_val) << 8;
    ot->send_request(OT_MSG_WRITE_DATA, OT_MSG_DHW_COMFORT, data);
}

float DHWModule::get_limit_temp() const {
    return (limit_number_ && limit_number_->has_state()) ? limit_number_->state : 60.0f;
}

}  // namespace opentherm