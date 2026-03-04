import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_PORT
from esphome.components.esp32 import include_builtin_idf_component

CODEOWNERS = ["@rar"]
DEPENDENCIES = ["smartgen_hsc941", "network", "esp32"]

CONF_SMARTGEN_ID = "smartgen_hsc941_id"
CONF_CSS_URL = "css_url"
CONF_JS_URL = "js_url"
CONF_AMBIENT_TEMP_ID = "ambient_temp_id"
CONF_AMBIENT_TEMP_NAME = "ambient_temp_name"
CONF_AMBIENT_HUMIDITY_ID = "ambient_humidity_id"
CONF_AMBIENT_HUMIDITY_NAME = "ambient_humidity_name"
CONF_SINGLE_PHASE = "single_phase"
CONF_TANK_SIZE = "tank_size_liters"
CONF_BURN_RATE = "burn_rate_lph"
CONF_LANGUAGE = "language"
CONF_MAINS_SENSOR_ID = "mains_sensor_id"
CONF_FUEL_LEVEL_ID = "fuel_level_id"
CONF_BUZZER_PIN = "buzzer_pin"
CONF_PIN_CODE = "pin_code"
CONF_RELAY_IDS = [f"relay_{i}_id" for i in range(1, 9)]
CONF_RELAY_NAMES = [f"relay_{i}_name" for i in range(1, 9)]

smartgen_hsc941_web_ns = cg.esphome_ns.namespace("smartgen_hsc941_web")
SmartgenHSC941Web = smartgen_hsc941_web_ns.class_("SmartgenHSC941Web", cg.Component)

# Import parent component and ESPHome base types
from ..smartgen_hsc941 import SmartgenHSC941

sensor_ns = cg.esphome_ns.namespace("sensor")
Sensor = sensor_ns.class_("Sensor")
switch_ns = cg.esphome_ns.namespace("switch_")
Switch = switch_ns.class_("Switch")
binary_sensor_ns = cg.esphome_ns.namespace("binary_sensor")
BinarySensor = binary_sensor_ns.class_("BinarySensor")

schema = {
    cv.GenerateID(): cv.declare_id(SmartgenHSC941Web),
    cv.Required(CONF_SMARTGEN_ID): cv.use_id(SmartgenHSC941),
    cv.Optional(CONF_PORT, default=8080): cv.port,
    cv.Optional(CONF_CSS_URL, default=""): cv.string,
    cv.Optional(CONF_JS_URL, default=""): cv.string,
    cv.Optional(CONF_SINGLE_PHASE, default=False): cv.boolean,
    cv.Optional(CONF_TANK_SIZE, default=0): cv.float_,
    cv.Optional(CONF_BURN_RATE, default=0): cv.float_,
    cv.Optional(CONF_LANGUAGE, default="en"): cv.string,
    cv.Optional(CONF_MAINS_SENSOR_ID): cv.use_id(BinarySensor),
    cv.Optional(CONF_FUEL_LEVEL_ID): cv.use_id(Sensor),
    cv.Optional(CONF_BUZZER_PIN): cv.int_range(min=0, max=48),
    cv.Optional(CONF_PIN_CODE, default=""): cv.string,
    cv.Optional(CONF_AMBIENT_TEMP_ID): cv.use_id(Sensor),
    cv.Optional(CONF_AMBIENT_TEMP_NAME, default="Ambient Temp"): cv.string,
    cv.Optional(CONF_AMBIENT_HUMIDITY_ID): cv.use_id(Sensor),
    cv.Optional(CONF_AMBIENT_HUMIDITY_NAME, default="Humidity"): cv.string,
}

# Add relay_1_id .. relay_8_id and relay_1_name .. relay_8_name
for i in range(8):
    schema[cv.Optional(CONF_RELAY_IDS[i])] = cv.use_id(Switch)
    schema[cv.Optional(CONF_RELAY_NAMES[i], default=f"Relay {i + 1}")] = cv.string

CONFIG_SCHEMA = cv.Schema(schema).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    # Re-enable SPIFFS IDF component (excluded by default in ESPHome)
    include_builtin_idf_component("spiffs")

    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    parent = await cg.get_variable(config[CONF_SMARTGEN_ID])
    cg.add(var.set_controller(parent))
    cg.add(var.set_port(config[CONF_PORT]))

    if config[CONF_CSS_URL]:
        cg.add(var.set_css_url(config[CONF_CSS_URL]))
    if config[CONF_JS_URL]:
        cg.add(var.set_js_url(config[CONF_JS_URL]))

    if config[CONF_SINGLE_PHASE]:
        cg.add(var.set_single_phase(True))

    if config[CONF_TANK_SIZE] > 0:
        cg.add(var.set_tank_size(config[CONF_TANK_SIZE]))
    if config[CONF_BURN_RATE] > 0:
        cg.add(var.set_burn_rate(config[CONF_BURN_RATE]))
    if config[CONF_LANGUAGE] != "en":
        cg.add(var.set_language(config[CONF_LANGUAGE]))

    if CONF_MAINS_SENSOR_ID in config:
        mains = await cg.get_variable(config[CONF_MAINS_SENSOR_ID])
        cg.add(var.set_mains_sensor(mains))

    if CONF_FUEL_LEVEL_ID in config:
        fuel_sens = await cg.get_variable(config[CONF_FUEL_LEVEL_ID])
        cg.add(var.set_fuel_level_sensor(fuel_sens))

    if CONF_BUZZER_PIN in config:
        cg.add(var.set_buzzer_pin(config[CONF_BUZZER_PIN]))

    if config[CONF_PIN_CODE]:
        cg.add(var.set_pin_code(config[CONF_PIN_CODE]))

    if CONF_AMBIENT_TEMP_ID in config:
        sens = await cg.get_variable(config[CONF_AMBIENT_TEMP_ID])
        cg.add(var.set_ambient_temp_sensor(sens))
    cg.add(var.set_ambient_temp_name(config[CONF_AMBIENT_TEMP_NAME]))

    if CONF_AMBIENT_HUMIDITY_ID in config:
        hum = await cg.get_variable(config[CONF_AMBIENT_HUMIDITY_ID])
        cg.add(var.set_ambient_humidity_sensor(hum))
    cg.add(var.set_ambient_humidity_name(config[CONF_AMBIENT_HUMIDITY_NAME]))

    for i in range(8):
        if CONF_RELAY_IDS[i] in config:
            sw = await cg.get_variable(config[CONF_RELAY_IDS[i]])
            cg.add(var.set_relay(i, sw, config[CONF_RELAY_NAMES[i]]))
