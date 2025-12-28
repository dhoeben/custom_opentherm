#include "emergency.h"

namespace custom_opentherm {

void EmergencyController::set_enabled(bool enabled) {
    enabled_ = enabled;
}

void EmergencyController::set_target_c(float target_c) {
    target_c_ = target_c;
}

void EmergencyController::set_fault_active(bool fault) {
    fault_active_ = fault;
}

bool EmergencyController::is_active() const {
    return enabled_ || fault_active_;
}

float EmergencyController::target_c() const {
    return target_c_;
}

}
