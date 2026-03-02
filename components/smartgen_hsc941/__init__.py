import esphome.codegen as cg
import esphome.config_validation as cv
from esphome import pins
from esphome.const import (
    CONF_ID,
    CONF_ADDRESS,
    CONF_BAUD_RATE,
    CONF_UPDATE_INTERVAL,
    CONF_RX_PIN,
    CONF_TX_PIN,
)

CODEOWNERS = ["@rar"]
DEPENDENCIES = []
AUTO_LOAD = ["sensor", "binary_sensor", "button"]
MULTI_CONF = True

CONF_FLOW_CONTROL_PIN = "flow_control_pin"
CONF_UART_NUM = "uart_num"

smartgen_hsc941_ns = cg.esphome_ns.namespace("smartgen_hsc941")
SmartgenHSC941 = smartgen_hsc941_ns.class_("SmartgenHSC941", cg.PollingComponent)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(SmartgenHSC941),
        cv.Optional(CONF_ADDRESS, default=1): cv.int_range(min=1, max=247),
        cv.Optional(CONF_BAUD_RATE, default=9600): cv.int_range(min=1200, max=115200),
        cv.Required(CONF_TX_PIN): pins.internal_gpio_output_pin_number,
        cv.Required(CONF_RX_PIN): pins.internal_gpio_input_pin_number,
        cv.Optional(CONF_FLOW_CONTROL_PIN): pins.internal_gpio_output_pin_number,
        cv.Optional(CONF_UART_NUM, default=1): cv.int_range(min=0, max=2),
        cv.Optional(CONF_UPDATE_INTERVAL, default="5s"): cv.update_interval,
    }
).extend(cv.COMPONENT_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_address(config[CONF_ADDRESS]))
    cg.add(var.set_baud_rate(config[CONF_BAUD_RATE]))
    cg.add(var.set_tx_pin(config[CONF_TX_PIN]))
    cg.add(var.set_rx_pin(config[CONF_RX_PIN]))
    cg.add(var.set_uart_num(config[CONF_UART_NUM]))

    if CONF_FLOW_CONTROL_PIN in config:
        cg.add(var.set_flow_control_pin(config[CONF_FLOW_CONTROL_PIN]))
