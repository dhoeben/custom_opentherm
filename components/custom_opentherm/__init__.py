import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import sensor, number, switch, climate
from esphome.const import CONF_ID

AUTO_LOAD = ["climate", "number", "sensor", "switch"]

ot_ns = cg.esphome_ns.namespace("opentherm")
OpenThermComponent = ot_ns.class_("OpenThermComponent", cg.Component)

CONF_IN_PIN = "in_pin"
CONF_OUT_PIN = "out_pin"
CONF_POLL_INTERVAL = "poll_interval"
CONF_RX_TIMEOUT = "rx_timeout"
CONF_DEBUG = "debug"

CONF_BOILER_TEMP = "boiler_temp"
CONF_RETURN_TEMP = "return_temp"
CONF_MODULATION = "modulation"
CONF_SETPOINT = "setpoint"

CONF_MAX_BOILER_TEMP_HEATING = "max_boiler_temp_heating"
CONF_MAX_BOILER_TEMP_WATER = "max_boiler_temp_water"

CONF_EQ_FB_GAIN = "eq_fb_gain"
CONF_EQ_K = "eq_k"
CONF_EQ_N = "eq_n"
CONF_EQ_T = "eq_t"

CONF_CH_CLIMATE = "ch_climate"
CONF_EMERGENCY_MODE = "emergency_mode"
CONF_FORCE_HEAT = "force_heat"
CONF_FORCE_DHW = "force_dhw"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(OpenThermComponent),
        cv.Required(CONF_IN_PIN): pins.gpio_input_pin_schema,
        cv.Required(CONF_OUT_PIN): pins.gpio_output_pin_schema,
        cv.Optional(
            CONF_POLL_INTERVAL, default="10s"
        ): cv.positive_time_period_milliseconds,
        cv.Optional(
            CONF_RX_TIMEOUT, default="40ms"
        ): cv.positive_time_period_milliseconds,
        cv.Optional(CONF_DEBUG, default=False): cv.boolean,
        cv.Optional(CONF_BOILER_TEMP): cv.use_id(sensor.Sensor),
        cv.Optional(CONF_RETURN_TEMP): cv.use_id(sensor.Sensor),
        cv.Optional(CONF_MODULATION): cv.use_id(sensor.Sensor),
        cv.Optional(CONF_SETPOINT): cv.use_id(sensor.Sensor),
        cv.Optional(CONF_MAX_BOILER_TEMP_HEATING): cv.use_id(number.Number),
        cv.Optional(CONF_MAX_BOILER_TEMP_WATER): cv.use_id(number.Number),
        cv.Optional(CONF_EQ_FB_GAIN): cv.use_id(number.Number),
        cv.Optional(CONF_EQ_K): cv.use_id(number.Number),
        cv.Optional(CONF_EQ_N): cv.use_id(number.Number),
        cv.Optional(CONF_EQ_T): cv.use_id(number.Number),
        cv.Optional(CONF_CH_CLIMATE): cv.use_id(climate.Climate),
        cv.Optional(CONF_EMERGENCY_MODE): cv.use_id(switch.Switch),
        cv.Optional(CONF_FORCE_HEAT): cv.use_id(switch.Switch),
        cv.Optional(CONF_FORCE_DHW): cv.use_id(switch.Switch),
    }
).extend(cv.COMPONENT_SCHEMA)


def _final_validate(config):
    eq_keys = {
        CONF_EQ_FB_GAIN,
        CONF_EQ_K,
        CONF_EQ_N,
        CONF_EQ_T,
    }

    if eq_keys.intersection(config) and CONF_CH_CLIMATE not in config:
        raise cv.Invalid("eq_* parameters require 'ch_climate' to be configured")

    if CONF_FORCE_HEAT in config and CONF_FORCE_DHW in config:
        raise cv.Invalid(
            "'force_heat' and 'force_dhw' cannot be enabled simultaneously"
        )

    return config


FINAL_VALIDATE_SCHEMA = _final_validate


async def _set_optional(var, config, key, setter):
    if key in config:
        obj = await cg.get_variable(config[key])
        cg.add(getattr(var, setter)(obj))


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    in_pin = await cg.gpio_pin_expression(config[CONF_IN_PIN])
    out_pin = await cg.gpio_pin_expression(config[CONF_OUT_PIN])
    cg.add(var.set_pins(in_pin, out_pin))

    cg.add(var.set_poll_interval(config[CONF_POLL_INTERVAL]))
    cg.add(var.set_rx_timeout(config[CONF_RX_TIMEOUT]))
    cg.add(var.set_debug(config[CONF_DEBUG]))

    await _set_optional(var, config, CONF_BOILER_TEMP, "set_boiler_temp_sensor")
    await _set_optional(var, config, CONF_RETURN_TEMP, "set_return_temp_sensor")
    await _set_optional(var, config, CONF_MODULATION, "set_modulation_sensor")
    await _set_optional(var, config, CONF_SETPOINT, "set_setpoint_sensor")

    await _set_optional(
        var, config, CONF_MAX_BOILER_TEMP_HEATING, "set_boiler_limit_number"
    )
    await _set_optional(var, config, CONF_MAX_BOILER_TEMP_WATER, "set_dhw_limit_number")

    for key, setter in (
        (CONF_EQ_FB_GAIN, "set_eq_fb_gain_number"),
        (CONF_EQ_K, "set_eq_k_number"),
        (CONF_EQ_N, "set_eq_n_number"),
        (CONF_EQ_T, "set_eq_t_number"),
    ):
        await _set_optional(var, config, key, setter)

    await _set_optional(var, config, CONF_CH_CLIMATE, "set_climate_entity")
    await _set_optional(var, config, CONF_EMERGENCY_MODE, "set_emergency_switch")
    await _set_optional(var, config, CONF_FORCE_HEAT, "set_force_heat_switch")
    await _set_optional(var, config, CONF_FORCE_DHW, "set_force_dhw_switch")
