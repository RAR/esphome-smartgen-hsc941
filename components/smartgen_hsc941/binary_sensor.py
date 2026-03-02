import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import binary_sensor
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_PROBLEM,
    DEVICE_CLASS_RUNNING,
    DEVICE_CLASS_SAFETY,
)

from . import SmartgenHSC941, smartgen_hsc941_ns

DEPENDENCIES = ["smartgen_hsc941"]

CONF_SMARTGEN_ID = "smartgen_hsc941_id"

# Alarms & Warnings (Function Code 01H - coil addresses)
CONF_COMMON_ALARM = "common_alarm"
CONF_COMMON_WARNING = "common_warning"
CONF_COMMON_SHUTDOWN = "common_shutdown"
CONF_GEN_ON_LOAD = "gen_on_load"
CONF_EMERGENCY_STOP = "emergency_stop"
CONF_OVERSPEED_SHUTDOWN = "overspeed_shutdown"
CONF_UNDERSPEED_SHUTDOWN = "underspeed_shutdown"
CONF_LOSS_SPEED_SIGNAL_SHUTDOWN = "loss_speed_signal_shutdown"
CONF_GEN_OVERFREQUENCY_SHUTDOWN = "gen_overfrequency_shutdown"
CONF_GEN_UNDERFREQUENCY_SHUTDOWN = "gen_underfrequency_shutdown"
CONF_GEN_OVERVOLTAGE_SHUTDOWN = "gen_overvoltage_shutdown"
CONF_GEN_UNDERVOLTAGE_SHUTDOWN = "gen_undervoltage_shutdown"
CONF_GEN_OVERCURRENT_SHUTDOWN = "gen_overcurrent_shutdown"
CONF_CRANK_FAILURE_SHUTDOWN = "crank_failure_shutdown"
CONF_HIGH_TEMP_SHUTDOWN_IN = "high_temp_shutdown_in"
CONF_LOW_OP_SHUTDOWN_IN = "low_oil_pressure_shutdown_in"
CONF_NO_GEN_SHUTDOWN = "no_gen_shutdown"
CONF_EXTERNAL_SHUTDOWN_INPUT = "external_shutdown_input"
CONF_LOW_FUEL_LEVEL_SHUTDOWN_IN = "low_fuel_level_shutdown_in"
CONF_LOW_COOLANT_LEVEL_SHUTDOWN_IN = "low_coolant_level_shutdown_in"
CONF_HIGH_TEMP_WARNING_IN = "high_temp_warning_in"
CONF_LOW_OP_WARNING_IN = "low_oil_pressure_warning_in"
CONF_STOP_FAILURE_WARNING = "stop_failure_warning"
CONF_LOW_FUEL_LEVEL_WARNING_IN = "low_fuel_level_warning_in"
CONF_CHARGING_FAILURE_WARNING = "charging_failure_warning"
CONF_BATTERY_UNDERVOLTAGE_WARNING = "battery_undervoltage_warning"
CONF_BATTERY_OVERVOLTAGE_WARNING = "battery_overvoltage_warning"
CONF_EXTERNAL_WARNING_INPUT = "external_warning_input"
CONF_LOSS_SPEED_SIGNAL_WARNING = "loss_speed_signal_warning"
CONF_LOW_COOLANT_LEVEL_WARNING_IN = "low_coolant_level_warning_in"
CONF_TEMP_SENSOR_OPEN_WARNING = "temp_sensor_open_warning"
CONF_OP_SENSOR_OPEN_WARNING = "oil_pressure_sensor_open_warning"
CONF_AUX_SENSOR_OPEN_WARNING = "aux_sensor_open_warning"
CONF_AUX_SENSOR_WARNING = "aux_sensor_warning"
CONF_AUX_SENSOR_SHUTDOWN = "aux_sensor_shutdown"
CONF_IN_AUTO_MODE = "in_auto_mode"
CONF_IN_MANUAL_MODE = "in_manual_mode"
CONF_IN_STOP_MODE = "in_stop_mode"
CONF_TEMP_SENSOR_OPEN_SHUTDOWN = "temp_sensor_open_shutdown"
CONF_OP_SENSOR_OPEN_SHUTDOWN = "oil_pressure_sensor_open_shutdown"
CONF_LOW_ENGINE_OIL_LEVEL_SHUTDOWN_IN = "low_engine_oil_level_shutdown_in"
CONF_AUX_SENSOR_OPEN_SHUTDOWN = "aux_sensor_open_shutdown"
CONF_EMERGENCY_STOP_INPUT = "emergency_stop_input"
CONF_AUX_INPUT_1 = "aux_input_1"
CONF_AUX_INPUT_2 = "aux_input_2"
CONF_AUX_INPUT_3 = "aux_input_3"
CONF_AUX_INPUT_4 = "aux_input_4"
CONF_AUX_INPUT_5 = "aux_input_5"
CONF_AUX_INPUT_6 = "aux_input_6"
CONF_START_RELAY_OUTPUT = "start_relay_output"
CONF_FUEL_RELAY_OUTPUT = "fuel_relay_output"
CONF_AUX_OUTPUT_1 = "aux_output_1"
CONF_AUX_OUTPUT_2 = "aux_output_2"
CONF_AUX_OUTPUT_3 = "aux_output_3"
CONF_AUX_OUTPUT_4 = "aux_output_4"
CONF_HIGH_TEMP_SHUTDOWN = "high_temp_shutdown"
CONF_LOW_OP_SHUTDOWN = "low_oil_pressure_shutdown"
CONF_HIGH_TEMP_WARNING = "high_temp_warning"
CONF_LOW_OP_WARNING = "low_oil_pressure_warning"


