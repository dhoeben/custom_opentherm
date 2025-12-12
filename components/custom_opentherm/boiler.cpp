#include "boiler.h"
#include "sensors.h"
#include "opentherm.h"
#include "esphome/core/log.h"

using namespace esphome;

namespace opentherm {
namespace Boiler {

static const char *const TAG = "ot_boiler";

// keep convenience handle (assigned in OpenThermComponent::setup)
esphome::number::Number *max_heating_temp = nullptr;

void update(OpenThermComponent *ot) {
  if (!ot) return;
  ot->enqueue_request(0x18); // Water Temp
  ot->enqueue_request(0x19); // Return Temp
  ot->enqueue_request(0x11); 
  ot->enqueue_request(0x1D);
}

} // namespace Boiler
} // namespace opentherm
