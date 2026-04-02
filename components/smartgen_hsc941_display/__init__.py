import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
import os

CODEOWNERS = ["@rar"]
DEPENDENCIES = ["smartgen_hsc941", "lvgl"]

CONF_SMARTGEN_ID = "smartgen_hsc941_id"
CONF_WEB_ID = "smartgen_hsc941_web_id"
CONF_FUEL_SENSOR_ID = "fuel_level_id"

from esphome.components.smartgen_hsc941 import SmartgenHSC941
from esphome.components.smartgen_hsc941_web import SmartgenHSC941Web
from esphome.components import sensor

smartgen_hsc941_display_ns = cg.esphome_ns.namespace("smartgen_hsc941_display")
SmartgenHSC941Display = smartgen_hsc941_display_ns.class_(
    "SmartgenHSC941Display", cg.Component
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(SmartgenHSC941Display),
        cv.Required(CONF_SMARTGEN_ID): cv.use_id(SmartgenHSC941),
        cv.Optional(CONF_WEB_ID): cv.use_id(SmartgenHSC941Web),
        cv.Optional(CONF_FUEL_SENSOR_ID): cv.use_id(sensor.Sensor),
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    hub = await cg.get_variable(config[CONF_SMARTGEN_ID])
    cg.add(var.set_hub(hub))

    if CONF_WEB_ID in config:
        web = await cg.get_variable(config[CONF_WEB_ID])
        cg.add(var.set_web(web))

    if CONF_FUEL_SENSOR_ID in config:
        fuel = await cg.get_variable(config[CONF_FUEL_SENSOR_ID])
        cg.add(var.set_fuel_sensor(fuel))

    # Register fonts used by EEZ UI so ESPHome enables them in lv_conf.h
    from esphome.components.lvgl.helpers import lv_fonts_used
    lv_fonts_used.add("montserrat_16")
    lv_fonts_used.add("montserrat_18")
    lv_fonts_used.add("montserrat_40")

    # Enable LVGL tabview widget (extra widget, not in core)
    cg.add_define("LV_USE_TABVIEW", 1)
