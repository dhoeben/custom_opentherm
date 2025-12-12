#pragma once
#include <queue>
#include "esphome.h"
#include "definitions.h"

// Include de nieuwe module classes
#include "boiler.h"
#include "dhw.h"
#include "diagnostics.h"
#include "equitherm.h"
#include "emergency.h"

namespace opentherm {

enum class CompensationMode { EQUITHERM, BOILER };

class OpenThermComponent : public esphome::Component {
 public:
  OpenThermComponent();
  
  void setup() override;
  void loop() override;

  // Queue helper (gebruikt door modules)
  void enqueue_request(uint8_t did) { request_queue_.push(did); }
  void send_request(uint8_t type, uint8_t id, uint16_t data); // Direct zenden

  // --- Setters (aangeroepen vanuit codegen/__init__.py) ---
  // Deze sturen we direct door naar de sub-modules!
  
  // Boiler Sensors
  void set_boiler_temp_sensor(esphome::sensor::Sensor *s) { boiler_.set_temp_sensor(s); }
  void set_return_temp_sensor(esphome::sensor::Sensor *s) { boiler_.set_return_sensor(s); }
  void set_modulation_sensor(esphome::sensor::Sensor *s)  { boiler_.set_modulation_sensor(s); }
  void set_setpoint_sensor(esphome::sensor::Sensor *s)    { boiler_.set_setpoint_sensor(s); }
  void set_boiler_limit_number(esphome::number::Number *n) { boiler_.set_limit_number(n); }

  // DHW Sensors
  void set_dhw_temp_sensor(esphome::sensor::Sensor *s)    { dhw_.set_temp_sensor(s); }
  void set_dhw_limit_number(esphome::number::Number *n)   { dhw_.set_limit_number(n); }
  
  // Diagnostics Sensors
  void set_fault_binary_sensor(esphome::binary_sensor::BinarySensor *s) { diagnostics_.set_fault_sensor(s); }
  void set_ch_active_binary_sensor(esphome::binary_sensor::BinarySensor *s) { diagnostics_.set_ch_active_sensor(s); }
  void set_dhw_active_binary_sensor(esphome::binary_sensor::BinarySensor *s) { diagnostics_.set_dhw_active_sensor(s); }
  void set_flame_binary_sensor(esphome::binary_sensor::BinarySensor *s) { diagnostics_.set_flame_sensor(s); }
  void set_fault_text_sensor(esphome::text_sensor::TextSensor *s) { diagnostics_.set_fault_text_sensor(s); }
  
  // Equitherm & Weather
  void set_ha_weather_sensor(esphome::sensor::Sensor *s) { equitherm_.set_outdoor_sensor(s); }
  void set_ha_indoor_sensor(esphome::sensor::Sensor *s)  { equitherm_.set_indoor_sensor(s); }
  void set_climate_entity(esphome::climate::Climate *c)  { equitherm_.set_climate(c); }
  void set_eq_params(esphome::number::Number *n, esphome::number::Number *k, 
                     esphome::number::Number *t, esphome::number::Number *fb) {
      equitherm_.set_params(n, k, t, fb);
  }

  // Pin setters
  void set_pins(esphome::InternalGPIOPin *in, esphome::InternalGPIOPin *out) { in_pin_ = in; out_pin_ = out; }

 private:
  // Sub-modules (Classes)
  BoilerModule boiler_;
  DHWModule dhw_;
  DiagnosticsModule diagnostics_;
  EquithermModule equitherm_;
  EmergencyModule emergency_;

  // IO & State
  esphome::InternalGPIOPin *in_pin_{nullptr};
  esphome::InternalGPIOPin *out_pin_{nullptr};
  std::queue<uint8_t> request_queue_;
  uint32_t last_poll_ms_{0};
  uint32_t last_req_ms_{0};

  // Helpers
  uint32_t read_did(uint8_t did);
  bool send_frame(uint32_t frame);
  bool recv_frame(uint32_t &resp);
  void process_response(uint8_t did, uint32_t response);
  uint32_t build_request(uint8_t mt, uint8_t did, uint16_t data);
  // ... bit-banging helpers (tx_bit etc) hier ook houden ...
};

} // namespace opentherm