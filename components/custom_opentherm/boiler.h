#pragma once
#include "definitions.h"
#include "esphome/components/number/number.h"
#include "esphome/components/sensor/sensor.h"

namespace opentherm {

class OpenThermComponent;

class BoilerModule {
   public:
    void setup();
    void update(OpenThermComponent *ot);
    bool process_message(uint8_t id, float value);

    void set_temp_sensor(esphome::sensor::Sensor *s) {
        temp_sensor_ = s;
    }
    void set_return_sensor(esphome::sensor::Sensor *s) {
        return_sensor_ = s;
    }
    void set_modulation_sensor(esphome::sensor::Sensor *s) {
        modulation_sensor_ = s;
    }
    void set_setpoint_sensor(esphome::sensor::Sensor *s) {
        setpoint_sensor_ = s;
    }

    void set_limit_number(esphome::number::Number *n) {
        limit_number_ = n;
    }
    float get_limit_temp() const;

   private:
    esphome::sensor::Sensor *temp_sensor_{nullptr};
    esphome::sensor::Sensor *return_sensor_{nullptr};
    esphome::sensor::Sensor *modulation_sensor_{nullptr};
    esphome::sensor::Sensor *setpoint_sensor_{nullptr};

    esphome::number::Number *limit_number_{nullptr};
};

}  // namespace opentherm