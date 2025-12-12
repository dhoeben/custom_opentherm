#include "emergency.h"
#include "esphome/core/log.h"

namespace opentherm {

void EmergencyModule::enable(bool state) {
    active_ = state;
    ESP_LOGW("ot_emergency", "Emergency mode: %s", state ? "ON" : "OFF");
    
    // Update de fysieke switch in HA als die er is
    if (emergency_switch_ && emergency_switch_->state != state) {
        emergency_switch_->publish_state(state);
    }
}

} // namespace opentherm