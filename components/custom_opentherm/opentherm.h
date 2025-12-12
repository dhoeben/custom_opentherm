#pragma once
#include <queue>
#include "esphome.h"
#include "definitions.h"
#include "driver/gpio.h"
#include "esp_timer.h"

// Include de module classes
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

  // --- PUBLIC MODULES (zodat je YAML erbij kan) ---
  BoilerModule boiler_;
  DHWModule dhw_;
  DiagnosticsModule diagnostics_;
  EquithermModule equitherm_;
  EmergencyModule emergency_;

  // --- Helpers voor YAML lambda's ---
  
  // Vroegere 'tap_flow' functie
  bool tap_flow() const { 
      // Haal de status op uit diagnostics (ervan uitgaande dat DHW active = tap flow)
      // Als je hier een specifieke flow sensor voor had, moet die logica hier terug.
      return diagnostics_.is_dhw_active(); 
  }

  // Wrapper om handmatig berichten te sturen (vervangt send_frame in YAML)
  void send_request(uint8_t type, uint8_t id, uint16_t data); 
  
  // Queue helper (gebruikt door modules)
  void enqueue_request(uint8_t did) { request_queue_.push(did); }

  // --- Setters (aangeroepen vanuit codegen/__init__.py) ---
  
  // Boiler Sensors
  void set_boiler_temp_sensor(esphome::sensor::Sensor *s) { boiler_.set_temp_sensor(s); }
  void set_return_temp_sensor(esphome::sensor::Sensor *s) { boiler_.set_return_sensor(s); }
  void set_modulation_sensor(esphome::sensor::Sensor *s)  { boiler_.set_modulation_sensor(s); }
  void set_setpoint_sensor(esphome::sensor::Sensor *s)    { boiler_.set_setpoint_sensor(s); }
  void set_boiler_limit_number(esphome::number::Number *n) { boiler_.set_limit_number(n); }

  // DHW Sensors
  void set_dhw_temp_sensor(esphome::sensor::Sensor *s)    { dhw_.set_temp_sensor(s); }
  void set_dhw_setpoint_sensor(esphome::sensor::Sensor *s){ dhw_.set_setpoint_sensor(s); }
  void set_dhw_limit_number(esphome::number::Number *n)   { dhw_.set_limit_number(n); }
  
  // Diagnostics Sensors
  void set_fault_binary_sensor(esphome::binary_sensor::BinarySensor *s) { diagnostics_.set_fault_sensor(s); }
  void set_ch_active_binary_sensor(esphome::binary_sensor::BinarySensor *s) { diagnostics_.set_ch_active_sensor(s); }
  void set_dhw_active_binary_sensor(esphome::binary_sensor::BinarySensor *s) { diagnostics_.set_dhw_active_sensor(s); }
  void set_flame_binary_sensor(esphome::binary_sensor::BinarySensor *s) { diagnostics_.set_flame_sensor(s); }
  void set_fault_text_sensor(esphome::text_sensor::TextSensor *s) { diagnostics_.set_fault_text_sensor(s); }
  void set_dhw_flow_rate_sensor(esphome::sensor::Sensor *s) { diagnostics_.set_flow_rate_sensor(s); }
  void set_comms_ok_binary_sensor(esphome::binary_sensor::BinarySensor *s) { diagnostics_.set_comms_ok_sensor(s); }
  
  // Equitherm & Weather
  void set_ha_weather_sensor(esphome::sensor::Sensor *s) { equitherm_.set_outdoor_sensor(s); }
  void set_ha_indoor_sensor(esphome::sensor::Sensor *s)  { equitherm_.set_indoor_sensor(s); }
  void set_adaptive_indoor_sensor(esphome::sensor::Sensor *s) { /* TODO: evt gebruiken */ }
  void set_climate_entity(esphome::climate::Climate *c)  { equitherm_.set_climate(c); }
  void set_dhw_climate_entity(esphome::climate::Climate *c) { dhw_.set_climate(c); }
  
  void set_eq_params(esphome::number::Number *n, esphome::number::Number *k, 
                     esphome::number::Number *t, esphome::number::Number *fb) {
      equitherm_.set_params(n, k, t, fb);
  }

  // Emergency Switches
  void set_emergency_switches(esphome::switch_::Switch *em, esphome::switch_::Switch *fh, esphome::switch_::Switch *fd) {
      emergency_.set_switches(em, fh, fd);
  }

  // Config setters
  void set_pins(esphome::InternalGPIOPin *in, esphome::InternalGPIOPin *out) { in_pin_ = in; out_pin_ = out; }
  void set_poll_interval(uint32_t ms) { poll_interval_ms_ = ms; }
  void set_rx_timeout(uint32_t ms)    { rx_timeout_ms_ = ms; }
  void set_debug(bool dbg)            { debug_ = dbg; }

  static OpenThermComponent* get_singleton();

 private:
  // IO & State
  esphome::InternalGPIOPin *in_pin_{nullptr};
  esphome::InternalGPIOPin *out_pin_{nullptr};
  std::queue<uint8_t> request_queue_;
  
  uint32_t last_poll_ms_{0};
  uint32_t last_req_ms_{0};
  
  // Config variables
  uint32_t poll_interval_ms_{10000};
  uint32_t rx_timeout_ms_{1000};
  bool debug_{false};

  // Low-level Helpers
  uint32_t read_did(uint8_t did);
  bool send_frame(uint32_t frame);
  bool recv_frame(uint32_t &resp);
  void process_response(uint8_t did, uint32_t response);
  uint32_t build_request(uint8_t mt, uint8_t did, uint16_t data);
  
  static uint8_t parity32(uint32_t v);
  void line_tx_level(bool high);
  bool line_rx_level() const;
  bool wait_us(uint32_t us);
  void tx_manchester_bit(bool logical_one);
};

// Global helpers
extern CompensationMode g_compensation_mode;
void set_compensation_mode(CompensationMode m);
void set_compensation_mode_from_string(const std::string &s);

} // namespace opentherm