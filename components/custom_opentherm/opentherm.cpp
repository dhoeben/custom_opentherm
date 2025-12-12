#include "opentherm.h"
#include "esphome/core/log.h"

namespace opentherm {

static const char *const TAG = "opentherm";

static OpenThermComponent *g_singleton = nullptr;

CompensationMode g_compensation_mode = CompensationMode::EQUITHERM;

void set_compensation_mode(CompensationMode m) { g_compensation_mode = m; }

void set_compensation_mode_from_string(const std::string &s) {
  if (s == "Boiler" || s == "boiler") {
    g_compensation_mode = CompensationMode::BOILER;
    ESP_LOGI(TAG, "Compensation mode: BOILER (internal curve).");
  } else {
    g_compensation_mode = CompensationMode::EQUITHERM;
    ESP_LOGI(TAG, "Compensation mode: EQUITHERM (external setpoint).");
  }
}

OpenThermComponent *OpenThermComponent::get_singleton() { return g_singleton; }

static constexpr uint32_t HALF_BIT_US = 500;
static constexpr uint32_t BIT_US = 1000;

OpenThermComponent::OpenThermComponent() { g_singleton = this; }

void OpenThermComponent::setup() {
  if (in_pin_)
    in_pin_->setup();
  if (out_pin_) {
    out_pin_->setup();
    out_pin_->digital_write(true);
  }

  boiler_.setup();
  dhw_.setup();

  ESP_LOGI(TAG, "OpenTherm Component Setup Complete");
}

void OpenThermComponent::loop() {
  uint32_t now = esphome::millis();

  if (now - last_poll_ms_ > poll_interval_ms_) {
    last_poll_ms_ = now;

    float target = 0.0f;
    if (emergency_.is_active()) {
      target = emergency_.get_target();
    } else {

      target = equitherm_.calculate_target(boiler_.get_limit_temp());
    }

    uint16_t raw_sp = static_cast<uint16_t>(target * 256.0f);
    send_request(OT_MSG_WRITE_DATA, OT_MSG_T_SET, raw_sp);

    boiler_.update(this);
    dhw_.update(this);
    diagnostics_.update(this);
  }

  if (!request_queue_.empty() && (now - last_req_ms_ > 100)) {
    last_req_ms_ = now;
    uint8_t did = request_queue_.front();
    request_queue_.pop();

    uint32_t response = read_did(did);
    if (response != 0) {
      process_response(did, response);
    }
  }
}

void OpenThermComponent::process_response(uint8_t did, uint32_t response) {
  uint16_t data = (response >> 8) & 0xFFFF;
  float value = static_cast<int16_t>(data) / 256.0f;

  if (boiler_.process_message(did, value))
    return;
  if (dhw_.process_message(did, data, value))
    return;
  if (diagnostics_.process_message(did, data, value))
    return;

  ESP_LOGD(TAG, "Unknown ID received: %d", did);
}

uint32_t OpenThermComponent::read_did(uint8_t did) {
  const uint32_t req = build_request(OT_MSG_READ_DATA, did, 0);
  if (!send_frame(req))
    return 0;

  uint32_t resp = 0;
  if (!recv_frame(resp)) {
    if (debug_)
      ESP_LOGW(TAG, "Timeout/bad frame for DID 0x%02X", did);
    return 0;
  }
  return resp;
}

void OpenThermComponent::send_request(uint8_t type, uint8_t id, uint16_t data) {
  uint32_t frame = build_request(type, id, data);
  send_frame(frame);
}

uint8_t OpenThermComponent::parity32(uint32_t v) {
  v >>= 1;
  v ^= v >> 16;
  v ^= v >> 8;
  v ^= v >> 4;
  v &= 0xF;
  static const uint8_t lut[16] = {0, 1, 1, 0, 1, 0, 0, 1,
                                  1, 0, 0, 1, 0, 1, 1, 0};
  return lut[v] & 1u;
}

uint32_t OpenThermComponent::build_request(uint8_t mt, uint8_t did,
                                           uint16_t data) {
  uint32_t f = 0;
  f |= (1u << 31);
  f |= (static_cast<uint32_t>(mt) & 0x7u) << 28;
  f |= (static_cast<uint32_t>(did) & 0xFFu) << 20;
  f |= (static_cast<uint32_t>(data) & 0xFFFFu) << 4;
  f |= parity32(f);
  return f;
}

void OpenThermComponent::line_tx_level(bool high) {
  out_pin_->digital_write(high);
}
bool OpenThermComponent::line_rx_level() const {
  return in_pin_->digital_read();
}

bool OpenThermComponent::wait_us(uint32_t us) {
  const int64_t start = esp_timer_get_time();
  while ((esp_timer_get_time() - start) < static_cast<int64_t>(us)) {
  }
  return true;
}

void OpenThermComponent::tx_manchester_bit(bool logical_one) {
  if (logical_one) {
    line_tx_level(true);
    wait_us(HALF_BIT_US);
    line_tx_level(false);
    wait_us(HALF_BIT_US);
  } else {
    line_tx_level(false);
    wait_us(HALF_BIT_US);
    line_tx_level(true);
    wait_us(HALF_BIT_US);
  }
}

bool OpenThermComponent::send_frame(uint32_t frame) {
  line_tx_level(true);
  wait_us(3 * BIT_US);
  for (int i = 31; i >= 0; --i)
    tx_manchester_bit((frame >> i) & 1u);
  line_tx_level(true);
  wait_us(3 * BIT_US);
  if (debug_)
    ESP_LOGD(TAG, "TX frame: 0x%08X", frame);
  return true;
}

bool OpenThermComponent::recv_frame(uint32_t &resp) {
  const int64_t overall_start = esp_timer_get_time();
  resp = 0;

  while ((esp_timer_get_time() - overall_start) <
         static_cast<int64_t>(rx_timeout_ms_) * 1000) {
    bool last = line_rx_level();
    int64_t edge_start = esp_timer_get_time();
    bool edge_found = false;

    while ((esp_timer_get_time() - edge_start) < 2 * BIT_US) {
      bool now = line_rx_level();
      if (now != last) {
        edge_found = true;
        last = now;
        break;
      }
    }

    if (!edge_found)
      continue;

    wait_us(HALF_BIT_US);

    uint32_t v = 0;
    for (int i = 31; i >= 0; --i) {
      const bool first = line_rx_level();
      wait_us(HALF_BIT_US);
      const bool second = line_rx_level();

      if (first && !second) {
        v = (v << 1) | 1u;
      } else if (!first && second) {
        v = (v << 1);
      } else {

        v = 0;
        goto next_try;
      }
    }

    resp = v;
    if ((resp & 0x1u) != parity32(resp)) {
      if (debug_)
        ESP_LOGW(TAG, "Parity error 0x%08X", resp);
      resp = 0;
      goto next_try;
    }

    if (debug_)
      ESP_LOGD(TAG, "RX frame OK: 0x%08X", resp);
    return true;

  next_try:
    continue;
  }
  return false;
}

} // namespace opentherm