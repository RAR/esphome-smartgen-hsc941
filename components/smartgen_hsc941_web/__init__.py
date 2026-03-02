import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_PORT

CODEOWNERS = ["@rar"]
DEPENDENCIES = ["smartgen_hsc941", "network"]

CONF_SMARTGEN_ID = "smartgen_hsc941_id"
CONF_CSS_URL = "css_url"
CONF_JS_URL = "js_url"
CONF_AMBIENT_TEMP_ID = "ambient_temp_id"
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

schema = {
    cv.GenerateID(): cv.declare_id(SmartgenHSC941Web),
    cv.Required(CONF_SMARTGEN_ID): cv.use_id(SmartgenHSC941),
    cv.Optional(CONF_PORT, default=8080): cv.port,
    cv.Optional(CONF_CSS_URL, default=""): cv.string,
    cv.Optional(CONF_JS_URL, default=""): cv.string,
    cv.Optional(CONF_AMBIENT_TEMP_ID): cv.use_id(Sensor),
}

# Add relay_1_id .. relay_8_id and relay_1_name .. relay_8_name
for i in range(8):
    schema[cv.Optional(CONF_RELAY_IDS[i])] = cv.use_id(Switch)
    schema[cv.Optional(CONF_RELAY_NAMES[i], default=f"Relay {i + 1}")] = cv.string

CONFIG_SCHEMA = cv.Schema(schema).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    parent = await cg.get_variable(config[CONF_SMARTGEN_ID])
    cg.add(var.set_controller(parent))
    cg.add(var.set_port(config[CONF_PORT]))

    if config[CONF_CSS_URL]:
        cg.add(var.set_css_url(config[CONF_CSS_URL]))
    if config[CONF_JS_URL]:
        cg.add(var.set_js_url(config[CONF_JS_URL]))

    if CONF_AMBIENT_TEMP_ID in config:
        sens = await cg.get_variable(config[CONF_AMBIENT_TEMP_ID])
        cg.add(var.set_ambient_temp_sensor(sens))

    for i in range(8):
        if CONF_RELAY_IDS[i] in config:
            sw = await cg.get_variable(config[CONF_RELAY_IDS[i]])
            cg.add(var.set_relay(i, sw, config[CONF_RELAY_NAMES[i]]))
