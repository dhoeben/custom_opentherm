#include "diagnostics.h"

#include <string>

namespace custom_opentherm {

static constexpr uint8_t STATUS_FAULT_BIT      = 0;
static constexpr uint8_t STATUS_CH_ACTIVE_BIT  = 1;
static constexpr uint8_t STATUS_DHW_ACTIVE_BIT = 2;
static constexpr uint8_t STATUS_FLAME_BIT      = 3;
static constexpr uint8_t STATUS_DIAG_BIT       = 6;

void DiagnosticsController::reset() {
    status_word_     = 0;
    has_status_word_ = false;
    raw_oem_error_   = 0;
    has_oem_error_   = false;
}

void DiagnosticsController::process_status_word(uint16_t status_word) {
    status_word_     = status_word;
    has_status_word_ = true;
}

void DiagnosticsController::process_oem_diagnostic(uint16_t raw) {
    raw_oem_error_ = raw;
    has_oem_error_ = true;
}

bool DiagnosticsController::fault_active() const {
    return has_status_word_ && ((status_word_ >> STATUS_FAULT_BIT) & 1u);
}

bool DiagnosticsController::ch_active() const {
    return has_status_word_ && ((status_word_ >> STATUS_CH_ACTIVE_BIT) & 1u);
}

bool DiagnosticsController::dhw_active() const {
    return has_status_word_ && ((status_word_ >> STATUS_DHW_ACTIVE_BIT) & 1u);
}

bool DiagnosticsController::flame_on() const {
    return has_status_word_ && ((status_word_ >> STATUS_FLAME_BIT) & 1u);
}

bool DiagnosticsController::diagnostic_active() const {
    return has_status_word_ && ((status_word_ >> STATUS_DIAG_BIT) & 1u);
}

bool DiagnosticsController::service_required() const {
    return diagnostic_active() && !fault_active();
}

bool DiagnosticsController::lockout_active() const {
    return fault_active();
}

uint16_t DiagnosticsController::raw_oem_error() const {
    return raw_oem_error_;
}

bool DiagnosticsController::has_oem_error() const {
    return has_oem_error_;
}

std::string DiagnosticsController::diagnostic_text() const {
    if (!has_status_word_) return "no data";
    if (lockout_active()) return "lockout";
    if (fault_active()) return "fault";
    if (service_required()) return "service required";
    if (diagnostic_active()) return "diagnostic";
    return "ok";
}

}  // namespace custom_opentherm
