#pragma once
#include <string>

#include "definitions.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"

namespace opentherm {

class OpenThermComponent;

class DiagnosticsModule {
   public:
    void update(OpenThermComponent *ot);
    bool process_message(uint8_t id, uint16_t data, float value);

    bool is_dhw_active() const {
        return dhw_active_state_;
    }

    void set_fault_sensor(esphome::binary_sensor::BinarySensor *s) {
        fault_ = s;
    }
    void set_ch_active_sensor(esphome::binary_sensor::BinarySensor *s) {
        ch_active_ = s;
    }
    void set_dhw_active_sensor(esphome::binary_sensor::BinarySensor *s) {
        dhw_active_ = s;
    }
    void set_flame_sensor(esphome::binary_sensor::BinarySensor *s) {
        flame_ = s;
    }
    void set_comms_ok_sensor(esphome::binary_sensor::BinarySensor *s) {
        comms_ok_ = s;
    }

    void set_fault_text_sensor(esphome::text_sensor::TextSensor *s) {
        fault_text_ = s;
    }
    void set_flow_rate_sensor(esphome::sensor::Sensor *s) {
        flow_rate_ = s;
    }

   private:
    esphome::binary_sensor::BinarySensor *fault_{nullptr};
    esphome::binary_sensor::BinarySensor *ch_active_{nullptr};
    esphome::binary_sensor::BinarySensor *dhw_active_{nullptr};
    esphome::binary_sensor::BinarySensor *flame_{nullptr};
    esphome::binary_sensor::BinarySensor *comms_ok_{nullptr};

    esphome::text_sensor::TextSensor *fault_text_{nullptr};
    esphome::sensor::Sensor          *flow_rate_{nullptr};

    uint32_t last_rx_time_{0};
    bool     last_comms_state_{false};
    bool     dhw_active_state_{false};

    std::string decode_fault_flags(uint16_t data);
};

}  // namespace opentherm