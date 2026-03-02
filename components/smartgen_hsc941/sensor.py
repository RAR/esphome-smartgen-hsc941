import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import (
    CONF_ID,
    DEVICE_CLASS_VOLTAGE,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_FREQUENCY,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_PRESSURE,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_ENERGY,
    DEVICE_CLASS_POWER_FACTOR,
    DEVICE_CLASS_SPEED,
    STATE_CLASS_MEASUREMENT,
    STATE_CLASS_TOTAL_INCREASING,
    UNIT_VOLT,
    UNIT_AMPERE,
    UNIT_HERTZ,
    UNIT_CELSIUS,
    UNIT_WATT,
    UNIT_KILOWATT_HOURS,
    UNIT_PERCENT,
)

from . import SmartgenHSC941, smartgen_hsc941_ns

DEPENDENCIES = ["smartgen_hsc941"]

# Custom units not in esphome.const
UNIT_KPA = "kPa"
UNIT_OHM = "Ω"
UNIT_RPM = "RPM"
UNIT_KW = "kW"
UNIT_KVAR = "kVAR"
UNIT_KVA = "kVA"
UNIT_DEGREE = "°"
UNIT_SECOND = "s"
UNIT_HOUR = "h"

# Configuration keys for all sensors
CONF_SMARTGEN_ID = "smartgen_hsc941_id"

# Generator Voltages
CONF_GEN_VOLTAGE_A = "gen_voltage_a"
CONF_GEN_VOLTAGE_B = "gen_voltage_b"
CONF_GEN_VOLTAGE_C = "gen_voltage_c"
CONF_GEN_VOLTAGE_AB = "gen_voltage_ab"
CONF_GEN_VOLTAGE_BC = "gen_voltage_bc"
CONF_GEN_VOLTAGE_CA = "gen_voltage_ca"

# Generator Frequency
CONF_GEN_FREQUENCY = "gen_frequency"

# Phase Currents
CONF_PHASE_A_CURRENT = "phase_a_current"
CONF_PHASE_B_CURRENT = "phase_b_current"
CONF_PHASE_C_CURRENT = "phase_c_current"

# Temperature & Pressure
CONF_WATER_TEMP = "water_temperature"
CONF_WATER_TEMP_RESISTANCE = "water_temp_resistance"
CONF_OIL_PRESSURE = "oil_pressure"
CONF_OIL_PRESSURE_RESISTANCE = "oil_pressure_resistance"

# Aux Sensor
CONF_AUX_SENSOR_1 = "aux_sensor_1"
CONF_AUX_SENSOR_1_RESISTANCE = "aux_sensor_1_resistance"

# Engine
CONF_ENGINE_SPEED = "engine_speed"

# Battery
CONF_BATTERY_VOLTAGE = "battery_voltage"
CONF_CHARGE_VOLTAGE = "charge_voltage"

# Date/Time
CONF_CONTROLLER_YEAR = "controller_year"
CONF_CONTROLLER_MONTH = "controller_month"
CONF_CONTROLLER_DAY = "controller_day"
CONF_CONTROLLER_WEEKDAY = "controller_weekday"
CONF_CONTROLLER_HOUR = "controller_hour"
CONF_CONTROLLER_MINUTE = "controller_minute"
CONF_CONTROLLER_SECOND = "controller_second"

# Engine Status & Timing
CONF_ENGINE_RUNNING_STATUS = "engine_running_status"
CONF_ENGINE_DELAY = "engine_delay"
CONF_AUTO_RUNNING_STATUS = "auto_running_status"
CONF_AUTO_DELAY = "auto_delay"

# Accumulated Values
CONF_ENGINE_TOTAL_HOURS = "engine_total_hours"
CONF_ENGINE_TOTAL_MINUTES = "engine_total_minutes"
CONF_ENGINE_TOTAL_SECONDS = "engine_total_seconds"
CONF_TOTAL_START_TIMES = "total_start_times"
CONF_TOTAL_ENERGY = "total_energy"

# Software/Hardware
CONF_SOFTWARE_VERSION = "software_version"
CONF_HARDWARE_VERSION = "hardware_version"

# Phase Angles
CONF_PHASE_A_ANGLE = "phase_a_angle"
CONF_PHASE_B_ANGLE = "phase_b_angle"
CONF_PHASE_C_ANGLE = "phase_c_angle"

