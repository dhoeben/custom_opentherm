#include "opentherm.h"

#include <algorithm>
#include <cstdint>
#include <string>

#include "esphome/core/log.h"

namespace custom_opentherm {

static const char *const TAG = "custom_opentherm";

static constexpr uint32_t COMMS_TIMEOUT_MS = 3000;

static constexpr uint8_t MASTER_STATUS_CH_ENABLE_BIT  = 0;
static constexpr uint8_t MASTER_STATUS_DHW_ENABLE_BIT = 1;

static constexpr uint32_t SPACING_DECREASE_OK_MS   = 10;
static constexpr uint32_t SPACING_INCREASE_FAIL_MS = 50;

void OpenThermComponent::setup() {
    protocol_.set_pins(in_pin_, out_pin_);
    protocol_.set_rx_timeout_ms(timing_.rx_timeout_ms);
    protocol_.set_debug(debug_enabled_);
    protocol_.setup();

    boiler_.reset();
    dhw_.reset();
    diagnostics_.reset();

    last_poll_ms_     = esphome::millis();
    last_req_ms_      = last_poll_ms_;
    last_valid_rx_ms_ = 0;

    comms_ok_           = false;
    outside_valid_      = false;
    current_spacing_ms_ = std::clamp(current_spacing_ms_, SPACING_MIN_MS, SPACING_MAX_MS);

    while (!high_prio_queue_.empty()) high_prio_queue_.pop();
    while (!low_prio_queue_.empty()) low_prio_queue_.pop();

    ESP_LOGI(TAG, "Custom OpenTherm gateway initialized");
}

void OpenThermComponent::loop() {
    const uint32_t now = esphome::millis();

    if (now - last_poll_ms_ >= timing_.poll_interval_ms) {
        last_poll_ms_ = now;
        poll_();
        publish_();
    }

    process_queue_();
    update_comms_ok_(now);
    update_mode_text_();
}

void OpenThermComponent::poll_() {
    refresh_runtime_inputs_();
    schedule_periodic_requests_();
    schedule_control_requests_();
}

void OpenThermComponent::refresh_runtime_inputs_() {
    if (max_boiler_temp_heating_number_ != nullptr) {
        limits_.max_boiler_temp_heating_c = max_boiler_temp_heating_number_->state;
    }

    if (max_boiler_temp_water_number_ != nullptr) {
        limits_.max_boiler_temp_water_c = max_boiler_temp_water_number_->state;
    }

    const float fb_gain = (eq_fb_gain_number_ != nullptr) ? eq_fb_gain_number_->state : 0.5f;
    const float k       = (eq_k_number_ != nullptr) ? eq_k_number_->state : 1.2f;
    const float n       = (eq_n_number_ != nullptr) ? eq_n_number_->state : 0.8f;
    const float t       = (eq_t_number_ != nullptr) ? eq_t_number_->state : 20.0f;

    equitherm_.set_parameters(fb_gain, k, n, t);
    equitherm_.set_limits(0.0f, limits_.max_boiler_temp_heating_c);

    emergency_.set_enabled(emergency_switch_ != nullptr && emergency_switch_->state);
}

void OpenThermComponent::schedule_periodic_requests_() {
    enqueue_read_(OT_MSG_STATUS);
    enqueue_read_(OT_MSG_REL_MOD_LEVEL);
    enqueue_read_(OT_MSG_CH_WATER_PRESSURE);
    enqueue_read_(OT_MSG_CH_WATER_TEMP);
    enqueue_read_(OT_MSG_RETURN_WATER_TEMP);
    enqueue_read_(OT_MSG_DHW_TEMP);
    enqueue_read_(OT_MSG_DHW_FLOW_RATE);
    enqueue_read_(OT_MSG_T_OUTSIDE);
    enqueue_read_(OT_MSG_OEM_DIAGNOSTIC);
}

bool OpenThermComponent::get_master_ch_enable_() const {
    if (force_heat_switch_ != nullptr && force_heat_switch_->state) return true;
    return last_target_setpoint_c_ > 0.0f;
}

bool OpenThermComponent::get_master_dhw_enable_() const {
    if (force_dhw_switch_ != nullptr && force_dhw_switch_->state) return true;
    return false;
}

void OpenThermComponent::schedule_control_requests_() {
    if (outside_valid_) {
        equitherm_.set_outside_temp(last_outside_c_);
    }

    if (ch_climate_ != nullptr) {
        if (ch_climate_->current_temperature.has_value()) {
            equitherm_.set_room_temp(*ch_climate_->current_temperature);
        }
        if (ch_climate_->target_temperature.has_value()) {
            equitherm_.set_room_setpoint(*ch_climate_->target_temperature);
        }
    }

    emergency_.set_fault_active(diagnostics_.fault_active());

    float target_c = emergency_.is_active() ? emergency_.target_c()
                                           : equitherm_.compute_target(limits_.max_boiler_temp_heating_c);

    target_c                = std::clamp(target_c, 0.0f, limits_.max_boiler_temp_heating_c);
    last_target_setpoint_c_ = target_c;

    uint8_t hb = 0;
    if (get_master_ch_enable_()) hb |= (1u << MASTER_STATUS_CH_ENABLE_BIT);
    if (get_master_dhw_enable_()) hb |= (1u << MASTER_STATUS_DHW_ENABLE_BIT);

    enqueue_write_(OT_MSG_STATUS, static_cast<uint16_t>(hb) << 8);
    enqueue_write_(OT_MSG_T_SET, static_cast<uint16_t>(target_c * 256.0f));
}

void OpenThermComponent::enqueue_read_(uint8_t did) {
    if (low_prio_queue_.size() >= MAX_LOW_PRIO_QUEUE) {
        low_prio_queue_.pop();
    }
    low_prio_queue_.push(Request{RequestKind::Read, RequestPriority::Low, did, 0});
}

void OpenThermComponent::enqueue_write_(uint8_t did, uint16_t data) {
    high_prio_queue_.push(Request{RequestKind::Write, RequestPriority::High, did, data});
}

void OpenThermComponent::process_queue_() {
    const uint32_t now = esphome::millis();

    if (now - last_req_ms_ < current_spacing_ms_) return;
    if (high_prio_queue_.empty() && low_prio_queue_.empty()) return;

    last_req_ms_ = now;

    Request req;
    if (!high_prio_queue_.empty()) {
        req = high_prio_queue_.front();
        high_prio_queue_.pop();
    } else {
        req = low_prio_queue_.front();
        low_prio_queue_.pop();
    }

    bool ok = false;

    if (req.kind == RequestKind::Write) {
        ok = protocol_.write(req.did, req.data);
        if (ok) {
            current_spacing_ms_ =
                std::max(SPACING_MIN_MS, current_spacing_ms_ - SPACING_DECREASE_OK_MS);
        } else {
            current_spacing_ms_ =
                std::min(SPACING_MAX_MS, current_spacing_ms_ + SPACING_INCREASE_FAIL_MS);
        }
        return;
    }

    uint32_t resp = 0;
    ok            = protocol_.read(req.did, resp);

    if (ok) {
        last_valid_rx_ms_   = now;
        current_spacing_ms_ = std::max(SPACING_MIN_MS, current_spacing_ms_ - SPACING_DECREASE_OK_MS);
        dispatch_response_(req.did, resp);
    } else {
        current_spacing_ms_ = std::min(SPACING_MAX_MS, current_spacing_ms_ + SPACING_INCREASE_FAIL_MS);
    }
}

void OpenThermComponent::dispatch_response_(uint8_t did, uint32_t response) {
    const uint16_t raw   = (response >> 8) & 0xFFFF;
    const float value_c = static_cast<int16_t>(raw) / 256.0f;

    if (did == OT_MSG_STATUS) {
        diagnostics_.process_status_word(raw);
        return;
    }

    if (did == OT_MSG_OEM_DIAGNOSTIC) {
        diagnostics_.process_oem_diagnostic(raw);
        return;
    }

    if (did == OT_MSG_T_OUTSIDE) {
        last_outside_c_ = value_c;
        outside_valid_  = true;
        return;
    }

    if (boiler_.process_message(did, raw, value_c)) return;
    if (dhw_.process_message(did, raw, value_c)) return;

    if (debug_enabled_) {
        ESP_LOGD(TAG, "Unhandled OpenTherm DID: %u", did);
    }
}

void OpenThermComponent::update_comms_ok_(uint32_t now_ms) {
    comms_ok_ = (last_valid_rx_ms_ != 0) && (now_ms - last_valid_rx_ms_ <= COMMS_TIMEOUT_MS);
}

void OpenThermComponent::update_mode_text_() {
    if (mode_text_ != nullptr) {
        mode_text_->publish_state(emergency_.is_active() ? "emergency" : "equitherm");
    }
}

void OpenThermComponent::publish_() {
    if (queue_depth_sensor_ != nullptr) {
        queue_depth_sensor_->publish_state(
            static_cast<float>(high_prio_queue_.size() + low_prio_queue_.size()));
    }

    if (boiler_temp_sensor_ != nullptr) boiler_temp_sensor_->publish_state(boiler_.flow_temp_c());
    if (return_temp_sensor_ != nullptr) return_temp_sensor_->publish_state(boiler_.return_temp_c());
    if (modulation_sensor_ != nullptr) modulation_sensor_->publish_state(boiler_.modulation_percent());
    if (pressure_sensor_ != nullptr) pressure_sensor_->publish_state(boiler_.pressure_bar());
    if (dhw_temp_sensor_ != nullptr) dhw_temp_sensor_->publish_state(dhw_.temperature_c());
    if (dhw_flow_rate_sensor_ != nullptr) dhw_flow_rate_sensor_->publish_state(dhw_.flow_rate_l_min());
    if (outside_temp_sensor_ != nullptr && outside_valid_) outside_temp_sensor_->publish_state(last_outside_c_);
    if (setpoint_sensor_ != nullptr) setpoint_sensor_->publish_state(last_target_setpoint_c_);

    if (fault_binary_ != nullptr) fault_binary_->publish_state(diagnostics_.fault_active());
    if (service_binary_ != nullptr) service_binary_->publish_state(diagnostics_.service_required());
    if (lockout_binary_ != nullptr) lockout_binary_->publish_state(diagnostics_.lockout_active());

    if (ch_active_binary_ != nullptr) ch_active_binary_->publish_state(diagnostics_.ch_active());
    if (dhw_active_binary_ != nullptr) dhw_active_binary_->publish_state(diagnostics_.dhw_active());
    if (flame_on_binary_ != nullptr) flame_on_binary_->publish_state(diagnostics_.flame_on());

    if (tap_flow_binary_ != nullptr) tap_flow_binary_->publish_state(dhw_.tap_flow_active());
    if (dhw_preheat_binary_ != nullptr) dhw_preheat_binary_->publish_state(dhw_.preheat_active());

    if (status_text_ != nullptr) status_text_->publish_state(diagnostics_.diagnostic_text());
    if (boiler_status_text_ != nullptr) boiler_status_text_->publish_state(diagnostics_.diagnostic_text());
    if (diagnostic_text_ != nullptr) diagnostic_text_->publish_state(diagnostics_.diagnostic_text());
}

}  // namespace custom_opentherm
