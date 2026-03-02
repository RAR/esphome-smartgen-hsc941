import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_PORT

CODEOWNERS = ["@rar"]
DEPENDENCIES = ["smartgen_hsc941", "network"]

CONF_SMARTGEN_ID = "smartgen_hsc941_id"
CONF_CSS_URL = "css_url"
CONF_JS_URL = "js_url"

smartgen_hsc941_web_ns = cg.esphome_ns.namespace("smartgen_hsc941_web")
SmartgenHSC941Web = smartgen_hsc941_web_ns.class_("SmartgenHSC941Web", cg.Component)

# Import parent component
from ..smartgen_hsc941 import SmartgenHSC941

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(SmartgenHSC941Web),
        cv.Required(CONF_SMARTGEN_ID): cv.use_id(SmartgenHSC941),
        cv.Optional(CONF_PORT, default=8080): cv.port,
        cv.Optional(CONF_CSS_URL, default=""): cv.string,
        cv.Optional(CONF_JS_URL, default=""): cv.string,
    }
).extend(cv.COMPONENT_SCHEMA)


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
