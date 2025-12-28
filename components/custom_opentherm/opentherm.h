#pragma once

#include <cstdint>
#include <queue>
#include <string>

#include "esphome/core/component.h"
#include "esphome/core/hal.h"

#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/climate/climate.h"
#include "esphome/components/number/number.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/text_sensor/text_sensor.h"

#include "boiler.h"
#include "dhw.h"
#include "diagnostics.h"
#include "emergency.h"
#include "equitherm.h"
#include "protocol.h"
#include "types.h"

namespace custom_opentherm {

using GPIOPin      = esphome::GPIOPin;
using Sensor       = esphome::sensor::Sensor;
using Number       = esphome::number::Number;
using BinarySensor = esphome::binary_sensor::BinarySensor;
using TextSensor   = esphome::text_sensor::TextSensor;
using Switch       = esphome::switch_::Switch;
using Climate      = esphome::climate::Climate;

class OpenThermComponent : public esphome::Component {
 public:
    OpenThermComponent() = default;

    void setup() override;
    void loop() override;

    void set_pins(GPIOPin *in_pin, GPIOPin *out_pin) {
        in_pin_  = in_pin;
        out_pin_ = out_pin;
    }

    void set_timing(const TimingConfig &timing) { timing_ = timing; }
    void set_limits(const LimitsConfig &limits) { limits_ = limits; }
    void set_debug(bool enabled) { debug_enabled_ = enabled; }

    void bind_boiler_temp_sensor(Sensor *s) { boiler_temp_sensor_ = s; }
    void bind_return_temp_sensor(Sensor *s) { return_temp_sensor_ = s; }
    void bind_modulation_sensor(Sensor *s) { modulation_sensor_ = s; }
    void bind_pressure_sensor(Sensor *s) { pressure_sensor_ = s; }
    void bind_dhw_temp_sensor(Sensor *s) { dhw_temp_sensor_ = s; }
    void bind_dhw_flow_rate_sensor(Sensor *s) { dhw_flow_rate_sensor_ = s; }
    void bind_outside_temp_sensor(Sensor *s) { outside_temp_sensor_ = s; }
    void bind_setpoint_sensor(Sensor *s) { setpoint_sensor_ = s; }
    void bind_queue_depth_sensor(Sensor *s) { queue_depth_sensor_ = s; }

    void bind_max_boiler_temp_heating_number(Number *n) {
        max_boiler_temp_heating_number_ = n;
    }
    void bind_max_boiler_temp_water_number(Number *n) {
        max_boiler_temp_water_number_ = n;
    }

    void bind_eq_fb_gain_number(Number *n) { eq_fb_gain_number_ = n; }
    void bind_eq_k_number(Number *n) { eq_k_number_ = n; }
    void bind_eq_n_number(Number *n) { eq_n_number_ = n; }
    void bind_eq_t_number(Number *n) { eq_t_number_ = n; }

    void bind_ch_climate(Climate *c) { ch_climate_ = c; }
    void bind_emergency_switch(Switch *s) { emergency_switch_ = s; }
    void bind_force_heat_switch(Switch *s) { force_heat_switch_ = s; }
    void bind_force_dhw_switch(Switch *s) { force_dhw_switch_ = s; }

    void bind_fault_binary(BinarySensor *b) { fault_binary_ = b; }
    void bind_service_binary(BinarySensor *b) { service_binary_ = b; }
    void bind_lockout_binary(BinarySensor *b) { lockout_binary_ = b; }
    void bind_ch_active_binary(BinarySensor *b) { ch_active_binary_ = b; }
    void bind_dhw_active_binary(BinarySensor *b) { dhw_active_binary_ = b; }
    void bind_flame_on_binary(BinarySensor *b) { flame_on_binary_ = b; }
    void bind_tap_flow_binary(BinarySensor *b) { tap_flow_binary_ = b; }
    void bind_dhw_preheat_binary(BinarySensor *b) { dhw_preheat_binary_ = b; }

    void bind_mode_text(TextSensor *t) { mode_text_ = t; }
    void bind_status_text(TextSensor *t) { status_text_ = t; }
    void bind_boiler_status_text(TextSensor *t) { boiler_status_text_ = t; }
    void bind_diagnostic_text(TextSensor *t) { diagnostic_text_ = t; }

 protected:
    void poll_();
    void publish_();

    void refresh_runtime_inputs_();
    void schedule_periodic_requests_();
    void schedule_control_requests_();

    void enqueue_read_(uint8_t did);
    void enqueue_write_(uint8_t did, uint16_t data);
    void process_queue_();

    void dispatch_response_(uint8_t did, uint32_t response);

    void update_comms_ok_(uint32_t now_ms);
    void update_mode_text_();

    bool get_master_ch_enable_() const;
    bool get_master_dhw_enable_() const;

 protected:
    GPIOPin *in_pin_  = nullptr;
    GPIOPin *out_pin_ = nullptr;

    Sensor *boiler_temp_sensor_   = nullptr;
    Sensor *return_temp_sensor_   = nullptr;
    Sensor *modulation_sensor_    = nullptr;
    Sensor *pressure_sensor_      = nullptr;
    Sensor *dhw_temp_sensor_      = nullptr;
    Sensor *dhw_flow_rate_sensor_ = nullptr;
    Sensor *outside_temp_sensor_  = nullptr;
    Sensor *setpoint_sensor_      = nullptr;
    Sensor *queue_depth_sensor_   = nullptr;

    Number *max_boiler_temp_heating_number_ = nullptr;
    Number *max_boiler_temp_water_number_   = nullptr;

    Number *eq_fb_gain_number_ = nullptr;
    Number *eq_k_number_       = nullptr;
    Number *eq_n_number_       = nullptr;
    Number *eq_t_number_       = nullptr;

    Climate *ch_climate_ = nullptr;

    Switch *emergency_switch_  = nullptr;
    Switch *force_heat_switch_ = nullptr;
    Switch *force_dhw_switch_  = nullptr;

    BinarySensor *fault_binary_       = nullptr;
    BinarySensor *service_binary_     = nullptr;
    BinarySensor *lockout_binary_     = nullptr;
    BinarySensor *ch_active_binary_   = nullptr;
    BinarySensor *dhw_active_binary_  = nullptr;
    BinarySensor *flame_on_binary_    = nullptr;
    BinarySensor *tap_flow_binary_    = nullptr;
    BinarySensor *dhw_preheat_binary_ = nullptr;

    TextSensor *mode_text_          = nullptr;
    TextSensor *status_text_        = nullptr;
    TextSensor *boiler_status_text_ = nullptr;
    TextSensor *diagnostic_text_    = nullptr;

    TimingConfig timing_;
    LimitsConfig limits_;

    OpenThermProtocol protocol_;
    BoilerController boiler_;
    DhwController dhw_;
    DiagnosticsController diagnostics_;
    EmergencyController emergency_;
    EquithermController equitherm_;

    std::queue<Request> high_prio_queue_;
    std::queue<Request> low_prio_queue_;

    static constexpr size_t MAX_LOW_PRIO_QUEUE = 16;

    uint32_t last_poll_ms_     = 0;
    uint32_t last_req_ms_      = 0;
    uint32_t last_valid_rx_ms_ = 0;

    uint32_t current_spacing_ms_ = 100;
    static constexpr uint32_t SPACING_MIN_MS = 60;
    static constexpr uint32_t SPACING_MAX_MS = 500;

    bool comms_ok_      = false;
    bool debug_enabled_ = false;

    float last_outside_c_         = 0.0f;
    bool outside_valid_           = false;
    float last_target_setpoint_c_ = 0.0f;
};

}  // namespace custom_opentherm