# Power
CONF_PHASE_A_ACTIVE_POWER = "phase_a_active_power"
CONF_PHASE_B_ACTIVE_POWER = "phase_b_active_power"
CONF_PHASE_C_ACTIVE_POWER = "phase_c_active_power"
CONF_TOTAL_ACTIVE_POWER = "total_active_power"
CONF_REACTIVE_POWER = "reactive_power"
CONF_APPARENT_POWER = "apparent_power"
CONF_POWER_FACTOR = "power_factor"
CONF_OUTPUT_LOAD_PERCENT = "output_load_percent"

# Release Date
CONF_RELEASE_YEAR = "release_year"
CONF_RELEASE_MONTH = "release_month"
CONF_RELEASE_DAY = "release_day"


def sensor_schema(unit, icon, accuracy, device_class=None, state_class=STATE_CLASS_MEASUREMENT):
    schema = sensor.sensor_schema(
        unit_of_measurement=unit,
        icon=icon,
        accuracy_decimals=accuracy,
        device_class=device_class,
        state_class=state_class,
    )
    return schema


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_SMARTGEN_ID): cv.use_id(SmartgenHSC941),
        # Generator Voltages
        cv.Optional(CONF_GEN_VOLTAGE_A): sensor_schema(UNIT_VOLT, "mdi:flash", 0, DEVICE_CLASS_VOLTAGE),
        cv.Optional(CONF_GEN_VOLTAGE_B): sensor_schema(UNIT_VOLT, "mdi:flash", 0, DEVICE_CLASS_VOLTAGE),
        cv.Optional(CONF_GEN_VOLTAGE_C): sensor_schema(UNIT_VOLT, "mdi:flash", 0, DEVICE_CLASS_VOLTAGE),
        cv.Optional(CONF_GEN_VOLTAGE_AB): sensor_schema(UNIT_VOLT, "mdi:flash", 0, DEVICE_CLASS_VOLTAGE),
        cv.Optional(CONF_GEN_VOLTAGE_BC): sensor_schema(UNIT_VOLT, "mdi:flash", 0, DEVICE_CLASS_VOLTAGE),
        cv.Optional(CONF_GEN_VOLTAGE_CA): sensor_schema(UNIT_VOLT, "mdi:flash", 0, DEVICE_CLASS_VOLTAGE),
        # Frequency
        cv.Optional(CONF_GEN_FREQUENCY): sensor_schema(UNIT_HERTZ, "mdi:sine-wave", 2, DEVICE_CLASS_FREQUENCY),
        # Currents
        cv.Optional(CONF_PHASE_A_CURRENT): sensor_schema(UNIT_AMPERE, "mdi:current-ac", 1, DEVICE_CLASS_CURRENT),
        cv.Optional(CONF_PHASE_B_CURRENT): sensor_schema(UNIT_AMPERE, "mdi:current-ac", 1, DEVICE_CLASS_CURRENT),
        cv.Optional(CONF_PHASE_C_CURRENT): sensor_schema(UNIT_AMPERE, "mdi:current-ac", 1, DEVICE_CLASS_CURRENT),
        # Temperature
        cv.Optional(CONF_WATER_TEMP): sensor_schema(UNIT_CELSIUS, "mdi:thermometer", 0, DEVICE_CLASS_TEMPERATURE),
        cv.Optional(CONF_WATER_TEMP_RESISTANCE): sensor_schema(UNIT_OHM, "mdi:omega", 1),
        # Oil Pressure
        cv.Optional(CONF_OIL_PRESSURE): sensor_schema(UNIT_KPA, "mdi:gauge", 0, DEVICE_CLASS_PRESSURE),
        cv.Optional(CONF_OIL_PRESSURE_RESISTANCE): sensor_schema(UNIT_OHM, "mdi:omega", 1),
        # Aux Sensor
        cv.Optional(CONF_AUX_SENSOR_1): sensor_schema("", "mdi:gauge", 0),
        cv.Optional(CONF_AUX_SENSOR_1_RESISTANCE): sensor_schema(UNIT_OHM, "mdi:omega", 1),
        # Engine Speed
        cv.Optional(CONF_ENGINE_SPEED): sensor_schema(UNIT_RPM, "mdi:engine", 0, DEVICE_CLASS_SPEED),
        # Battery
        cv.Optional(CONF_BATTERY_VOLTAGE): sensor_schema(UNIT_VOLT, "mdi:car-battery", 1, DEVICE_CLASS_VOLTAGE),
        cv.Optional(CONF_CHARGE_VOLTAGE): sensor_schema(UNIT_VOLT, "mdi:battery-charging", 1, DEVICE_CLASS_VOLTAGE),
        # Date/Time
        cv.Optional(CONF_CONTROLLER_YEAR): sensor_schema("", "mdi:calendar", 0),
        cv.Optional(CONF_CONTROLLER_MONTH): sensor_schema("", "mdi:calendar", 0),
        cv.Optional(CONF_CONTROLLER_DAY): sensor_schema("", "mdi:calendar", 0),
        cv.Optional(CONF_CONTROLLER_WEEKDAY): sensor_schema("", "mdi:calendar", 0),
        cv.Optional(CONF_CONTROLLER_HOUR): sensor_schema("", "mdi:clock-outline", 0),
        cv.Optional(CONF_CONTROLLER_MINUTE): sensor_schema("", "mdi:clock-outline", 0),
        cv.Optional(CONF_CONTROLLER_SECOND): sensor_schema("", "mdi:clock-outline", 0),
        # Engine Status
        cv.Optional(CONF_ENGINE_RUNNING_STATUS): sensor_schema("", "mdi:engine", 0),
        cv.Optional(CONF_ENGINE_DELAY): sensor_schema(UNIT_SECOND, "mdi:timer", 0),
        cv.Optional(CONF_AUTO_RUNNING_STATUS): sensor_schema("", "mdi:auto-fix", 0),
        cv.Optional(CONF_AUTO_DELAY): sensor_schema(UNIT_SECOND, "mdi:timer", 0),
        # Accumulated Values
        cv.Optional(CONF_ENGINE_TOTAL_HOURS): sensor_schema(UNIT_HOUR, "mdi:timer-sand", 0, state_class=STATE_CLASS_TOTAL_INCREASING),
        cv.Optional(CONF_ENGINE_TOTAL_MINUTES): sensor_schema("min", "mdi:timer-sand", 0),
        cv.Optional(CONF_ENGINE_TOTAL_SECONDS): sensor_schema("s", "mdi:timer-sand", 0),
        cv.Optional(CONF_TOTAL_START_TIMES): sensor_schema("", "mdi:counter", 0, state_class=STATE_CLASS_TOTAL_INCREASING),
        cv.Optional(CONF_TOTAL_ENERGY): sensor_schema(UNIT_KILOWATT_HOURS, "mdi:lightning-bolt", 0, DEVICE_CLASS_ENERGY, STATE_CLASS_TOTAL_INCREASING),
        # Versions
        cv.Optional(CONF_SOFTWARE_VERSION): sensor_schema("", "mdi:information", 0),
        cv.Optional(CONF_HARDWARE_VERSION): sensor_schema("", "mdi:information", 0),
        # Phase Angles
        cv.Optional(CONF_PHASE_A_ANGLE): sensor_schema(UNIT_DEGREE, "mdi:angle-acute", 0),
        cv.Optional(CONF_PHASE_B_ANGLE): sensor_schema(UNIT_DEGREE, "mdi:angle-acute", 0),
        cv.Optional(CONF_PHASE_C_ANGLE): sensor_schema(UNIT_DEGREE, "mdi:angle-acute", 0),
        # Power
        cv.Optional(CONF_PHASE_A_ACTIVE_POWER): sensor_schema(UNIT_KW, "mdi:flash", 1, DEVICE_CLASS_POWER),
        cv.Optional(CONF_PHASE_B_ACTIVE_POWER): sensor_schema(UNIT_KW, "mdi:flash", 1, DEVICE_CLASS_POWER),
        cv.Optional(CONF_PHASE_C_ACTIVE_POWER): sensor_schema(UNIT_KW, "mdi:flash", 1, DEVICE_CLASS_POWER),
        cv.Optional(CONF_TOTAL_ACTIVE_POWER): sensor_schema(UNIT_KW, "mdi:flash", 1, DEVICE_CLASS_POWER),
        cv.Optional(CONF_REACTIVE_POWER): sensor_schema(UNIT_KVAR, "mdi:flash", 1),
        cv.Optional(CONF_APPARENT_POWER): sensor_schema(UNIT_KVA, "mdi:flash", 1),
        cv.Optional(CONF_POWER_FACTOR): sensor_schema("", "mdi:angle-acute", 2, DEVICE_CLASS_POWER_FACTOR),
        cv.Optional(CONF_OUTPUT_LOAD_PERCENT): sensor_schema(UNIT_PERCENT, "mdi:gauge", 0),
        # Release Date
        cv.Optional(CONF_RELEASE_YEAR): sensor_schema("", "mdi:calendar", 0),
        cv.Optional(CONF_RELEASE_MONTH): sensor_schema("", "mdi:calendar", 0),
        cv.Optional(CONF_RELEASE_DAY): sensor_schema("", "mdi:calendar", 0),
    }
)


