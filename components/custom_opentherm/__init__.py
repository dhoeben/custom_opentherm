import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins

from esphome.components import (
    sensor,
    binary_sensor,
    text_sensor,
    number,
    switch,
    climate,
)

from esphome.const import CONF_ID

DEPENDENCIES = []
AUTO_LOAD = ["sensor", "binary_sensor", "text_sensor", "number", "switch", "climate"]

CONF_IN_PIN = "in_pin"
CONF_OUT_PIN = "out_pin"
CONF_POLL_INTERVAL = "poll_interval"
CONF_RX_TIMEOUT = "rx_timeout"
CONF_DEBUG = "debug"

custom_opentherm_ns = cg.esphome_ns.namespace("custom_opentherm")
OpenThermComponent = custom_opentherm_ns.class_("OpenThermComponent", cg.Component)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(OpenThermComponent),

            cv.Required(CONF_IN_PIN): pins.gpio_input_pin_schema,
            cv.Required(CONF_OUT_PIN): pins.gpio_output_pin_schema,

            cv.Optional(CONF_POLL_INTERVAL, default="10s"): cv.positive_time_period_milliseconds,
            cv.Optional(CONF_RX_TIMEOUT, default="40ms"): cv.positive_time_period_milliseconds,
            cv.Optional(CONF_DEBUG, default=False): cv.boolean,

            cv.Optional("ch_climate"): cv.use_id(climate.Climate),

            cv.Optional("emergency_switch"): cv.use_id(switch.Switch),
            cv.Optional("force_heat_switch"): cv.use_id(switch.Switch),
            cv.Optional("force_dhw_switch"): cv.use_id(switch.Switch),

            cv.Optional("max_boiler_temp_heating"): cv.use_id(number.Number),
            cv.Optional("max_boiler_temp_water"): cv.use_id(number.Number),

            cv.Optional("eq_fb_gain"): cv.use_id(number.Number),
            cv.Optional("eq_k"): cv.use_id(number.Number),
            cv.Optional("eq_n"): cv.use_id(number.Number),
            cv.Optional("eq_t"): cv.use_id(number.Number),

            # numeric sensors
            cv.Optional("boiler_temp"): cv.use_id(sensor.Sensor),
            cv.Optional("return_temp"): cv.use_id(sensor.Sensor),
            cv.Optional("modulation"): cv.use_id(sensor.Sensor),
            cv.Optional("pressure"): cv.use_id(sensor.Sensor),
            cv.Optional("dhw_temp"): cv.use_id(sensor.Sensor),
            cv.Optional("dhw_flow_rate"): cv.use_id(sensor.Sensor),
            cv.Optional("outside_temp"): cv.use_id(sensor.Sensor),
            cv.Optional("setpoint"): cv.use_id(sensor.Sensor),

            # binary sensors
            cv.Optional("fault"): cv.use_id(binary_sensor.BinarySensor),
            cv.Optional("service"): cv.use_id(binary_sensor.BinarySensor),
            cv.Optional("lockout"): cv.use_id(binary_sensor.BinarySensor),
            cv.Optional("ch_active"): cv.use_id(binary_sensor.BinarySensor),
            cv.Optional("dhw_active"): cv.use_id(binary_sensor.BinarySensor),
            cv.Optional("flame_on"): cv.use_id(binary_sensor.BinarySensor),
            cv.Optional("tap_flow"): cv.use_id(binary_sensor.BinarySensor),
            cv.Optional("dhw_preheat_active"): cv.use_id(binary_sensor.BinarySensor),

            # text sensors
            cv.Optional("mode"): cv.use_id(text_sensor.TextSensor),
            cv.Optional("status"): cv.use_id(text_sensor.TextSensor),
            cv.Optional("boiler_status"): cv.use_id(text_sensor.TextSensor),
            cv.Optional("diagnostic_text"): cv.use_id(text_sensor.TextSensor),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    in_pin = await cg.gpio_pin_expression(config[CONF_IN_PIN])
    out_pin = await cg.gpio_pin_expression(config[CONF_OUT_PIN])
    cg.add(var.set_pins(in_pin, out_pin))

    cg.add(var.set_poll_interval_ms(config[CONF_POLL_INTERVAL]))
    cg.add(var.set_rx_timeout_ms(config[CONF_RX_TIMEOUT]))
    cg.add(var.set_debug(config[CONF_DEBUG]))

    if "ch_climate" in config:
        c = await cg.get_variable(config["ch_climate"])
        cg.add(var.bind_ch_climate(c))

    for key, binder in [
        ("emergency_switch", "bind_emergency_switch"),
        ("force_heat_switch", "bind_force_heat_switch"),
        ("force_dhw_switch", "bind_force_dhw_switch"),
    ]:
        if key in config:
            sw = await cg.get_variable(config[key])
            cg.add(getattr(var, binder)(sw))

    for key, binder in [
        ("max_boiler_temp_heating", "bind_max_boiler_temp_heating_number"),
        ("max_boiler_temp_water", "bind_max_boiler_temp_water_number"),
        ("eq_fb_gain", "bind_eq_fb_gain_number"),
        ("eq_k", "bind_eq_k_number"),
        ("eq_n", "bind_eq_n_number"),
        ("eq_t", "bind_eq_t_number"),
    ]:
        if key in config:
            n = await cg.get_variable(config[key])
            cg.add(getattr(var, binder)(n))

    for key, binder in [
        ("boiler_temp", "bind_boiler_temp_sensor"),
        ("return_temp", "bind_return_temp_sensor"),
        ("modulation", "bind_modulation_sensor"),
        ("pressure", "bind_pressure_sensor"),
        ("dhw_temp", "bind_dhw_temp_sensor"),
        ("dhw_flow_rate", "bind_dhw_flow_rate_sensor"),
        ("outside_temp", "bind_outside_temp_sensor"),
        ("setpoint", "bind_setpoint_sensor"),
    ]:
        if key in config:
            s = await cg.get_variable(config[key])
            cg.add(getattr(var, binder)(s))

    for key, binder in [
        ("fault", "bind_fault_binary"),
        ("service", "bind_service_binary"),
        ("lockout", "bind_lockout_binary"),
        ("ch_active", "bind_ch_active_binary"),
        ("dhw_active", "bind_dhw_active_binary"),
        ("flame_on", "bind_flame_on_binary"),
        ("tap_flow", "bind_tap_flow_binary"),
        ("dhw_preheat_active", "bind_dhw_preheat_binary"),
    ]:
        if key in config:
            b = await cg.get_variable(config[key])
            cg.add(getattr(var, binder)(b))

    for key, binder in [
        ("mode", "bind_mode_text"),
        ("status", "bind_status_text"),
        ("boiler_status", "bind_boiler_status_text"),
        ("diagnostic_text", "bind_diagnostic_text"),
    ]:
        if key in config:
            t = await cg.get_variable(config[key])
            cg.add(getattr(var, binder)(t))
