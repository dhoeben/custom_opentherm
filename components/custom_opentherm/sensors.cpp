#include "sensors.h"

namespace opentherm {
namespace Sensors {

// -------------------------------------
// Extern pointers initialiseren op null
// -------------------------------------

// Boiler / CH / DHW
esphome::sensor::Sensor *boiler_temp   = nullptr;
esphome::sensor::Sensor *return_temp   = nullptr;
esphome::sensor::Sensor *modulation    = nullptr;
esphome::sensor::Sensor *setpoint      = nullptr;
esphome::sensor::Sensor *dhw_temp      = nullptr;

// Extern / omgeving
esphome::sensor::Sensor *ha_weather_temp      = nullptr;
esphome::sensor::Sensor *ha_indoor_temp       = nullptr;
esphome::sensor::Sensor *adaptive_indoor_temp = nullptr;

// Flow / diagnostiek
esphome::sensor::Sensor *dhw_flow_rate = nullptr;

// Diagnostic binary sensors
esphome::binary_sensor::BinarySensor *ch_active  = nullptr;
esphome::binary_sensor::BinarySensor *dhw_active = nullptr;
esphome::binary_sensor::BinarySensor *flame      = nullptr;
esphome::binary_sensor::BinarySensor *fault      = nullptr;
esphome::binary_sensor::BinarySensor *comms_ok   = nullptr;

// Tekstdiagnostiek
esphome::text_sensor::TextSensor *fault_text = nullptr;

// Boiler / DHW grenzen
esphome::number::Number *max_boiler_temp = nullptr;
esphome::number::Number *max_dhw_temp    = nullptr;

// Equitherm tuning
esphome::number::Number *eq_n       = nullptr;
esphome::number::Number *eq_k       = nullptr;
esphome::number::Number *eq_t       = nullptr;
esphome::number::Number *eq_fb_gain = nullptr;

// Climate
esphome::climate::Climate *ch_climate = nullptr;

// -------------------------------------------------------------------
// Binding helpers
// -------------------------------------------------------------------
void bind_boiler_sensors(esphome::sensor::Sensor *temp,
                         esphome::sensor::Sensor *ret,
                         esphome::sensor::Sensor *mod,
                         esphome::sensor::Sensor *setp) {
  boiler_temp = temp;
  return_temp = ret;
  modulation  = mod;
  setpoint    = setp;
}

void bind_diagnostics(esphome::binary_sensor::BinarySensor *ch,
                      esphome::binary_sensor::BinarySensor *dhw,
                      esphome::binary_sensor::BinarySensor *flame_s,
                      esphome::binary_sensor::BinarySensor *fault_s,
                      esphome::binary_sensor::BinarySensor *comms,
                      esphome::sensor::Sensor *flow_rate,
                      esphome::text_sensor::TextSensor *fault_text_s) {
  ch_active     = ch;
  dhw_active    = dhw;
  flame         = flame_s;
  fault         = fault_s;
  comms_ok      = comms;
  dhw_flow_rate = flow_rate;
  fault_text    = fault_text_s;
}

}  // namespace Sensors
}  // namespace opentherm
