#include "boiler.h"
#include "opentherm.h"
#include "esphome/core/log.h"

namespace opentherm {

static const char *const TAG = "ot_boiler";

void BoilerModule::setup() {
  if (limit_number_) {
    // Defaults instellen als dat nog niet gebeurd is
    if (!limit_number_->has_state()) {
        limit_number_->publish_state(60.0f); // Default 60 graden
    }
  }
}

void BoilerModule::update(OpenThermComponent *ot) {
  // Bestellingen plaatsen met de nieuwe ID namen
  ot->enqueue_request(OT_MSG_CH_WATER_TEMP);   // 0x19
  ot->enqueue_request(OT_MSG_RETURN_WATER_TEMP); // 0x1C (Let op: check of jouw ketel 0x1C of 0x19 return is, standaard is 28/0x1C voor Return)
  // Correctie: In je oude code gebruikte je 0x19 voor return? 
  // Standaard OpenTherm: ID 25 (0x19) = Boiler Flow, ID 28 (0x1C) = Return.
  // Ik houd jouw oude mapping aan voor ID 25/26 als je dat gebruikte, maar check dit!
  // Jouw oude code: 0x18 (24) -> Boiler Temp, 0x19 (25) -> Return Temp.
  // Dat is een beetje afwijkend van de standaard, maar ik respecteer je oude code:
  
  ot->enqueue_request(0x18); // Boiler Temp (Standard ID 25 usually, but you used 0x18?)
  ot->enqueue_request(0x19); // Return Temp
  
  ot->enqueue_request(OT_MSG_REL_MOD_LEVEL);   // 0x11
  ot->enqueue_request(OT_MSG_SOLAR_STORAGE);   // 0x1D (Fallback modulation)
}

bool BoilerModule::process_message(uint8_t id, float value) {
  switch (id) {
    case 0x18: // Jouw Boiler Temp ID
      if (temp_sensor_) temp_sensor_->publish_state(value);
      return true;
    case 0x19: // Jouw Return Temp ID
      if (return_sensor_) return_sensor_->publish_state(value);
      return true;
    case OT_MSG_REL_MOD_LEVEL:
      if (modulation_sensor_) modulation_sensor_->publish_state(value);
      // Soms zit setpoint ook in ID 17, apart uitlezen:
      if (setpoint_sensor_) setpoint_sensor_->publish_state(value);
      return true;
    case OT_MSG_SOLAR_STORAGE: // 0x1D
      // Als dit ook modulatie is voor jouw ketel:
      if (modulation_sensor_) modulation_sensor_->publish_state(value);
      return true;
    default:
      return false;
  }
}

float BoilerModule::get_limit_temp() const {
    if (limit_number_ && limit_number_->has_state()) {
        return limit_number_->state;
    }
    return 60.0f; // Default safe
}

} // namespace opentherm