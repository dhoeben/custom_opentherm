#pragma once
#include <cstdint>

namespace opentherm {

enum OpenThermMessageType : uint8_t {
    OT_MSG_READ_DATA    = 0,
    OT_MSG_WRITE_DATA   = 1,
    OT_MSG_INVALID      = 2,
    OT_MSG_RESERVED     = 3,
    OT_MSG_READ_ACK     = 4,
    OT_MSG_WRITE_ACK    = 5,
    OT_MSG_DATA_INVALID = 6,
    OT_MSG_UNKNOWN_ID   = 7
};

enum OpenThermMessageID : uint8_t {
    OT_MSG_STATUS      = 0,
    OT_MSG_T_SET       = 1,
    OT_MSG_FAULT_FLAGS = 1,
    OT_MSG_OEM_CODE    = 2,

    OT_MSG_REL_MOD_LEVEL     = 17,
    OT_MSG_CH_WATER_PRESSURE = 18,
    OT_MSG_CH_WATER_TEMP     = 25,
    OT_MSG_DHW_TEMP          = 26,
    OT_MSG_T_OUTSIDE         = 27,
    OT_MSG_RETURN_WATER_TEMP = 28,

    OT_MSG_BOILER_TEMP_LEGACY = 24,
    OT_MSG_RETURN_TEMP_LEGACY = 25,

    OT_MSG_SOLAR_STORAGE = 29,

    OT_MSG_DHW_BOUNDS    = 48,
    OT_MSG_DHW_SETPOINT  = 56,
    OT_MSG_DHW_COMFORT   = 51,
    OT_MSG_DHW_FLOW_RATE = 62,

    OT_MSG_OEM_DIAGNOSTIC = 115
};

}  // namespace opentherm