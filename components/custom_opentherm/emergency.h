#pragma once

namespace custom_opentherm {

class EmergencyController {
 public:
    EmergencyController() = default;

    void set_enabled(bool enabled);
    void set_target_c(float target_c);

    void set_fault_active(bool fault);

    bool is_active() const;
    float target_c() const;

 private:
    bool enabled_      = false;
    bool fault_active_ = false;
    float target_c_    = 60.0f;
};

}
