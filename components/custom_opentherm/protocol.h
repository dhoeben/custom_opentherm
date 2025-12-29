#pragma once

#include <cstdint>

#include "esphome/core/hal.h"

namespace custom_opentherm {

class OpenThermProtocol {
 public:
    void set_pins(esphome::GPIOPin *in_pin, esphome::GPIOPin *out_pin);
    void set_rx_timeout_ms(uint32_t rx_timeout_ms);
    void set_debug(bool enabled);
    void setup();

    bool read(uint8_t did, uint32_t &response);
    bool write(uint8_t did, uint16_t data);

 private:
    bool read_blocking(uint8_t did, uint32_t &response);
    bool write_blocking(uint8_t did, uint16_t data);

    uint32_t build_request_(uint8_t msg_type, uint8_t did, uint16_t data) const;
    uint8_t parity32_(uint32_t v) const;

    void idle_line_();
    void line_tx_level_(bool high);
    bool line_rx_level_() const;

    void wait_us_(uint32_t us) const;
    void tx_manchester_bit_(bool logical_one);

    bool send_frame_(uint32_t frame);
    bool recv_frame_(uint32_t &resp);

    esphome::GPIOPin *in_pin_{nullptr};
    esphome::GPIOPin *out_pin_{nullptr};

    uint32_t rx_timeout_ms_{1000};
    bool debug_enabled_{false};
};

}  // namespace custom_opentherm