def bs_schema(device_class=DEVICE_CLASS_PROBLEM, icon=None):
    kwargs = {}
    if device_class is not None:
        kwargs["device_class"] = device_class
    if icon is not None:
        kwargs["icon"] = icon
    return binary_sensor.binary_sensor_schema(**kwargs)


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_SMARTGEN_ID): cv.use_id(SmartgenHSC941),
        # Alarms
        cv.Optional(CONF_COMMON_ALARM): bs_schema(),
        cv.Optional(CONF_COMMON_WARNING): bs_schema(),
        cv.Optional(CONF_COMMON_SHUTDOWN): bs_schema(DEVICE_CLASS_SAFETY),
        cv.Optional(CONF_GEN_ON_LOAD): bs_schema(DEVICE_CLASS_RUNNING, "mdi:engine"),
        cv.Optional(CONF_EMERGENCY_STOP): bs_schema(DEVICE_CLASS_SAFETY, "mdi:alert-octagon"),
        # Shutdowns
        cv.Optional(CONF_OVERSPEED_SHUTDOWN): bs_schema(DEVICE_CLASS_SAFETY),
        cv.Optional(CONF_UNDERSPEED_SHUTDOWN): bs_schema(DEVICE_CLASS_SAFETY),
        cv.Optional(CONF_LOSS_SPEED_SIGNAL_SHUTDOWN): bs_schema(DEVICE_CLASS_SAFETY),
        cv.Optional(CONF_GEN_OVERFREQUENCY_SHUTDOWN): bs_schema(DEVICE_CLASS_SAFETY),
        cv.Optional(CONF_GEN_UNDERFREQUENCY_SHUTDOWN): bs_schema(DEVICE_CLASS_SAFETY),
        cv.Optional(CONF_GEN_OVERVOLTAGE_SHUTDOWN): bs_schema(DEVICE_CLASS_SAFETY),
        cv.Optional(CONF_GEN_UNDERVOLTAGE_SHUTDOWN): bs_schema(DEVICE_CLASS_SAFETY),
        cv.Optional(CONF_GEN_OVERCURRENT_SHUTDOWN): bs_schema(DEVICE_CLASS_SAFETY),
        cv.Optional(CONF_CRANK_FAILURE_SHUTDOWN): bs_schema(DEVICE_CLASS_SAFETY),
        cv.Optional(CONF_HIGH_TEMP_SHUTDOWN_IN): bs_schema(DEVICE_CLASS_SAFETY),
        cv.Optional(CONF_LOW_OP_SHUTDOWN_IN): bs_schema(DEVICE_CLASS_SAFETY),
        cv.Optional(CONF_NO_GEN_SHUTDOWN): bs_schema(DEVICE_CLASS_SAFETY),
        cv.Optional(CONF_EXTERNAL_SHUTDOWN_INPUT): bs_schema(DEVICE_CLASS_SAFETY),
        cv.Optional(CONF_LOW_FUEL_LEVEL_SHUTDOWN_IN): bs_schema(DEVICE_CLASS_SAFETY),
        cv.Optional(CONF_LOW_COOLANT_LEVEL_SHUTDOWN_IN): bs_schema(DEVICE_CLASS_SAFETY),
        # Warnings
        cv.Optional(CONF_HIGH_TEMP_WARNING_IN): bs_schema(),
        cv.Optional(CONF_LOW_OP_WARNING_IN): bs_schema(),
        cv.Optional(CONF_STOP_FAILURE_WARNING): bs_schema(),
        cv.Optional(CONF_LOW_FUEL_LEVEL_WARNING_IN): bs_schema(),
        cv.Optional(CONF_CHARGING_FAILURE_WARNING): bs_schema(),
        cv.Optional(CONF_BATTERY_UNDERVOLTAGE_WARNING): bs_schema(),
        cv.Optional(CONF_BATTERY_OVERVOLTAGE_WARNING): bs_schema(),
        cv.Optional(CONF_EXTERNAL_WARNING_INPUT): bs_schema(),
        cv.Optional(CONF_LOSS_SPEED_SIGNAL_WARNING): bs_schema(),
        cv.Optional(CONF_LOW_COOLANT_LEVEL_WARNING_IN): bs_schema(),
        cv.Optional(CONF_TEMP_SENSOR_OPEN_WARNING): bs_schema(),
        cv.Optional(CONF_OP_SENSOR_OPEN_WARNING): bs_schema(),
        cv.Optional(CONF_AUX_SENSOR_OPEN_WARNING): bs_schema(),
        cv.Optional(CONF_AUX_SENSOR_WARNING): bs_schema(),
        cv.Optional(CONF_AUX_SENSOR_SHUTDOWN): bs_schema(DEVICE_CLASS_SAFETY),
        # Mode indicators
        cv.Optional(CONF_IN_AUTO_MODE): bs_schema(DEVICE_CLASS_RUNNING, "mdi:auto-fix"),
        cv.Optional(CONF_IN_MANUAL_MODE): bs_schema(DEVICE_CLASS_RUNNING, "mdi:hand-back-left"),
        cv.Optional(CONF_IN_STOP_MODE): bs_schema(DEVICE_CLASS_RUNNING, "mdi:stop-circle"),
        # More shutdowns
        cv.Optional(CONF_TEMP_SENSOR_OPEN_SHUTDOWN): bs_schema(DEVICE_CLASS_SAFETY),
        cv.Optional(CONF_OP_SENSOR_OPEN_SHUTDOWN): bs_schema(DEVICE_CLASS_SAFETY),
        cv.Optional(CONF_LOW_ENGINE_OIL_LEVEL_SHUTDOWN_IN): bs_schema(DEVICE_CLASS_SAFETY),
        cv.Optional(CONF_AUX_SENSOR_OPEN_SHUTDOWN): bs_schema(DEVICE_CLASS_SAFETY),
        cv.Optional(CONF_EMERGENCY_STOP_INPUT): bs_schema(DEVICE_CLASS_SAFETY, "mdi:alert-octagon"),
        # Aux Inputs
        cv.Optional(CONF_AUX_INPUT_1): bs_schema(None, "mdi:electric-switch"),
        cv.Optional(CONF_AUX_INPUT_2): bs_schema(None, "mdi:electric-switch"),
        cv.Optional(CONF_AUX_INPUT_3): bs_schema(None, "mdi:electric-switch"),
        cv.Optional(CONF_AUX_INPUT_4): bs_schema(None, "mdi:electric-switch"),
        cv.Optional(CONF_AUX_INPUT_5): bs_schema(None, "mdi:electric-switch"),
        cv.Optional(CONF_AUX_INPUT_6): bs_schema(None, "mdi:electric-switch"),
        # Relay Outputs
        cv.Optional(CONF_START_RELAY_OUTPUT): bs_schema(DEVICE_CLASS_RUNNING, "mdi:electric-switch"),
        cv.Optional(CONF_FUEL_RELAY_OUTPUT): bs_schema(DEVICE_CLASS_RUNNING, "mdi:gas-station"),
        cv.Optional(CONF_AUX_OUTPUT_1): bs_schema(None, "mdi:electric-switch"),
        cv.Optional(CONF_AUX_OUTPUT_2): bs_schema(None, "mdi:electric-switch"),
        cv.Optional(CONF_AUX_OUTPUT_3): bs_schema(None, "mdi:electric-switch"),
        cv.Optional(CONF_AUX_OUTPUT_4): bs_schema(None, "mdi:electric-switch"),
        # Analog-derived shutdowns/warnings (coils 0064-0081)
        cv.Optional(CONF_HIGH_TEMP_SHUTDOWN): bs_schema(DEVICE_CLASS_SAFETY),
        cv.Optional(CONF_LOW_OP_SHUTDOWN): bs_schema(DEVICE_CLASS_SAFETY),
        cv.Optional(CONF_HIGH_TEMP_WARNING): bs_schema(),
        cv.Optional(CONF_LOW_OP_WARNING): bs_schema(),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_SMARTGEN_ID])

    bs_map = {
        CONF_COMMON_ALARM: "set_common_alarm_binary_sensor",
        CONF_COMMON_WARNING: "set_common_warning_binary_sensor",
        CONF_COMMON_SHUTDOWN: "set_common_shutdown_binary_sensor",
        CONF_GEN_ON_LOAD: "set_gen_on_load_binary_sensor",
        CONF_EMERGENCY_STOP: "set_emergency_stop_binary_sensor",
        CONF_OVERSPEED_SHUTDOWN: "set_overspeed_shutdown_binary_sensor",
        CONF_UNDERSPEED_SHUTDOWN: "set_underspeed_shutdown_binary_sensor",
        CONF_LOSS_SPEED_SIGNAL_SHUTDOWN: "set_loss_speed_signal_shutdown_binary_sensor",
        CONF_GEN_OVERFREQUENCY_SHUTDOWN: "set_gen_overfrequency_shutdown_binary_sensor",
        CONF_GEN_UNDERFREQUENCY_SHUTDOWN: "set_gen_underfrequency_shutdown_binary_sensor",
        CONF_GEN_OVERVOLTAGE_SHUTDOWN: "set_gen_overvoltage_shutdown_binary_sensor",
        CONF_GEN_UNDERVOLTAGE_SHUTDOWN: "set_gen_undervoltage_shutdown_binary_sensor",
        CONF_GEN_OVERCURRENT_SHUTDOWN: "set_gen_overcurrent_shutdown_binary_sensor",
        CONF_CRANK_FAILURE_SHUTDOWN: "set_crank_failure_shutdown_binary_sensor",
        CONF_HIGH_TEMP_SHUTDOWN_IN: "set_high_temp_shutdown_in_binary_sensor",
        CONF_LOW_OP_SHUTDOWN_IN: "set_low_op_shutdown_in_binary_sensor",
        CONF_NO_GEN_SHUTDOWN: "set_no_gen_shutdown_binary_sensor",
        CONF_EXTERNAL_SHUTDOWN_INPUT: "set_external_shutdown_input_binary_sensor",
        CONF_LOW_FUEL_LEVEL_SHUTDOWN_IN: "set_low_fuel_level_shutdown_in_binary_sensor",
        CONF_LOW_COOLANT_LEVEL_SHUTDOWN_IN: "set_low_coolant_level_shutdown_in_binary_sensor",
        CONF_HIGH_TEMP_WARNING_IN: "set_high_temp_warning_in_binary_sensor",
        CONF_LOW_OP_WARNING_IN: "set_low_op_warning_in_binary_sensor",
        CONF_STOP_FAILURE_WARNING: "set_stop_failure_warning_binary_sensor",
        CONF_LOW_FUEL_LEVEL_WARNING_IN: "set_low_fuel_level_warning_in_binary_sensor",
        CONF_CHARGING_FAILURE_WARNING: "set_charging_failure_warning_binary_sensor",
        CONF_BATTERY_UNDERVOLTAGE_WARNING: "set_battery_undervoltage_warning_binary_sensor",
        CONF_BATTERY_OVERVOLTAGE_WARNING: "set_battery_overvoltage_warning_binary_sensor",
        CONF_EXTERNAL_WARNING_INPUT: "set_external_warning_input_binary_sensor",
        CONF_LOSS_SPEED_SIGNAL_WARNING: "set_loss_speed_signal_warning_binary_sensor",
        CONF_LOW_COOLANT_LEVEL_WARNING_IN: "set_low_coolant_level_warning_in_binary_sensor",
        CONF_TEMP_SENSOR_OPEN_WARNING: "set_temp_sensor_open_warning_binary_sensor",
        CONF_OP_SENSOR_OPEN_WARNING: "set_op_sensor_open_warning_binary_sensor",
        CONF_AUX_SENSOR_OPEN_WARNING: "set_aux_sensor_open_warning_binary_sensor",
        CONF_AUX_SENSOR_WARNING: "set_aux_sensor_warning_binary_sensor",
        CONF_AUX_SENSOR_SHUTDOWN: "set_aux_sensor_shutdown_binary_sensor",
        CONF_IN_AUTO_MODE: "set_in_auto_mode_binary_sensor",
        CONF_IN_MANUAL_MODE: "set_in_manual_mode_binary_sensor",
        CONF_IN_STOP_MODE: "set_in_stop_mode_binary_sensor",
        CONF_TEMP_SENSOR_OPEN_SHUTDOWN: "set_temp_sensor_open_shutdown_binary_sensor",
        CONF_OP_SENSOR_OPEN_SHUTDOWN: "set_op_sensor_open_shutdown_binary_sensor",
        CONF_LOW_ENGINE_OIL_LEVEL_SHUTDOWN_IN: "set_low_engine_oil_level_shutdown_in_binary_sensor",
        CONF_AUX_SENSOR_OPEN_SHUTDOWN: "set_aux_sensor_open_shutdown_binary_sensor",
        CONF_EMERGENCY_STOP_INPUT: "set_emergency_stop_input_binary_sensor",
        CONF_AUX_INPUT_1: "set_aux_input_1_binary_sensor",
        CONF_AUX_INPUT_2: "set_aux_input_2_binary_sensor",
        CONF_AUX_INPUT_3: "set_aux_input_3_binary_sensor",
        CONF_AUX_INPUT_4: "set_aux_input_4_binary_sensor",
        CONF_AUX_INPUT_5: "set_aux_input_5_binary_sensor",
        CONF_AUX_INPUT_6: "set_aux_input_6_binary_sensor",
        CONF_START_RELAY_OUTPUT: "set_start_relay_output_binary_sensor",
        CONF_FUEL_RELAY_OUTPUT: "set_fuel_relay_output_binary_sensor",
        CONF_AUX_OUTPUT_1: "set_aux_output_1_binary_sensor",
        CONF_AUX_OUTPUT_2: "set_aux_output_2_binary_sensor",
        CONF_AUX_OUTPUT_3: "set_aux_output_3_binary_sensor",
        CONF_AUX_OUTPUT_4: "set_aux_output_4_binary_sensor",
        CONF_HIGH_TEMP_SHUTDOWN: "set_high_temp_shutdown_binary_sensor",
        CONF_LOW_OP_SHUTDOWN: "set_low_op_shutdown_binary_sensor",
        CONF_HIGH_TEMP_WARNING: "set_high_temp_warning_binary_sensor",
        CONF_LOW_OP_WARNING: "set_low_op_warning_binary_sensor",
    }

    for conf_key, setter in bs_map.items():
        if conf_key in config:
            sens = await binary_sensor.new_binary_sensor(config[conf_key])
            cg.add(getattr(parent, setter)(sens))
