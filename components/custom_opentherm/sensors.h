#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace opentherm {
namespace Sensors {

// ---------------------------
// Boiler sensors (F8.8 values)
// ---------------------------
extern esphome::sensor::Sensor *boiler_temp;
extern esphome::sensor::Sensor *return_temp;
extern esphome::sensor::Sensor *modulation;
extern esphome::sensor::Sensor *setpoint;

// ---------------------------
// Diagnostics binary sensors
// ---------------------------
extern esphome::binary_sensor::BinarySensor *ch_active;
extern esphome::binary_sensor::BinarySensor *dhw_active;
extern esphome::binary_sensor::BinarySensor *flame;
extern esphome::binary_sensor::BinarySensor *fault;
extern esphome::binary_sensor::BinarySensor *comms_ok;

// ---------------------------
// DHW related sensors
// ---------------------------
extern esphome::sensor::Sensor *dhw_flow_rate;

// ---------------------------
// Diagnostics text sensors
// ---------------------------
extern esphome::text_sensor::TextSensor *fault_text;

// ---------------------------
// Binding helpers
// ---------------------------
void bind_boiler_sensors(esphome::sensor::Sensor *temp,
                         esphome::sensor::Sensor *ret,
                         esphome::sensor::Sensor *mod,
                         esphome::sensor::Sensor *setp);

void bind_diagnostics(esphome::binary_sensor::BinarySensor *ch,
                      esphome::binary_sensor::BinarySensor *dhw,
                      esphome::binary_sensor::BinarySensor *flame_s,
                      esphome::binary_sensor::BinarySensor *fault_s,
                      esphome::binary_sensor::BinarySensor *comms,
                      esphome::sensor::Sensor *flow_rate,
                      esphome::text_sensor::TextSensor *fault_text_s);

}  // namespace Sensors
}  // namespace opentherm
