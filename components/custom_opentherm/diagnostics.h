#pragma once

namespace opentherm {

// Forward declaration van de main component class
class OpenThermComponent;

namespace Diagnostics {

void update(OpenThermComponent *ot);

}  // namespace Diagnostics
}  // namespace opentherm
