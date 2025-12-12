#include "diagnostics.h"
#include "opentherm.h"
#include "esphome/core/log.h"
#include <vector>

namespace opentherm {

void DiagnosticsModule::update(OpenThermComponent *ot) {
    // Comms check
    uint32_t now = esphome::millis();
    if (comms_ok_) {
        bool ok = (now - last_rx_time_) < 60000;
        if (ok != last_comms_state_) {
            last_comms_state_ = ok;
            comms_ok_->publish_state(ok);
        }
    }

    ot->enqueue_request(OT_MSG_STATUS);      // 0x00
    ot->enqueue_request(OT_MSG_FAULT_FLAGS); // 0x01
    
    if (flow_rate_) ot->enqueue_request(OT_MSG_DHW_FLOW_RATE); // 0x3E
}

bool DiagnosticsModule::process_message(uint8_t id, uint16_t data, float value) {
    last_rx_time_ = esphome::millis(); // Geldig bericht ontvangen = Comms OK

    switch (id) {
        case OT_MSG_STATUS: { // 0x00
            bool is_fault = data & (1 << 0);
            bool ch       = data & (1 << 1);
            bool dhw      = data & (1 << 2);
            bool flame    = data & (1 << 3);

            if (fault_) fault_->publish_state(is_fault);
            if (ch_active_) ch_active_->publish_state(ch);
            if (dhw_active_) dhw_active_->publish_state(dhw);
            if (flame_) flame_->publish_state(flame);
            return true;
        }
        case OT_MSG_FAULT_FLAGS: // 0x01
            if (fault_text_) fault_text_->publish_state(decode_fault_flags(data));
            return true;
        case OT_MSG_DHW_FLOW_RATE: // 0x3E
            if (flow_rate_) flow_rate_->publish_state(value);
            return true;
        default:
            return false;
    }
}

std::string DiagnosticsModule::decode_fault_flags(uint16_t data) {
    std::vector<std::string> faults;
    if (data & (1 << 0)) faults.push_back("Service Required");
    if (data & (1 << 1)) faults.push_back("Lockout Active");
    if (data & (1 << 2)) faults.push_back("Low Water Pressure");
    if (data & (1 << 3)) faults.push_back("Flame Loss");
    if (data & (1 << 4)) faults.push_back("Sensor Failure");
    if (data & (1 << 5)) faults.push_back("Overheat Protection");
    if (data & (1 << 6)) faults.push_back("Gas Fault");
    if (data & (1 << 7)) faults.push_back("Air Pressure Fault");
    if (data & (1 << 8)) faults.push_back("Fan Fault");
    if (data & (1 << 9)) faults.push_back("Communication Error");
    if (data & (1 << 10)) faults.push_back("Return Temp Sensor Fault");
    if (data & (1 << 11)) faults.push_back("Flow Temp Sensor Fault");
    if (data & (1 << 12)) faults.push_back("Ignition Failure");
    if (data & (1 << 13)) faults.push_back("Flue Blocked");
    if (data & (1 << 14)) faults.push_back("Circulation Fault");
    if (data & (1 << 15)) faults.push_back("Unknown Fault");

    if (faults.empty()) return "No faults";
    
    std::string result = faults[0];
    for(size_t i=1; i<faults.size(); i++) result += ", " + faults[i];
    return result;
}

} // namespace opentherm