async def to_code(config):
    parent = await cg.get_variable(config[CONF_SMARTGEN_ID])

    sensor_map = {
        CONF_GEN_VOLTAGE_A: "set_gen_voltage_a_sensor",
        CONF_GEN_VOLTAGE_B: "set_gen_voltage_b_sensor",
        CONF_GEN_VOLTAGE_C: "set_gen_voltage_c_sensor",
        CONF_GEN_VOLTAGE_AB: "set_gen_voltage_ab_sensor",
        CONF_GEN_VOLTAGE_BC: "set_gen_voltage_bc_sensor",
        CONF_GEN_VOLTAGE_CA: "set_gen_voltage_ca_sensor",
        CONF_GEN_FREQUENCY: "set_gen_frequency_sensor",
        CONF_PHASE_A_CURRENT: "set_phase_a_current_sensor",
        CONF_PHASE_B_CURRENT: "set_phase_b_current_sensor",
        CONF_PHASE_C_CURRENT: "set_phase_c_current_sensor",
        CONF_WATER_TEMP: "set_water_temp_sensor",
        CONF_WATER_TEMP_RESISTANCE: "set_water_temp_resistance_sensor",
        CONF_OIL_PRESSURE: "set_oil_pressure_sensor",
        CONF_OIL_PRESSURE_RESISTANCE: "set_oil_pressure_resistance_sensor",
        CONF_AUX_SENSOR_1: "set_aux_sensor_1_sensor",
        CONF_AUX_SENSOR_1_RESISTANCE: "set_aux_sensor_1_resistance_sensor",
        CONF_ENGINE_SPEED: "set_engine_speed_sensor",
        CONF_BATTERY_VOLTAGE: "set_battery_voltage_sensor",
        CONF_CHARGE_VOLTAGE: "set_charge_voltage_sensor",
        CONF_CONTROLLER_YEAR: "set_controller_year_sensor",
        CONF_CONTROLLER_MONTH: "set_controller_month_sensor",
        CONF_CONTROLLER_DAY: "set_controller_day_sensor",
        CONF_CONTROLLER_WEEKDAY: "set_controller_weekday_sensor",
        CONF_CONTROLLER_HOUR: "set_controller_hour_sensor",
        CONF_CONTROLLER_MINUTE: "set_controller_minute_sensor",
        CONF_CONTROLLER_SECOND: "set_controller_second_sensor",
        CONF_ENGINE_RUNNING_STATUS: "set_engine_running_status_sensor",
        CONF_ENGINE_DELAY: "set_engine_delay_sensor",
        CONF_AUTO_RUNNING_STATUS: "set_auto_running_status_sensor",
        CONF_AUTO_DELAY: "set_auto_delay_sensor",
        CONF_ENGINE_TOTAL_HOURS: "set_engine_total_hours_sensor",
        CONF_ENGINE_TOTAL_MINUTES: "set_engine_total_minutes_sensor",
        CONF_ENGINE_TOTAL_SECONDS: "set_engine_total_seconds_sensor",
        CONF_TOTAL_START_TIMES: "set_total_start_times_sensor",
        CONF_TOTAL_ENERGY: "set_total_energy_sensor",
        CONF_SOFTWARE_VERSION: "set_software_version_sensor",
        CONF_HARDWARE_VERSION: "set_hardware_version_sensor",
        CONF_PHASE_A_ANGLE: "set_phase_a_angle_sensor",
        CONF_PHASE_B_ANGLE: "set_phase_b_angle_sensor",
        CONF_PHASE_C_ANGLE: "set_phase_c_angle_sensor",
        CONF_PHASE_A_ACTIVE_POWER: "set_phase_a_active_power_sensor",
        CONF_PHASE_B_ACTIVE_POWER: "set_phase_b_active_power_sensor",
        CONF_PHASE_C_ACTIVE_POWER: "set_phase_c_active_power_sensor",
        CONF_TOTAL_ACTIVE_POWER: "set_total_active_power_sensor",
        CONF_REACTIVE_POWER: "set_reactive_power_sensor",
        CONF_APPARENT_POWER: "set_apparent_power_sensor",
        CONF_POWER_FACTOR: "set_power_factor_sensor",
        CONF_OUTPUT_LOAD_PERCENT: "set_output_load_percent_sensor",
        CONF_RELEASE_YEAR: "set_release_year_sensor",
        CONF_RELEASE_MONTH: "set_release_month_sensor",
        CONF_RELEASE_DAY: "set_release_day_sensor",
    }

    for conf_key, setter in sensor_map.items():
        if conf_key in config:
            sens = await sensor.new_sensor(config[conf_key])
            cg.add(getattr(parent, setter)(sens))
