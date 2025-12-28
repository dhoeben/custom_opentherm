#include "diagnostics.h"

#include <vector>

#include "esphome/core/log.h"
#include "opentherm.h"

namespace opentherm {

void DiagnosticsModule::update(OpenThermComponent *ot) {
    uint32_t now = esphome::millis();
    if (comms_ok_) {
        bool ok = (now - last_rx_time_) < 60000;
        if (ok != last_comms_state_) {
            last_comms_state_ = ok;
            comms_ok_->publish_state(ok);
        }
    }

    ot->enqueue_request(OT_MSG_STATUS);
    ot->enqueue_request(OT_MSG_FAULT_FLAGS);

    if (flow_rate_) ot->enqueue_request(OT_MSG_DHW_FLOW_RATE);
}

bool DiagnosticsModule::process_message(uint8_t id, uint16_t data, float value) {
    last_rx_time_ = esphome::millis();

    switch (id) {
        case OT_MSG_STATUS: {
            bool is_fault = data & (1 << 0);
            bool ch       = data & (1 << 1);
            bool dhw      = data & (1 << 2);
            bool flame    = data & (1 << 3);

            dhw_active_state_ = dhw;

            if (fault_) fault_->publish_state(is_fault);
            if (ch_active_) ch_active_->publish_state(ch);
            if (dhw_active_) dhw_active_->publish_state(dhw);
            if (flame_) flame_->publish_state(flame);
            return true;
        }
        case OT_MSG_FAULT_FLAGS:
            if (fault_text_) fault_text_->publish_state(decode_fault_flags(data));
            return true;
        case OT_MSG_DHW_FLOW_RATE:
            if (flow_rate_) flow_rate_->publish_state(value);
            return true;
        default:
            return false;
    }
}

std::string DiagnosticsModule::decode_fault_flags(uint16_t data) {
    static const struct {
        uint16_t    mask;
        const char *message;
    } fault_definitions[] = {{1 << 0, "Service Required"},
                             {1 << 1, "Lockout Active"},
                             {1 << 2, "Low Water Pressure"},
                             {1 << 3, "Flame Loss"},
                             {1 << 4, "Sensor Failure"},
                             {1 << 5, "Overheat Protection"},
                             {1 << 6, "Gas Fault"},
                             {1 << 7, "Air Pressure Fault"},
                             {1 << 8, "Fan Fault"},
                             {1 << 9, "Communication Error"},
                             {1 << 10, "Return Temp Sensor Fault"},
                             {1 << 11, "Flow Temp Sensor Fault"},
                             {1 << 12, "Ignition Failure"},
                             {1 << 13, "Flue Blocked"},
                             {1 << 14, "Circulation Fault"},
                             {1 << 15, "Unknown Fault"}};

    std::vector<std::string> faults;

    for (const auto &def : fault_definitions) {
        if (data & def.mask) {
            faults.push_back(def.message);
        }
    }

    if (faults.empty()) return "No faults";

    std::string result = faults[0];
    for (size_t i = 1; i < faults.size(); ++i) result += ", " + faults[i];
    return result;
}

}  // namespace opentherm