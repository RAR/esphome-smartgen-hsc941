import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import button
from esphome.const import (
    CONF_ID,
    ENTITY_CATEGORY_CONFIG,
)

from . import SmartgenHSC941, smartgen_hsc941_ns

DEPENDENCIES = ["smartgen_hsc941"]

CONF_SMARTGEN_ID = "smartgen_hsc941_id"

# Button types for remote control (Function Code 05H)
CONF_ENGINE_START = "engine_start"
CONF_ENGINE_STOP = "engine_stop"
CONF_ENGINE_AUTO = "engine_auto"
CONF_ENGINE_MANUAL = "engine_manual"
CONF_GEN_SWITCH_OFF = "gen_switch_off"
CONF_GEN_SWITCH_ON = "gen_switch_on"
CONF_FAULT_RESET = "fault_reset"

SmartgenButton = smartgen_hsc941_ns.class_("SmartgenButton", button.Button, cg.Component)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_SMARTGEN_ID): cv.use_id(SmartgenHSC941),
        cv.Optional(CONF_ENGINE_START): button.button_schema(
            SmartgenButton,
            icon="mdi:engine",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_ENGINE_STOP): button.button_schema(
            SmartgenButton,
            icon="mdi:engine-off",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_ENGINE_AUTO): button.button_schema(
            SmartgenButton,
            icon="mdi:auto-fix",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_ENGINE_MANUAL): button.button_schema(
            SmartgenButton,
            icon="mdi:hand-back-left",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_GEN_SWITCH_OFF): button.button_schema(
            SmartgenButton,
            icon="mdi:power-plug-off",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_GEN_SWITCH_ON): button.button_schema(
            SmartgenButton,
            icon="mdi:power-plug",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
        cv.Optional(CONF_FAULT_RESET): button.button_schema(
            SmartgenButton,
            icon="mdi:restart-alert",
            entity_category=ENTITY_CATEGORY_CONFIG,
        ),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_SMARTGEN_ID])

    button_map = {
        CONF_ENGINE_START: ("set_engine_start_button", 0),
        CONF_ENGINE_STOP: ("set_engine_stop_button", 1),
        CONF_ENGINE_AUTO: ("set_engine_auto_button", 3),
        CONF_ENGINE_MANUAL: ("set_engine_manual_button", 4),
        CONF_GEN_SWITCH_OFF: ("set_gen_switch_off_button", 5),
        CONF_GEN_SWITCH_ON: ("set_gen_switch_on_button", 6),
        CONF_FAULT_RESET: ("set_fault_reset_button", 7),
    }

    for conf_key, (setter, coil_addr) in button_map.items():
        if conf_key in config:
            btn = await button.new_button(config[conf_key])
            cg.add(btn.set_parent(parent))
            cg.add(btn.set_coil_address(coil_addr))
            cg.add(getattr(parent, setter)(btn))
