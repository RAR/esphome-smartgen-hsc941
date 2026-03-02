# ESPHome SmartGen HSC941 Component

An [ESPHome](https://esphome.io/) external component for the **SmartGen HSC941** generator controller. Communicates over **Modbus RTU / RS485** using a custom implementation built directly on the ESP-IDF UART driver — no dependency on the ESPHome `modbus` component.

## Features

- **Full monitoring** of all HSC941 registers as defined in the official protocol manual
- **Remote control** via Modbus coil writes (start, stop, auto, manual, gen switch, fault reset)
- **Custom Modbus RTU** implementation using ESP-IDF `driver/uart.h` — avoids known reliability issues with the built-in ESPHome modbus stack
- **ESP32-S3** with built-in RS485 transceiver support
- Optional flow control pin for RS485 DE/RE direction control
- Auto-detects communication failures and reports status in Home Assistant

## Supported Hardware

- **Controller:** SmartGen HSC941 (firmware V1.0+)
- **MCU:** ESP32-S3 (ESP-IDF framework)
- **Interface:** RS485 (half-duplex, 9600 baud, 8N1)

## Installation

Add this repository as an external component in your ESPHome YAML:

```yaml
external_components:
  - source:
      type: git
      url: https://github.com/RAR/esphome-smartgen-hsc941
      ref: main
    components: [smartgen_hsc941]
```

## Configuration

### Hub

```yaml
smartgen_hsc941:
  id: genset
  address: 1              # Modbus slave address (1-247, default: 1)
  baud_rate: 9600          # Must match controller (default: 9600)
  tx_pin: GPIO17           # RS485 TX pin
  rx_pin: GPIO18           # RS485 RX pin
  # flow_control_pin: GPIO8  # Optional: DE/RE pin for RS485 transceiver
  uart_num: 1              # ESP32 UART number (0-2, default: 1)
  update_interval: 5s      # Polling interval (default: 5s)
```

| Parameter | Required | Default | Description |
|-----------|----------|---------|-------------|
| `address` | No | `1` | Modbus slave address of the HSC941 |
| `baud_rate` | No | `9600` | Serial baud rate (must match controller) |
| `tx_pin` | **Yes** | — | GPIO pin for RS485 TX |
| `rx_pin` | **Yes** | — | GPIO pin for RS485 RX |
| `flow_control_pin` | No | — | GPIO pin for RS485 DE/RE direction control |
| `uart_num` | No | `1` | ESP32 UART peripheral number (0, 1, or 2) |
| `update_interval` | No | `5s` | How often to poll the controller |

### Sensors (Holding Registers — Function Code 03H)

All sensors are optional. Only include the ones you need.

```yaml
sensor:
  - platform: smartgen_hsc941
    smartgen_hsc941_id: genset

    # Generator Voltages
    gen_voltage_a:
      name: "Gen Voltage Phase A"
    gen_voltage_b:
      name: "Gen Voltage Phase B"
    gen_voltage_c:
      name: "Gen Voltage Phase C"
    gen_voltage_ab:
      name: "Gen Voltage AB"
    gen_voltage_bc:
      name: "Gen Voltage BC"
    gen_voltage_ca:
      name: "Gen Voltage CA"

    # Frequency
    gen_frequency:
      name: "Gen Frequency"

    # Phase Currents
    phase_a_current:
      name: "Gen Current Phase A"
    phase_b_current:
      name: "Gen Current Phase B"
    phase_c_current:
      name: "Gen Current Phase C"

    # Temperature & Pressure
    water_temperature:
      name: "Gen Coolant Temperature"
    water_temp_resistance:
      name: "Gen Coolant Temp Resistance"
    oil_pressure:
      name: "Gen Oil Pressure"
    oil_pressure_resistance:
      name: "Gen Oil Pressure Resistance"

    # Aux Sensor
    aux_sensor_1:
      name: "Gen Aux Sensor 1"
    aux_sensor_1_resistance:
      name: "Gen Aux Sensor 1 Resistance"

    # Engine
    engine_speed:
      name: "Gen Engine Speed"

    # Battery
    battery_voltage:
      name: "Gen Battery Voltage"
    charge_voltage:
      name: "Gen D+ Charge Voltage"

    # Controller Date/Time
    controller_year:
      name: "Gen Controller Year"
    controller_month:
      name: "Gen Controller Month"
    controller_day:
      name: "Gen Controller Day"
    controller_weekday:
      name: "Gen Controller Weekday"
    controller_hour:
      name: "Gen Controller Hour"
    controller_minute:
      name: "Gen Controller Minute"
    controller_second:
      name: "Gen Controller Second"

    # Engine Status
    engine_running_status:
      name: "Gen Engine Running Status"
    engine_delay:
      name: "Gen Engine Delay"
    auto_running_status:
      name: "Gen Auto Running Status"
    auto_delay:
      name: "Gen Auto Delay"

    # Accumulated Values
    engine_total_hours:
      name: "Gen Total Running Hours"
    engine_total_minutes:
      name: "Gen Running Minutes"
    engine_total_seconds:
      name: "Gen Running Seconds"
    total_start_times:
      name: "Gen Total Starts"
    total_energy:
      name: "Gen Total Energy"

    # Versions
    software_version:
      name: "Gen Software Version"
    hardware_version:
      name: "Gen Hardware Version"

    # Phase Angles
    phase_a_angle:
      name: "Gen Phase A Angle"
    phase_b_angle:
      name: "Gen Phase B Angle"
    phase_c_angle:
      name: "Gen Phase C Angle"

    # Power
    phase_a_active_power:
      name: "Gen Active Power Phase A"
    phase_b_active_power:
      name: "Gen Active Power Phase B"
    phase_c_active_power:
      name: "Gen Active Power Phase C"
    total_active_power:
      name: "Gen Total Active Power"
    reactive_power:
      name: "Gen Reactive Power"
    apparent_power:
      name: "Gen Apparent Power"
    power_factor:
      name: "Gen Power Factor"
    output_load_percent:
      name: "Gen Output Load"

    # Firmware Release Date
    release_year:
      name: "Gen Release Year"
    release_month:
      name: "Gen Release Month"
    release_day:
      name: "Gen Release Day"
```

#### Sensor Reference

| Key | Unit | Register | Ratio | Description |
|-----|------|----------|-------|-------------|
| `gen_voltage_a` | V | 0007 | 1 | Generator Phase A voltage |
| `gen_voltage_b` | V | 0008 | 1 | Generator Phase B voltage |
| `gen_voltage_c` | V | 0009 | 1 | Generator Phase C voltage |
| `gen_voltage_ab` | V | 0010 | 1 | Generator line voltage AB |
| `gen_voltage_bc` | V | 0011 | 1 | Generator line voltage BC |
| `gen_voltage_ca` | V | 0012 | 1 | Generator line voltage CA |
| `gen_frequency` | Hz | 0013 | 0.01 | Generator frequency |
| `phase_a_current` | A | 0014 | 0.1 | Phase A current |
| `phase_b_current` | A | 0015 | 0.1 | Phase B current |
| `phase_c_current` | A | 0016 | 0.1 | Phase C current |
| `water_temperature` | °C | 0017 | 1 | Coolant water temperature |
| `water_temp_resistance` | Ω | 0018 | 0.1 | Coolant temp sensor resistance |
| `oil_pressure` | kPa | 0019 | 1 | Engine oil pressure |
| `oil_pressure_resistance` | Ω | 0020 | 0.1 | Oil pressure sensor resistance |
| `aux_sensor_1` | — | 0021 | 1 | Auxiliary sensor 1 value |
| `aux_sensor_1_resistance` | Ω | 0022 | 0.1 | Auxiliary sensor 1 resistance |
| `engine_speed` | RPM | 0023 | 1 | Engine speed |
| `battery_voltage` | V | 0024 | 0.1 | Battery voltage |
| `charge_voltage` | V | 0025 | 0.1 | D+ charging voltage |
| `controller_year` | — | 0026 | 1 | Controller RTC year |
| `controller_month` | — | 0027 | 1 | Controller RTC month |
| `controller_day` | — | 0028 | 1 | Controller RTC day |
| `controller_weekday` | — | 0029 | — | Day of week (0=Sun, 1-6=Mon-Sat) |
| `controller_hour` | — | 0030 | 1 | Controller RTC hour |
| `controller_minute` | — | 0031 | 1 | Controller RTC minute |
| `controller_second` | — | 0032 | 1 | Controller RTC second |
| `engine_running_status` | — | 0034 | — | Engine state (see table below) |
| `engine_delay` | s | 0035 | 1 | Current state delay countdown |
| `auto_running_status` | — | 0036 | — | Auto mode state |
| `auto_delay` | s | 0037 | 1 | Auto mode delay countdown |
| `engine_total_hours` | h | 0042–0043 | 1 | Accumulated running hours (32-bit) |
| `engine_total_minutes` | min | 0044 | 1 | Running time minutes part |
| `engine_total_seconds` | s | 0045 | 1 | Running time seconds part |
| `total_start_times` | — | 0046–0047 | 1 | Accumulated start count (32-bit) |
| `total_energy` | kWh | 0048–0049 | 1 | Accumulated energy (32-bit) |
| `software_version` | — | 0050 | — | Controller software version |
| `hardware_version` | — | 0051 | — | Controller hardware version |
| `phase_a_angle` | ° | 0052 | 1 | Phase A angle (signed) |
| `phase_b_angle` | ° | 0053 | 1 | Phase B angle (signed) |
| `phase_c_angle` | ° | 0054 | 1 | Phase C angle (signed) |
| `phase_a_active_power` | kW | 0055 | 0.1 | Phase A active power (signed) |
| `phase_b_active_power` | kW | 0056 | 0.1 | Phase B active power (signed) |
| `phase_c_active_power` | kW | 0057 | 0.1 | Phase C active power (signed) |
| `total_active_power` | kW | 0058 | 0.1 | Total active power (signed) |
| `reactive_power` | kVAR | 0059 | 0.1 | Reactive power (signed) |
| `apparent_power` | kVA | 0060 | 0.1 | Apparent power (signed) |
| `power_factor` | — | 0061 | 0.01 | Power factor |
| `output_load_percent` | % | 0062 | 1 | Output load percentage |
| `release_year` | — | 0063 | 1 | Firmware release year |
| `release_month` | — | 0064 | 1 | Firmware release month |
| `release_day` | — | 0065 | 1 | Firmware release day |

#### Engine Running Status Values

| Value | State | Description |
|-------|-------|-------------|
| 0 | Standby | Engine idle, no active sequence |
| 1 | Preheat | Glow plug / intake heater active |
| 2 | Fuel Output | Fuel solenoid energized |
| 3 | Crank | Starter motor engaged |
| 4 | Crank Rest | Pause between crank attempts |
| 5 | Safety On Delay | Post-start safety delay |
| 6 | Start Idle | Low-idle warm-up |
| 7 | Warming Up | Engine warming up |
| 8 | Wait for On-load | Waiting to accept load |
| 9 | Normal Running | Engine running normally |
| 10 | Cooling | Cool-down period before stop |
| 11 | Stop Idle | Low-idle before shutdown |
| 12 | ETS | Emergency trip sequence |
| 13 | Wait for Stop | Waiting for engine to stop |
| 14 | Stop Failure | Engine failed to stop |

### Binary Sensors (Coils — Function Code 01H)

All binary sensors are optional.

```yaml
binary_sensor:
  - platform: smartgen_hsc941
    smartgen_hsc941_id: genset

    # Summary Alarms
    common_alarm:
      name: "Gen Common Alarm"
    common_warning:
      name: "Gen Common Warning"
    common_shutdown:
      name: "Gen Common Shutdown"

    # Operating Mode
    gen_on_load:
      name: "Gen On Load"
    in_auto_mode:
      name: "Gen Auto Mode"
    in_manual_mode:
      name: "Gen Manual Mode"
    in_stop_mode:
      name: "Gen Stop Mode"

    # Emergency
    emergency_stop:
      name: "Gen Emergency Stop"
    emergency_stop_input:
      name: "Gen E-Stop Input"

    # Shutdown Alarms
    overspeed_shutdown:
      name: "Gen Overspeed Shutdown"
    underspeed_shutdown:
      name: "Gen Underspeed Shutdown"
    loss_speed_signal_shutdown:
      name: "Gen Loss Speed Signal Shutdown"
    gen_overfrequency_shutdown:
      name: "Gen Overfrequency Shutdown"
    gen_underfrequency_shutdown:
      name: "Gen Underfrequency Shutdown"
    gen_overvoltage_shutdown:
      name: "Gen Overvoltage Shutdown"
    gen_undervoltage_shutdown:
      name: "Gen Undervoltage Shutdown"
    gen_overcurrent_shutdown:
      name: "Gen Overcurrent Shutdown"
    crank_failure_shutdown:
      name: "Gen Crank Failure"
    no_gen_shutdown:
      name: "Gen No Gen Shutdown"
    external_shutdown_input:
      name: "Gen External Shutdown"

    # Input-based Shutdowns
    high_temp_shutdown_in:
      name: "Gen High Temp Shutdown (Input)"
    low_oil_pressure_shutdown_in:
      name: "Gen Low Oil Pressure Shutdown (Input)"
    low_fuel_level_shutdown_in:
      name: "Gen Low Fuel Shutdown (Input)"
    low_coolant_level_shutdown_in:
      name: "Gen Low Coolant Shutdown (Input)"
    low_engine_oil_level_shutdown_in:
      name: "Gen Low Engine Oil Level Shutdown (Input)"

    # Analog-derived Shutdowns
    high_temp_shutdown:
      name: "Gen High Temp Shutdown"
    low_oil_pressure_shutdown:
      name: "Gen Low Oil Pressure Shutdown"

    # Sensor Open Shutdowns
    temp_sensor_open_shutdown:
      name: "Gen Temp Sensor Open Shutdown"
    oil_pressure_sensor_open_shutdown:
      name: "Gen OP Sensor Open Shutdown"
    aux_sensor_open_shutdown:
      name: "Gen Aux Sensor Open Shutdown"
    aux_sensor_shutdown:
      name: "Gen Aux Sensor Shutdown"

    # Warnings
    high_temp_warning_in:
      name: "Gen High Temp Warning (Input)"
    low_oil_pressure_warning_in:
      name: "Gen Low Oil Pressure Warning (Input)"
    stop_failure_warning:
      name: "Gen Stop Failure Warning"
    low_fuel_level_warning_in:
      name: "Gen Low Fuel Warning (Input)"
    charging_failure_warning:
      name: "Gen Charging Failure Warning"
    battery_undervoltage_warning:
      name: "Gen Battery Undervoltage Warning"
    battery_overvoltage_warning:
      name: "Gen Battery Overvoltage Warning"
    external_warning_input:
      name: "Gen External Warning"
    loss_speed_signal_warning:
      name: "Gen Loss Speed Signal Warning"
    low_coolant_level_warning_in:
      name: "Gen Low Coolant Warning (Input)"

    # Analog-derived Warnings
    high_temp_warning:
      name: "Gen High Temp Warning"
    low_oil_pressure_warning:
      name: "Gen Low Oil Pressure Warning"

    # Sensor Open Warnings
    temp_sensor_open_warning:
      name: "Gen Temp Sensor Open Warning"
    oil_pressure_sensor_open_warning:
      name: "Gen OP Sensor Open Warning"
    aux_sensor_open_warning:
      name: "Gen Aux Sensor Open Warning"
    aux_sensor_warning:
      name: "Gen Aux Sensor Warning"

    # Auxiliary Inputs
    aux_input_1:
      name: "Gen Aux Input 1"
    aux_input_2:
      name: "Gen Aux Input 2"
    aux_input_3:
      name: "Gen Aux Input 3"
    aux_input_4:
      name: "Gen Aux Input 4"
    aux_input_5:
      name: "Gen Aux Input 5"
    aux_input_6:
      name: "Gen Aux Input 6"

    # Relay Output Status
    start_relay_output:
      name: "Gen Start Relay"
    fuel_relay_output:
      name: "Gen Fuel Relay"
    aux_output_1:
      name: "Gen Aux Output 1"
    aux_output_2:
      name: "Gen Aux Output 2"
    aux_output_3:
      name: "Gen Aux Output 3"
    aux_output_4:
      name: "Gen Aux Output 4"
```

#### Binary Sensor Reference

| Key | Coil Address | Description |
|-----|-------------|-------------|
| `common_alarm` | 0000 | Any alarm active |
| `common_warning` | 0001 | Any warning active |
| `common_shutdown` | 0002 | Any shutdown active |
| `gen_on_load` | 0007 | Generator supplying load |
| `emergency_stop` | 0008 | Emergency stop active |
| `overspeed_shutdown` | 0009 | Engine overspeed |
| `underspeed_shutdown` | 0010 | Engine underspeed |
| `loss_speed_signal_shutdown` | 0011 | Speed sensor signal lost |
| `gen_overfrequency_shutdown` | 0012 | Generator overfrequency |
| `gen_underfrequency_shutdown` | 0013 | Generator underfrequency |
| `gen_overvoltage_shutdown` | 0014 | Generator overvoltage |
| `gen_undervoltage_shutdown` | 0015 | Generator undervoltage |
| `gen_overcurrent_shutdown` | 0016 | Generator overcurrent |
| `crank_failure_shutdown` | 0017 | Failed to start (crank) |
| `high_temp_shutdown_in` | 0018 | High temperature (digital input) |
| `low_oil_pressure_shutdown_in` | 0019 | Low oil pressure (digital input) |
| `no_gen_shutdown` | 0020 | No generator output detected |
| `external_shutdown_input` | 0021 | External shutdown input active |
| `low_fuel_level_shutdown_in` | 0022 | Low fuel level (digital input) |
| `low_coolant_level_shutdown_in` | 0023 | Low coolant level (digital input) |
| `high_temp_warning_in` | 0024 | High temperature warning (digital input) |
| `low_oil_pressure_warning_in` | 0025 | Low oil pressure warning (digital input) |
| `stop_failure_warning` | 0027 | Engine failed to stop |
| `low_fuel_level_warning_in` | 0028 | Low fuel level warning (digital input) |
| `charging_failure_warning` | 0029 | Battery charging failure |
| `battery_undervoltage_warning` | 0030 | Battery voltage too low |
| `battery_overvoltage_warning` | 0031 | Battery voltage too high |
| `external_warning_input` | 0032 | External warning input active |
| `loss_speed_signal_warning` | 0033 | Speed sensor signal lost (warning) |
| `low_coolant_level_warning_in` | 0034 | Low coolant level warning (digital input) |
| `temp_sensor_open_warning` | 0035 | Temperature sensor open circuit |
| `oil_pressure_sensor_open_warning` | 0036 | Oil pressure sensor open circuit |
| `aux_sensor_open_warning` | 0037 | Aux sensor open circuit |
| `aux_sensor_warning` | 0038 | Aux sensor value warning |
| `aux_sensor_shutdown` | 0040 | Aux sensor value shutdown |
| `in_auto_mode` | 0041 | Controller in auto mode |
| `in_manual_mode` | 0042 | Controller in manual mode |
| `in_stop_mode` | 0043 | Controller in stop mode |
| `temp_sensor_open_shutdown` | 0044 | Temperature sensor open shutdown |
| `oil_pressure_sensor_open_shutdown` | 0045 | Oil pressure sensor open shutdown |
| `low_engine_oil_level_shutdown_in` | 0046 | Low engine oil level (digital input) |
| `aux_sensor_open_shutdown` | 0047 | Aux sensor open circuit shutdown |
| `emergency_stop_input` | 0048 | E-stop input state |
| `aux_input_1` | 0049 | Auxiliary input 1 |
| `aux_input_2` | 0050 | Auxiliary input 2 |
| `aux_input_3` | 0051 | Auxiliary input 3 |
| `aux_input_4` | 0052 | Auxiliary input 4 |
| `aux_input_5` | 0053 | Auxiliary input 5 |
| `aux_input_6` | 0054 | Auxiliary input 6 |
| `start_relay_output` | 0056 | Start relay energized |
| `fuel_relay_output` | 0057 | Fuel relay energized |
| `aux_output_1` | 0058 | Auxiliary output 1 active |
| `aux_output_2` | 0059 | Auxiliary output 2 active |
| `aux_output_3` | 0060 | Auxiliary output 3 active |
| `aux_output_4` | 0061 | Auxiliary output 4 active |
| `high_temp_shutdown` | 0064 | High temperature shutdown (analog) |
| `low_oil_pressure_shutdown` | 0065 | Low oil pressure shutdown (analog) |
| `high_temp_warning` | 0080 | High temperature warning (analog) |
| `low_oil_pressure_warning` | 0081 | Low oil pressure warning (analog) |

### Control Buttons (Write Coil — Function Code 05H)

Remote control buttons for the generator. All are optional.

```yaml
button:
  - platform: smartgen_hsc941
    smartgen_hsc941_id: genset
    engine_start:
      name: "Gen Engine Start"
    engine_stop:
      name: "Gen Engine Stop"
    engine_auto:
      name: "Gen Set Auto Mode"
    engine_manual:
      name: "Gen Set Manual Mode"
    gen_switch_on:
      name: "Gen Switch On"
    gen_switch_off:
      name: "Gen Switch Off"
    fault_reset:
      name: "Gen Fault Reset"
```

| Key | Coil Address | Description |
|-----|-------------|-------------|
| `engine_start` | 0000 | Start the engine |
| `engine_stop` | 0001 | Stop the engine |
| `engine_auto` | 0003 | Switch to auto mode |
| `engine_manual` | 0004 | Switch to manual mode |
| `gen_switch_off` | 0005 | Open generator breaker |
| `gen_switch_on` | 0006 | Close generator breaker |
| `fault_reset` | 0007 | Reset active faults |

> **Note:** Remote commands are activated by writing `0xFFFF` to the coil address (per the HSC941 protocol). Each button press sends a single write command.

## Wiring

Connect the ESP32-S3 RS485 interface to the HSC941 controller's RS485 port:

```
ESP32-S3          RS485 Transceiver          HSC941
─────────         ─────────────────          ──────
GPIO17 (TX) ───── DI
GPIO18 (RX) ───── RO
                  A ────────────────────────── A
                  B ────────────────────────── B
                  GND ──────────────────────── GND
```

- If your RS485 transceiver requires manual direction control, connect the DE/RE pin and set `flow_control_pin` in the configuration.
- If your transceiver has automatic direction control, omit `flow_control_pin`.
- Two 120Ω termination resistors may be needed at each end of the RS485 bus depending on cable length and environment.

## Communication Details

- **Protocol:** Modbus RTU
- **Default baud rate:** 9600
- **Frame format:** 8 data bits, no parity, 1 stop bit (8N1)
- **Error checking:** CRC-16 (polynomial 0xA001)
- **Max slaves on bus:** 32

The component reads all data in two efficient requests per update cycle:
1. **88 coils** (addresses 0–87) via Function Code 01H — all alarms, warnings, shutdowns, mode status, I/O
2. **59 holding registers** (addresses 0x0007–0x0041) via Function Code 03H — all analog measurements

## License

See [LICENSE](LICENSE) for details.
