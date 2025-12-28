#pragma once

#ifndef OT_IN_PIN
#define OT_IN_PIN GPIO_NUM_18
#endif

#ifndef OT_OUT_PIN
#define OT_OUT_PIN GPIO_NUM_17
#endif

#ifndef TEMP_PIN
#define TEMP_PIN GPIO_NUM_4
#endif

#ifndef LIGHT_PIN
#define LIGHT_PIN GPIO_NUM_48
#endif

#ifndef OT_POLL_INTERVAL
#define OT_POLL_INTERVAL 10000
#endif

#ifndef OT_RX_TIMEOUT
#define OT_RX_TIMEOUT 40
#endif

#define ENABLE_DHW_MODULE 1
#define ENABLE_EQUITHERM_MODULE 1
#define ENABLE_DIAGNOSTICS_MODULE 1
#define ENABLE_EMERGENCY_MODULE 1
#define ENABLE_LED_STATUS 1

#ifndef OT_DEBUG
#define OT_DEBUG 1
#endif

#define OT_LOG_TAG "opentherm"

#define DEFAULT_MAX_HEATING_TEMP 70.0f
#define DEFAULT_MAX_DHW_TEMP 60.0f
#define DEFAULT_MIN_FLOW_TEMP 30.0f
#define DEFAULT_BOOST_TEMP 20.0f
#define DEFAULT_ROOM_TEMP_FALLBACK 21.0f

#define COMM_TIMEOUT_MS 30000
#define MIN_DHW_TEMP 30.0f

#define OT_FW_VERSION "0.4.0"
