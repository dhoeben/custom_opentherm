#include "protocol.h"

#include "esphome/core/log.h"

namespace custom_opentherm {

static const char *const TAG = "custom_opentherm.protocol";

static constexpr uint32_t HALF_BIT_US = 500;
static constexpr uint32_t BIT_US      = 1000;

void OpenThermProtocol::set_pins(GPIOPin *in_pin, GPIOPin *out_pin) {
    in_pin_  = in_pin;
    out_pin_ = out_pin;
}

void OpenThermProtocol::set_rx_timeout_ms(uint32_t rx_timeout_ms) {
    rx_timeout_ms_ = rx_timeout_ms;
}

void OpenThermProtocol::set_debug(bool enabled) {
    debug_enabled_ = enabled;
}

void OpenThermProtocol::setup() {
    if (in_pin_) {
        in_pin_->setup();
        in_pin_->pin_mode(gpio::FLAG_INPUT);
    }
    if (out_pin_) {
        out_pin_->setup();
        idle_line_();
    }
}

bool OpenThermProtocol::read_blocking(uint8_t did, uint32_t &response) {
    const uint32_t frame = build_request_(0, did, 0);

    if (!send_frame_(frame)) {
        return false;
    }

    uint32_t resp = 0;
    if (!recv_frame_(resp)) {
        if (debug_enabled_) {
            ESP_LOGW(TAG, "Timeout/bad frame for DID 0x%02X", did);
        }
        return false;
    }

    response = resp;
    return true;
}

bool OpenThermProtocol::write_blocking(uint8_t did, uint16_t data) {
    const uint32_t frame = build_request_(1, did, data);
    return send_frame_(frame);
}

uint8_t OpenThermProtocol::parity32_(uint32_t v) const {
    v >>= 1;
    v ^= v >> 16;
    v ^= v >> 8;
    v ^= v >> 4;
    v &= 0xF;

    static const uint8_t lut[16] = {
        0, 1, 1, 0,
        1, 0, 0, 1,
        1, 0, 0, 1,
        0, 1, 1, 0
    };

    return lut[v] & 1u;
}

uint32_t OpenThermProtocol::build_request_(uint8_t msg_type, uint8_t did, uint16_t data) const {
    uint32_t f = 0;
    f |= (1u << 31);
    f |= (static_cast<uint32_t>(msg_type) & 0x7u) << 28;
    f |= (static_cast<uint32_t>(did) & 0xFFu) << 20;
    f |= (static_cast<uint32_t>(data) & 0xFFFFu) << 4;
    f |= parity32_(f);
    return f;
}

void OpenThermProtocol::idle_line_() {
    line_tx_level_(true);
}

void OpenThermProtocol::line_tx_level_(bool high) {
    if (out_pin_) {
        out_pin_->digital_write(high);
    }
}

bool OpenThermProtocol::line_rx_level_() const {
    return in_pin_ ? in_pin_->digital_read() : true;
}

void OpenThermProtocol::wait_us_(uint32_t us) const {
    const int64_t start = esp_timer_get_time();
    while ((esp_timer_get_time() - start) < static_cast<int64_t>(us)) {
    }
}

void OpenThermProtocol::tx_manchester_bit_(bool logical_one) {
    if (logical_one) {
        line_tx_level_(true);
        wait_us_(HALF_BIT_US);
        line_tx_level_(false);
        wait_us_(HALF_BIT_US);
    } else {
        line_tx_level_(false);
        wait_us_(HALF_BIT_US);
        line_tx_level_(true);
        wait_us_(HALF_BIT_US);
    }
}

bool OpenThermProtocol::send_frame_(uint32_t frame) {
    idle_line_();
    wait_us_(3 * BIT_US);

    for (int i = 31; i >= 0; --i) {
        tx_manchester_bit_((frame >> i) & 1u);
    }

    idle_line_();
    wait_us_(3 * BIT_US);

    if (debug_enabled_) {
        ESP_LOGD(TAG, "TX frame: 0x%08X", frame);
    }

    return true;
}

bool OpenThermProtocol::recv_frame_(uint32_t &resp) {
    const int64_t start = esp_timer_get_time();
    resp = 0;

    while ((esp_timer_get_time() - start) < static_cast<int64_t>(rx_timeout_ms_) * 1000) {
        bool last = line_rx_level_();

        const int64_t edge_start = esp_timer_get_time();
        while ((esp_timer_get_time() - edge_start) < 2 * BIT_US) {
            const bool now = line_rx_level_();
            if (now != last) {
                last = now;
                break;
            }
        }

        wait_us_(HALF_BIT_US);

        uint32_t v = 0;
        for (int i = 31; i >= 0; --i) {
            const bool a = line_rx_level_();
            wait_us_(HALF_BIT_US);
            const bool b = line_rx_level_();

            if (a && !b) {
                v = (v << 1) | 1u;
            } else if (!a && b) {
                v <<= 1;
            } else {
                goto retry;
            }
        }

        if ((v & 1u) != parity32_(v)) {
            if (debug_enabled_) {
                ESP_LOGW(TAG, "Parity error 0x%08X", v);
            }
            goto retry;
        }

        resp = v;

        if (debug_enabled_) {
            ESP_LOGD(TAG, "RX frame OK: 0x%08X", resp);
        }

        return true;

    retry:
        continue;
    }

    return false;
}

}  // namespace custom_opentherm
