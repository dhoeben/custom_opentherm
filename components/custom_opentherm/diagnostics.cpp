#include "diagnostics.h"
#include "opentherm.h"
#include "sensors.h"
#include "esphome/core/log.h"

using namespace esphome;

namespace opentherm {
namespace Diagnostics {

static const char *const TAG = "ot_diag";

static bool last_comms_ok = false;
static uint32_t last_rx_time = 0;

static std::string decode_fault_flags(uint16_t data) {
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
  
  std::string result;
  for (size_t i = 0; i < faults.size(); ++i) {
    result += faults[i];
    if (i != faults.size() - 1) result += ", ";
  }
  return result;
}

void process_status(uint16_t data) {
    last_rx_time = millis(); 
    
    const bool is_fault  = data & (1 << 0);
    const bool ch        = data & (1 << 1);
    const bool dhw       = data & (1 << 2);
    const bool flame     = data & (1 << 3);

    if (Sensors::fault)      Sensors::fault->publish_state(is_fault);
    if (Sensors::ch_active)  Sensors::ch_active->publish_state(ch);
    if (Sensors::dhw_active) Sensors::dhw_active->publish_state(dhw);
    if (Sensors::flame)      Sensors::flame->publish_state(flame);
}

void process_fault_flags(uint16_t data) {
    if (Sensors::fault_text != nullptr) {
        Sensors::fault_text->publish_state(decode_fault_flags(data));
    }
}

void process_flow_rate(float value) {
    if (Sensors::dhw_flow_rate != nullptr) {
        Sensors::dhw_flow_rate->publish_state(value);
    }
}

void update(OpenThermComponent *ot) {
  if (!ot) return;
  const uint32_t now = millis();

  if (Sensors::comms_ok) {
    const bool timed_out = (now - last_rx_time) > 60000; 
    
    if (!timed_out != last_comms_ok) {
      last_comms_ok = !timed_out;
      Sensors::comms_ok->publish_state(last_comms_ok);
    }
  }

  ot->enqueue_request(0x00); // Status
  ot->enqueue_request(0x01); // Fault flags
  
  if (Sensors::dhw_flow_rate != nullptr) {
     ot->enqueue_request(0x3E); // Flow rate
  }
}

}  // namespace Diagnostics
}  // namespace opentherm