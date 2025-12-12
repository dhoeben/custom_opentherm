#pragma once
#include <string> 
#include <vector>

namespace opentherm {

class OpenThermComponent;

namespace Diagnostics {

void update(OpenThermComponent *ot);
void process_status(uint16_t data);
void process_fault_flags(uint16_t data);
void process_flow_rate(float value);

}  // namespace Diagnostics
}  // namespace opentherm