#pragma once

#include <cstdint>
#include <string>

namespace custom_opentherm {

class DiagnosticsController {
 public:
    DiagnosticsController() = default;

    void reset();

    void process_status_word(uint16_t status_word);
    void process_oem_diagnostic(uint16_t raw);

    bool fault_active() const;
    bool service_required() const;
    bool lockout_active() const;

    bool ch_active() const;
    bool dhw_active() const;
    bool flame_on() const;
    bool diagnostic_active() const;

    uint16_t raw_oem_error() const;
    bool has_oem_error() const;

    std::string diagnostic_text() const;

 private:
    uint16_t status_word_     = 0;
    bool has_status_word_     = false;

    uint16_t raw_oem_error_   = 0;
    bool has_oem_error_       = false;
};

}  // namespace custom_opentherm
