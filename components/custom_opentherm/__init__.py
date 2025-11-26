import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.components import sensor, number, switch, climate
from esphome.const import CONF_ID

DEPENDENCIES = []
AUTO_LOAD = ["sensor", "number", "switch", "climate"]

# ------------------------------------------------------------------------------
# C++ namespace + class binding
# ------------------------------------------------------------------------------

# Bind naar de bestaande C++ class opentherm::OpenThermComponent
ot_ns = cg.esphome_ns.namespace("opentherm")
OpenThermComponent = ot_ns.class_("OpenThermComponent", cg.Component)

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
# CONFIG_SCHEMA
# ------------------------------------------------------------------------------

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(OpenThermComponent),

        cv.Required(CONF_IN_PIN): pins.gpio_input_pin_schema,
        cv.Required(CONF_OUT_PIN): pins.gpio_output_pin_schema,

        cv.Optional(CONF_POLL_INTERVAL, default="10s"): cv.positive_time_period_milliseconds,
        cv.Optional(CONF_RX_TIMEOUT, default="40ms"): cv.positive_time_period_milliseconds,
        cv.Optional(CONF_DEBUG, default=False): cv.boolean,

        # Koppeling met bestaande sensoren (uit sensors.yaml) via id:
        cv.Optional(CONF_BOILER_TEMP): cv.use_id(sensor.Sensor),
        cv.Optional(CONF_RETURN_TEMP): cv.use_id(sensor.Sensor),
        cv.Optional(CONF_MODULATION): cv.use_id(sensor.Sensor),
        cv.Optional(CONF_SETPOINT): cv.use_id(sensor.Sensor),

        # Bestaande number entities voor limieten
        cv.Optional("max_boiler_temp_heating"): cv.use_id(number.Number),
        cv.Optional("max_boiler_temp_water"): cv.use_id(number.Number),

        # Equitherm tuning
        cv.Optional("eq_fb_gain"): cv.use_id(number.Number),
        cv.Optional("eq_k"): cv.use_id(number.Number),
        cv.Optional("eq_n"): cv.use_id(number.Number),
        cv.Optional("eq_t"): cv.use_id(number.Number),

        # Optionele gekoppelde entiteiten
        cv.Optional("ch_climate"): cv.use_id(climate.Climate),
        cv.Optional("emergency_mode"): cv.use_id(switch.Switch),
        cv.Optional("force_heat"): cv.use_id(switch.Switch),
        cv.Optional("force_dhw"): cv.use_id(switch.Switch),
    }
).extend(cv.COMPONENT_SCHEMA)


# ------------------------------------------------------------------------------
# to_code
# ------------------------------------------------------------------------------

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    # Pins
    in_pin = await cg.gpio_pin_expression(config[CONF_IN_PIN])
    out_pin = await cg.gpio_pin_expression(config[CONF_OUT_PIN])
    cg.add(var.set_pins(in_pin, out_pin))

    # Basisconfig
    cg.add(var.set_poll_interval(config[CONF_POLL_INTERVAL]))
    cg.add(var.set_rx_timeout(config[CONF_RX_TIMEOUT]))
    cg.add(var.set_debug(config[CONF_DEBUG]))

    # Sensor binding (naar bestaande sensoren met id in sensors.yaml)
    if CONF_BOILER_TEMP in config:
        sens = await cg.get_variable(config[CONF_BOILER_TEMP])
        cg.add(var.set_boiler_temp_sensor(sens))

    if CONF_RETURN_TEMP in config:
        sens = await cg.get_variable(config[CONF_RETURN_TEMP])
        cg.add(var.set_return_temp_sensor(sens))

    if CONF_MODULATION in config:
        sens = await cg.get_variable(config[CONF_MODULATION])
        cg.add(var.set_modulation_sensor(sens))

    if CONF_SETPOINT in config:
        sens = await cg.get_variable(config[CONF_SETPOINT])
        cg.add(var.set_setpoint_sensor(sens))

    # Limit numbers
    if "max_boiler_temp_heating" in config:
        num = await cg.get_variable(config["max_boiler_temp_heating"])
        cg.add(var.set_boiler_limit_number(num))

    if "max_boiler_temp_water" in config:
        num = await cg.get_variable(config["max_boiler_temp_water"])
        cg.add(var.set_dhw_limit_number(num))

    # Equitherm tuning numbers
    for key, setter in [
        ("eq_fb_gain", "set_eq_fb_gain_number"),
        ("eq_k", "set_eq_k_number"),
        ("eq_n", "set_eq_n_number"),
        ("eq_t", "set_eq_t_number"),
    ]:
        if key in config:
            num = await cg.get_variable(config[key])
            cg.add(getattr(var, setter)(num))

    # Optionele climate entity
    if "ch_climate" in config:
        climate_entity = await cg.get_variable(config["ch_climate"])
        cg.add(var.set_climate_entity(climate_entity))

    # Optionele switches
    if "emergency_mode" in config:
        sw = await cg.get_variable(config["emergency_mode"])
        cg.add(var.set_emergency_switch(sw))

    if "force_heat" in config:
        sw = await cg.get_variable(config["force_heat"])
        cg.add(var.set_force_heat_switch(sw))

    if "force_dhw" in config:
        sw = await cg.get_variable(config["force_dhw"])
        cg.add(var.set_force_dhw_switch(sw))
