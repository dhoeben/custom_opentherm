import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import sensor, number, switch, climate
from esphome.const import CONF_ID

# ------------------------------------------------------------------------------
# Namespace + C++ class
# ------------------------------------------------------------------------------
custom_ot_ns = cg.esphome_ns.namespace("custom_opentherm")
CustomOpenThermComponent = custom_ot_ns.class_("CustomOpenThermComponent", cg.Component)

# ------------------------------------------------------------------------------
# Config keys
# ------------------------------------------------------------------------------
CONF_IN_PIN = "in_pin"
CONF_OUT_PIN = "out_pin"
CONF_POLL_INTERVAL = "poll_interval"
CONF_RX_TIMEOUT = "rx_timeout"
CONF_DEBUG = "debug"

CONF_BOILER_TEMP = "boiler_temp"
CONF_RETURN_TEMP = "return_temp"
CONF_MODULATION = "modulation"
CONF_SETPOINT = "setpoint"

# ------------------------------------------------------------------------------
# Full CONFIG_SCHEMA
# ------------------------------------------------------------------------------
CONFIG_SCHEMA = cv.Schema({
    cv.GenerateID(): cv.declare_id(CustomOpenThermComponent),

    cv.Required(CONF_IN_PIN): pins.gpio_input_pin_schema,
    cv.Required(CONF_OUT_PIN): pins.gpio_output_pin_schema,

    cv.Optional(CONF_POLL_INTERVAL, default="10s"): cv.positive_time_period_milliseconds,
    cv.Optional(CONF_RX_TIMEOUT, default="40ms"): cv.positive_time_period_milliseconds,
    cv.Optional(CONF_DEBUG, default=False): cv.boolean,

    # Sensor bindings
    cv.Optional(CONF_BOILER_TEMP):
        sensor.sensor_schema(unit_of_measurement="°C", accuracy_decimals=1, entity_category="diagnostic"),

    cv.Optional(CONF_RETURN_TEMP):
        sensor.sensor_schema(unit_of_measurement="°C", accuracy_decimals=1, entity_category="diagnostic"),

    cv.Optional(CONF_MODULATION):
        sensor.sensor_schema(unit_of_measurement="%", accuracy_decimals=0, entity_category="diagnostic"),

    cv.Optional(CONF_SETPOINT):
        sensor.sensor_schema(unit_of_measurement="°C", accuracy_decimals=1, entity_category="diagnostic"),

    # Limits
    cv.Optional("max_boiler_temp_heating"): number.number_schema,
    cv.Optional("max_boiler_temp_water"): number.number_schema,

    # Equitherm tuning numbers
    cv.Optional("eq_fb_gain"): number.number_schema,
    cv.Optional("eq_k"): number.number_schema,
    cv.Optional("eq_n"): number.number_schema,
    cv.Optional("eq_t"): number.number_schema,

    # Optional linked entities
    cv.Optional("ch_climate"): climate.CLIMATE_SCHEMA,
    cv.Optional("emergency_mode"): switch.SWITCH_SCHEMA,
    cv.Optional("force_heat"): switch.SWITCH_SCHEMA,
    cv.Optional("force_dhw"): switch.SWITCH_SCHEMA,

}).extend(cv.COMPONENT_SCHEMA)

# ------------------------------------------------------------------------------
# Codegen
# ------------------------------------------------------------------------------
async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    # Pins
    in_pin = await cg.gpio_pin_expression(config[CONF_IN_PIN])
    out_pin = await cg.gpio_pin_expression(config[CONF_OUT_PIN])
    cg.add(var.set_pins(in_pin, out_pin))

    # Basic config
    cg.add(var.set_poll_interval(config[CONF_POLL_INTERVAL]))
    cg.add(var.set_rx_timeout(config[CONF_RX_TIMEOUT]))
    cg.add(var.set_debug(config[CONF_DEBUG]))

    # Sensor bindings
    if CONF_BOILER_TEMP in config:
        sens = await sensor.new_sensor(config[CONF_BOILER_TEMP])
        cg.add(var.set_boiler_temp_sensor(sens))

    if CONF_RETURN_TEMP in config:
        sens = await sensor.new_sensor(config[CONF_RETURN_TEMP])
        cg.add(var.set_return_temp_sensor(sens))

    if CONF_MODULATION in config:
        sens = await sensor.new_sensor(config[CONF_MODULATION])
        cg.add(var.set_modulation_sensor(sens))

    if CONF_SETPOINT in config:
        sens = await sensor.new_sensor(config[CONF_SETPOINT])
        cg.add(var.set_setpoint_sensor(sens))

    # Limits
    if "max_boiler_temp_heating" in config:
        num = await number.new_number(config["max_boiler_temp_heating"])
        cg.add(var.set_boiler_limit_number(num))

    if "max_boiler_temp_water" in config:
        num = await number.new_number(config["max_boiler_temp_water"])
        cg.add(var.set_dhw_limit_number(num))

    # Equitherm tuning numbers
    for key in ["eq_fb_gain", "eq_k", "eq_n", "eq_t"]:
        if key in config:
            num = await number.new_number(config[key])
            cg.add(getattr(var, f"set_{key}_number")(num))

    # Optional climate entity
    if "ch_climate" in config:
        climate_entity = await climate.new_climate(config["ch_climate"])
        cg.add(var.set_climate_entity(climate_entity))

    # Optional switches
    if "emergency_mode" in config:
        sw = await switch.new_switch(config["emergency_mode"])
        cg.add(var.set_emergency_switch(sw))

    if "force_heat" in config:
        sw = await switch.new_switch(config["force_heat"])
        cg.add(var.set_force_heat_switch(sw))

    if "force_dhw" in config:
        sw = await switch.new_switch(config["force_dhw"])
        cg.add(var.set_force_dhw_switch(sw))


# ------------------------------------------------------------------------------
# Register YAML key → Component mapping
# ------------------------------------------------------------------------------
DEPENDENCIES = []
AUTO_LOAD = ["sensor", "number", "switch", "climate"]

def _register():
    from esphome import yaml_util
    yaml_util.add_component_name("custom_opentherm", CustomOpenThermComponent)

_register()
