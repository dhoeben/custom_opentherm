#include "sensors.h"

namespace opentherm {
namespace Sensors {

// -------------------------------------
// Allocate extern pointers (initially null)
// -------------------------------------
esphome::sensor::Sensor *boiler_temp   = nullptr;
esphome::sensor::Sensor *return_temp   = nullptr;
esphome::sensor::Sensor *modulation    = nullptr;
esphome::sensor::Sensor *setpoint      = nullptr;

esphome::binary_sensor::BinarySensor *ch_active = nullptr;
esphome::binary_sensor::BinarySensor *dhw_active = nullptr;
esphome::binary_sensor::BinarySensor *flame = nullptr;
esphome::binary_sensor::BinarySensor *fault = nullptr;
esphome::binary_sensor::BinarySensor *comms_ok = nullptr;

esphome::sensor::Sensor *dhw_flow_rate = nullptr;

esphome::text_sensor::TextSensor *fault_text = nullptr;

// -------------------------------------
// Binding API
// -------------------------------------

void bind_boiler_sensors(esphome::sensor::Sensor *temp,
                         esphome::sensor::Sensor *ret,
                         esphome::sensor::Sensor *mod,
                         esphome::sensor::Sensor *setp) {
  boiler_temp = temp;
  return_temp = ret;
  modulation = mod;
  setpoint = setp;
}

void bind_diagnostics(esphome::binary_sensor::BinarySensor *ch,
                      esphome::binary_sensor::BinarySensor *dhw,
                      esphome::binary_sensor::BinarySensor *flame_s,
                      esphome::binary_sensor::BinarySensor *fault_s,
                      esphome::binary_sensor::BinarySensor *comms,
                      esphome::sensor::Sensor *flow_rate,
                      esphome::text_sensor::TextSensor *fault_text_s) {
  ch_active = ch;
  dhw_active = dhw;
  flame = flame_s;
  fault = fault_s;
  comms_ok = comms;
  dhw_flow_rate = flow_rate;
  fault_text = fault_text_s;
}

}  // namespace Sensors
}  // namespace opentherm
