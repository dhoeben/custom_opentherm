#pragma once

#include <cstdint>

namespace custom_opentherm {
    
struct TimingConfig {
    uint32_t poll_interval_ms = 10000;
    uint32_t rx_timeout_ms    = 40;
};

struct LimitsConfig {
    float max_boiler_temp_heating_c = 80.0f;
    float max_boiler_temp_water_c   = 60.0f;
};

struct EquithermConfig {
    float fb_gain = 0.5f;
    float k       = 1.2f;
    float n       = 0.8f;
    float t       = 20.0f;
};

enum class RequestKind : uint8_t {
    Read,
    Write,
};

enum class RequestPriority : uint8_t {
    Low,
    High,
};

struct Request {
    RequestKind     kind;
    RequestPriority priority;
    uint8_t         did;
    uint16_t        data;
};

}  // namespace custom_opentherm
