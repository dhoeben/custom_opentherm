#pragma once

#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/number/number.h"
#include "esphome/components/climate/climate.h"

namespace opentherm {
namespace Sensors {

// ---------------------------
// Boiler / CH / DHW sensoren
// ---------------------------
extern esphome::sensor::Sensor *boiler_temp;
extern esphome::sensor::Sensor *return_temp;
extern esphome::sensor::Sensor *modulation;
extern esphome::sensor::Sensor *setpoint;
extern esphome::sensor::Sensor *dhw_temp;

// ---------------------------
// Externe / omgevingssens
// ---------------------------
extern esphome::sensor::Sensor *ha_weather_temp;
extern esphome::sensor::Sensor *ha_indoor_temp;
extern esphome::sensor::Sensor *adaptive_indoor_temp;

// ---------------------------
// Flow / diagnostiek
// ---------------------------
extern esphome::sensor::Sensor *dhw_flow_rate;

// ---------------------------
// Diagnostic binary sensors
// ---------------------------
extern esphome::binary_sensor::BinarySensor *ch_active;
extern esphome::binary_sensor::BinarySensor *dhw_active;
extern esphome::binary_sensor::BinarySensor *flame;
extern esphome::binary_sensor::BinarySensor *fault;
extern esphome::binary_sensor::BinarySensor *comms_ok;

// ---------------------------
// Tekstdiagnostiek
// ---------------------------
extern esphome::text_sensor::TextSensor *fault_text;

// ---------------------------
// Boiler / DHW grenzen (numbers)
// ---------------------------
extern esphome::number::Number *max_boiler_temp;
extern esphome::number::Number *max_dhw_temp;

// ---------------------------
// Equitherm tuning (numbers)
// ---------------------------
extern esphome::number::Number *eq_n;
extern esphome::number::Number *eq_k;
extern esphome::number::Number *eq_t;
extern esphome::number::Number *eq_fb_gain;

// ---------------------------
// Climate entity
// ---------------------------
extern esphome::climate::Climate *ch_climate;

// -------------------------------------------------------------------
// Binding helpers
// -------------------------------------------------------------------
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

// Helper macro's voor korte toegang in andere .cpp's
#define OT_SENSOR(name) ::opentherm::Sensors::name

// Publiceer alleen als de pointer niet null is
#define PUBLISH_IF(sensor_ptr, value)      \
  do {                                     \
    if ((sensor_ptr) != nullptr) {         \
      (sensor_ptr)->publish_state(value);  \
    }                                      \
  } while (0)
