#pragma once
#include <cstdint>

enum OpenThermMessageID : uint8_t {
    OT_MSG_STATUS          = 0,    // 0x00 - Master/Slave Status flags
    OT_MSG_T_SET           = 1,    // 0x01 - Control Setpoint (Fault flags in read-ack)
    OT_MSG_FAULT_FLAGS     = 1,    // 0x01 - Application Specific Fault Flags
    OT_MSG_OEM_CODE        = 2,    // 0x02 - OEM Code
    OT_MSG_REL_MOD_LEVEL   = 17,   // 0x11 - Relative Modulation Level
    OT_MSG_CH_WATER_PRESSURE = 18, // 0x12 - Water pressure
    OT_MSG_CH_WATER_TEMP   = 25,   // 0x19 - Boiler Flow Temperature
    OT_MSG_DHW_TEMP        = 26,   // 0x1A - DHW Temperature
    OT_MSG_T_OUTSIDE       = 27,   // 0x1B - Outside Temperature
    OT_MSG_RETURN_WATER_TEMP = 28, // 0x1C - Return Water Temperature
    
    // Specifieke / Afwijkende mappings uit jouw project
    // Jouw oude code gebruikte 0x18 (24) voor Boiler Temp en 0x19 (25) voor Return. 
    // Standaard OT is 25=Flow, 28=Return. Ik definieer ze hier zodat de namen kloppen met jouw ketel.
    OT_MSG_BOILER_TEMP_LEGACY = 24, // 0x18
    OT_MSG_RETURN_TEMP_LEGACY = 25, // 0x19
    
    OT_MSG_SOLAR_STORAGE   = 29,   // 0x1D - Solar storage / Alternative modulation
    
    OT_MSG_DHW_BOUNDS      = 48,   // 0x30 - DHW Setpoint boundaries
    OT_MSG_DHW_SETPOINT    = 56,   // 0x38 - DHW Setpoint
    OT_MSG_DHW_COMFORT     = 51,   // 0x33 - DHW Comfort parameters
    OT_MSG_DHW_FLOW_RATE   = 62,   // 0x3E - DHW Flow Rate
    
    // OEM / Diagnostics
    OT_MSG_OEM_DIAGNOSTIC  = 115   // 0x73 - OEM Diagnostic code
};