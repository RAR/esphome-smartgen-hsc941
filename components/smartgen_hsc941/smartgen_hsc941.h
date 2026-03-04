#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/button/button.h"
#include <driver/uart.h>
#include <driver/gpio.h>
#include <string>

namespace esphome {
namespace smartgen_hsc941 {

// Forward declaration
class SmartgenHSC941;

class SmartgenButton : public button::Button, public Component {
 public:
  void set_parent(SmartgenHSC941 *parent) { this->parent_ = parent; }
  void set_coil_address(uint16_t address) { this->coil_address_ = address; }
  uint16_t get_coil_address() const { return this->coil_address_; }
  void press_action() override;

 protected:
  SmartgenHSC941 *parent_{nullptr};
  uint16_t coil_address_{0};
};

class SmartgenHSC941 : public PollingComponent {
 public:
  SmartgenHSC941() = default;

  // Configuration setters
  void set_address(uint8_t address) { this->address_ = address; }
  void set_baud_rate(uint32_t baud_rate) { this->baud_rate_ = baud_rate; }
  void set_tx_pin(int tx_pin) { this->tx_pin_ = tx_pin; }
  void set_rx_pin(int rx_pin) { this->rx_pin_ = rx_pin; }
  void set_flow_control_pin(int pin) { this->flow_control_pin_ = pin; }
  void set_uart_num(int num) { this->uart_num_ = num; }

  // Component overrides
  void setup() override;
  void dump_config() override;
  void update() override;
  float get_setup_priority() const override { return setup_priority::DATA; }

  // Remote control - write coil (function code 05H)
  bool write_coil(uint16_t coil_address, bool value);

  // JSON status builder for web UI companion component
  void build_sensors_json(std::string &output);
  void build_binary_sensors_json(std::string &output);
  void build_status_json(std::string &output);

  // Communication health
  bool is_connected() const { return this->comm_failures_ < MAX_FAILURES; }

  // Engine state accessors (for exercise scheduler / companion components)
  float get_engine_rpm() const {
    return (this->engine_speed_sensor_ && this->engine_speed_sensor_->has_state())
               ? this->engine_speed_sensor_->state : 0.0f;
  }
  bool is_engine_running() const { return this->get_engine_rpm() > 100.0f; }
  bool is_crank_failure() const {
    return (this->crank_failure_shutdown_bs_ && this->crank_failure_shutdown_bs_->state);
  }
  bool is_any_shutdown() const {
    return (this->common_shutdown_bs_ && this->common_shutdown_bs_->state);
  }
  bool is_start_relay_active() const {
    return (this->start_relay_output_bs_ && this->start_relay_output_bs_->state);
  }
  bool is_in_auto_mode() const {
    return (this->in_auto_mode_bs_ && this->in_auto_mode_bs_->state);
  }
  bool is_in_manual_mode() const {
    return (this->in_manual_mode_bs_ && this->in_manual_mode_bs_->state);
  }
  bool is_in_stop_mode() const {
    return (this->in_stop_mode_bs_ && this->in_stop_mode_bs_->state);
  }
  bool is_emergency_stop() const {
    return (this->emergency_stop_bs_ && this->emergency_stop_bs_->state) ||
           (this->emergency_stop_input_bs_ && this->emergency_stop_input_bs_->state);
  }
  bool is_gen_on_load() const {
    return (this->gen_on_load_bs_ && this->gen_on_load_bs_->state);
  }

  // Temperature sensor accessors (for companion components)
  sensor::Sensor *get_water_temp_sensor() const { return this->water_temp_sensor_; }
  sensor::Sensor *get_aux_sensor_1() const { return this->aux_sensor_1_sensor_; }
  sensor::Sensor *get_total_hours_sensor() const { return this->engine_total_hours_sensor_; }
  sensor::Sensor *get_battery_voltage_sensor() const { return this->battery_voltage_sensor_; }
  sensor::Sensor *get_output_load_percent_sensor() const { return this->output_load_percent_sensor_; }
  sensor::Sensor *get_engine_running_status_sensor() const { return this->engine_running_status_sensor_; }

  // ===== SENSOR SETTERS (Function Code 03H - Holding Registers) =====
  void set_gen_voltage_a_sensor(sensor::Sensor *s) { this->gen_voltage_a_sensor_ = s; }
  void set_gen_voltage_b_sensor(sensor::Sensor *s) { this->gen_voltage_b_sensor_ = s; }
  void set_gen_voltage_c_sensor(sensor::Sensor *s) { this->gen_voltage_c_sensor_ = s; }
  void set_gen_voltage_ab_sensor(sensor::Sensor *s) { this->gen_voltage_ab_sensor_ = s; }
  void set_gen_voltage_bc_sensor(sensor::Sensor *s) { this->gen_voltage_bc_sensor_ = s; }
  void set_gen_voltage_ca_sensor(sensor::Sensor *s) { this->gen_voltage_ca_sensor_ = s; }
  void set_gen_frequency_sensor(sensor::Sensor *s) { this->gen_frequency_sensor_ = s; }
  void set_phase_a_current_sensor(sensor::Sensor *s) { this->phase_a_current_sensor_ = s; }
  void set_phase_b_current_sensor(sensor::Sensor *s) { this->phase_b_current_sensor_ = s; }
  void set_phase_c_current_sensor(sensor::Sensor *s) { this->phase_c_current_sensor_ = s; }
  void set_water_temp_sensor(sensor::Sensor *s) { this->water_temp_sensor_ = s; }
  void set_water_temp_resistance_sensor(sensor::Sensor *s) { this->water_temp_resistance_sensor_ = s; }
  void set_oil_pressure_sensor(sensor::Sensor *s) { this->oil_pressure_sensor_ = s; }
  void set_oil_pressure_resistance_sensor(sensor::Sensor *s) { this->oil_pressure_resistance_sensor_ = s; }
  void set_aux_sensor_1_sensor(sensor::Sensor *s) { this->aux_sensor_1_sensor_ = s; }
  void set_aux_sensor_1_resistance_sensor(sensor::Sensor *s) { this->aux_sensor_1_resistance_sensor_ = s; }
  void set_engine_speed_sensor(sensor::Sensor *s) { this->engine_speed_sensor_ = s; }
  void set_battery_voltage_sensor(sensor::Sensor *s) { this->battery_voltage_sensor_ = s; }
  void set_charge_voltage_sensor(sensor::Sensor *s) { this->charge_voltage_sensor_ = s; }
  void set_controller_year_sensor(sensor::Sensor *s) { this->controller_year_sensor_ = s; }
  void set_controller_month_sensor(sensor::Sensor *s) { this->controller_month_sensor_ = s; }
  void set_controller_day_sensor(sensor::Sensor *s) { this->controller_day_sensor_ = s; }
  void set_controller_weekday_sensor(sensor::Sensor *s) { this->controller_weekday_sensor_ = s; }
  void set_controller_hour_sensor(sensor::Sensor *s) { this->controller_hour_sensor_ = s; }
  void set_controller_minute_sensor(sensor::Sensor *s) { this->controller_minute_sensor_ = s; }
  void set_controller_second_sensor(sensor::Sensor *s) { this->controller_second_sensor_ = s; }
  void set_engine_running_status_sensor(sensor::Sensor *s) { this->engine_running_status_sensor_ = s; }
  void set_engine_delay_sensor(sensor::Sensor *s) { this->engine_delay_sensor_ = s; }
  void set_auto_running_status_sensor(sensor::Sensor *s) { this->auto_running_status_sensor_ = s; }
  void set_auto_delay_sensor(sensor::Sensor *s) { this->auto_delay_sensor_ = s; }
  void set_engine_total_hours_sensor(sensor::Sensor *s) { this->engine_total_hours_sensor_ = s; }
  void set_engine_total_minutes_sensor(sensor::Sensor *s) { this->engine_total_minutes_sensor_ = s; }
  void set_engine_total_seconds_sensor(sensor::Sensor *s) { this->engine_total_seconds_sensor_ = s; }
  void set_total_start_times_sensor(sensor::Sensor *s) { this->total_start_times_sensor_ = s; }
  void set_total_energy_sensor(sensor::Sensor *s) { this->total_energy_sensor_ = s; }
  void set_software_version_sensor(sensor::Sensor *s) { this->software_version_sensor_ = s; }
  void set_hardware_version_sensor(sensor::Sensor *s) { this->hardware_version_sensor_ = s; }
  void set_phase_a_angle_sensor(sensor::Sensor *s) { this->phase_a_angle_sensor_ = s; }
  void set_phase_b_angle_sensor(sensor::Sensor *s) { this->phase_b_angle_sensor_ = s; }
  void set_phase_c_angle_sensor(sensor::Sensor *s) { this->phase_c_angle_sensor_ = s; }
  void set_phase_a_active_power_sensor(sensor::Sensor *s) { this->phase_a_active_power_sensor_ = s; }
  void set_phase_b_active_power_sensor(sensor::Sensor *s) { this->phase_b_active_power_sensor_ = s; }
  void set_phase_c_active_power_sensor(sensor::Sensor *s) { this->phase_c_active_power_sensor_ = s; }
  void set_total_active_power_sensor(sensor::Sensor *s) { this->total_active_power_sensor_ = s; }
  void set_reactive_power_sensor(sensor::Sensor *s) { this->reactive_power_sensor_ = s; }
  void set_apparent_power_sensor(sensor::Sensor *s) { this->apparent_power_sensor_ = s; }
  void set_power_factor_sensor(sensor::Sensor *s) { this->power_factor_sensor_ = s; }
  void set_output_load_percent_sensor(sensor::Sensor *s) { this->output_load_percent_sensor_ = s; }
  void set_release_year_sensor(sensor::Sensor *s) { this->release_year_sensor_ = s; }
  void set_release_month_sensor(sensor::Sensor *s) { this->release_month_sensor_ = s; }
  void set_release_day_sensor(sensor::Sensor *s) { this->release_day_sensor_ = s; }

  // ===== BINARY SENSOR SETTERS (Function Code 01H - Coils) =====
  void set_common_alarm_binary_sensor(binary_sensor::BinarySensor *s) { this->common_alarm_bs_ = s; }
  void set_common_warning_binary_sensor(binary_sensor::BinarySensor *s) { this->common_warning_bs_ = s; }
  void set_common_shutdown_binary_sensor(binary_sensor::BinarySensor *s) { this->common_shutdown_bs_ = s; }
  void set_gen_on_load_binary_sensor(binary_sensor::BinarySensor *s) { this->gen_on_load_bs_ = s; }
  void set_emergency_stop_binary_sensor(binary_sensor::BinarySensor *s) { this->emergency_stop_bs_ = s; }
  void set_overspeed_shutdown_binary_sensor(binary_sensor::BinarySensor *s) { this->overspeed_shutdown_bs_ = s; }
  void set_underspeed_shutdown_binary_sensor(binary_sensor::BinarySensor *s) { this->underspeed_shutdown_bs_ = s; }
  void set_loss_speed_signal_shutdown_binary_sensor(binary_sensor::BinarySensor *s) { this->loss_speed_signal_shutdown_bs_ = s; }
  void set_gen_overfrequency_shutdown_binary_sensor(binary_sensor::BinarySensor *s) { this->gen_overfrequency_shutdown_bs_ = s; }
  void set_gen_underfrequency_shutdown_binary_sensor(binary_sensor::BinarySensor *s) { this->gen_underfrequency_shutdown_bs_ = s; }
  void set_gen_overvoltage_shutdown_binary_sensor(binary_sensor::BinarySensor *s) { this->gen_overvoltage_shutdown_bs_ = s; }
  void set_gen_undervoltage_shutdown_binary_sensor(binary_sensor::BinarySensor *s) { this->gen_undervoltage_shutdown_bs_ = s; }
  void set_gen_overcurrent_shutdown_binary_sensor(binary_sensor::BinarySensor *s) { this->gen_overcurrent_shutdown_bs_ = s; }
  void set_crank_failure_shutdown_binary_sensor(binary_sensor::BinarySensor *s) { this->crank_failure_shutdown_bs_ = s; }
  void set_high_temp_shutdown_in_binary_sensor(binary_sensor::BinarySensor *s) { this->high_temp_shutdown_in_bs_ = s; }
  void set_low_op_shutdown_in_binary_sensor(binary_sensor::BinarySensor *s) { this->low_op_shutdown_in_bs_ = s; }
  void set_no_gen_shutdown_binary_sensor(binary_sensor::BinarySensor *s) { this->no_gen_shutdown_bs_ = s; }
  void set_external_shutdown_input_binary_sensor(binary_sensor::BinarySensor *s) { this->external_shutdown_input_bs_ = s; }
  void set_low_fuel_level_shutdown_in_binary_sensor(binary_sensor::BinarySensor *s) { this->low_fuel_level_shutdown_in_bs_ = s; }
  void set_low_coolant_level_shutdown_in_binary_sensor(binary_sensor::BinarySensor *s) { this->low_coolant_level_shutdown_in_bs_ = s; }
  void set_high_temp_warning_in_binary_sensor(binary_sensor::BinarySensor *s) { this->high_temp_warning_in_bs_ = s; }
  void set_low_op_warning_in_binary_sensor(binary_sensor::BinarySensor *s) { this->low_op_warning_in_bs_ = s; }
  void set_stop_failure_warning_binary_sensor(binary_sensor::BinarySensor *s) { this->stop_failure_warning_bs_ = s; }
  void set_low_fuel_level_warning_in_binary_sensor(binary_sensor::BinarySensor *s) { this->low_fuel_level_warning_in_bs_ = s; }
  void set_charging_failure_warning_binary_sensor(binary_sensor::BinarySensor *s) { this->charging_failure_warning_bs_ = s; }
  void set_battery_undervoltage_warning_binary_sensor(binary_sensor::BinarySensor *s) { this->battery_undervoltage_warning_bs_ = s; }
  void set_battery_overvoltage_warning_binary_sensor(binary_sensor::BinarySensor *s) { this->battery_overvoltage_warning_bs_ = s; }
  void set_external_warning_input_binary_sensor(binary_sensor::BinarySensor *s) { this->external_warning_input_bs_ = s; }
  void set_loss_speed_signal_warning_binary_sensor(binary_sensor::BinarySensor *s) { this->loss_speed_signal_warning_bs_ = s; }
  void set_low_coolant_level_warning_in_binary_sensor(binary_sensor::BinarySensor *s) { this->low_coolant_level_warning_in_bs_ = s; }
  void set_temp_sensor_open_warning_binary_sensor(binary_sensor::BinarySensor *s) { this->temp_sensor_open_warning_bs_ = s; }
  void set_op_sensor_open_warning_binary_sensor(binary_sensor::BinarySensor *s) { this->op_sensor_open_warning_bs_ = s; }
  void set_aux_sensor_open_warning_binary_sensor(binary_sensor::BinarySensor *s) { this->aux_sensor_open_warning_bs_ = s; }
  void set_aux_sensor_warning_binary_sensor(binary_sensor::BinarySensor *s) { this->aux_sensor_warning_bs_ = s; }
  void set_aux_sensor_shutdown_binary_sensor(binary_sensor::BinarySensor *s) { this->aux_sensor_shutdown_bs_ = s; }
  void set_in_auto_mode_binary_sensor(binary_sensor::BinarySensor *s) { this->in_auto_mode_bs_ = s; }
  void set_in_manual_mode_binary_sensor(binary_sensor::BinarySensor *s) { this->in_manual_mode_bs_ = s; }
  void set_in_stop_mode_binary_sensor(binary_sensor::BinarySensor *s) { this->in_stop_mode_bs_ = s; }
  void set_temp_sensor_open_shutdown_binary_sensor(binary_sensor::BinarySensor *s) { this->temp_sensor_open_shutdown_bs_ = s; }
  void set_op_sensor_open_shutdown_binary_sensor(binary_sensor::BinarySensor *s) { this->op_sensor_open_shutdown_bs_ = s; }
  void set_low_engine_oil_level_shutdown_in_binary_sensor(binary_sensor::BinarySensor *s) { this->low_engine_oil_level_shutdown_in_bs_ = s; }
  void set_aux_sensor_open_shutdown_binary_sensor(binary_sensor::BinarySensor *s) { this->aux_sensor_open_shutdown_bs_ = s; }
  void set_emergency_stop_input_binary_sensor(binary_sensor::BinarySensor *s) { this->emergency_stop_input_bs_ = s; }
  void set_aux_input_1_binary_sensor(binary_sensor::BinarySensor *s) { this->aux_input_1_bs_ = s; }
  void set_aux_input_2_binary_sensor(binary_sensor::BinarySensor *s) { this->aux_input_2_bs_ = s; }
  void set_aux_input_3_binary_sensor(binary_sensor::BinarySensor *s) { this->aux_input_3_bs_ = s; }
  void set_aux_input_4_binary_sensor(binary_sensor::BinarySensor *s) { this->aux_input_4_bs_ = s; }
  void set_aux_input_5_binary_sensor(binary_sensor::BinarySensor *s) { this->aux_input_5_bs_ = s; }
  void set_aux_input_6_binary_sensor(binary_sensor::BinarySensor *s) { this->aux_input_6_bs_ = s; }
  void set_start_relay_output_binary_sensor(binary_sensor::BinarySensor *s) { this->start_relay_output_bs_ = s; }
  void set_fuel_relay_output_binary_sensor(binary_sensor::BinarySensor *s) { this->fuel_relay_output_bs_ = s; }
  void set_aux_output_1_binary_sensor(binary_sensor::BinarySensor *s) { this->aux_output_1_bs_ = s; }
  void set_aux_output_2_binary_sensor(binary_sensor::BinarySensor *s) { this->aux_output_2_bs_ = s; }
  void set_aux_output_3_binary_sensor(binary_sensor::BinarySensor *s) { this->aux_output_3_bs_ = s; }
  void set_aux_output_4_binary_sensor(binary_sensor::BinarySensor *s) { this->aux_output_4_bs_ = s; }
  void set_high_temp_shutdown_binary_sensor(binary_sensor::BinarySensor *s) { this->high_temp_shutdown_bs_ = s; }
  void set_low_op_shutdown_binary_sensor(binary_sensor::BinarySensor *s) { this->low_op_shutdown_bs_ = s; }
  void set_high_temp_warning_binary_sensor(binary_sensor::BinarySensor *s) { this->high_temp_warning_bs_ = s; }
  void set_low_op_warning_binary_sensor(binary_sensor::BinarySensor *s) { this->low_op_warning_bs_ = s; }

  // ===== BUTTON SETTERS =====
  void set_engine_start_button(SmartgenButton *b) { this->engine_start_button_ = b; }
  void set_engine_stop_button(SmartgenButton *b) { this->engine_stop_button_ = b; }
  void set_engine_auto_button(SmartgenButton *b) { this->engine_auto_button_ = b; }
  void set_engine_manual_button(SmartgenButton *b) { this->engine_manual_button_ = b; }
  void set_gen_switch_off_button(SmartgenButton *b) { this->gen_switch_off_button_ = b; }
  void set_gen_switch_on_button(SmartgenButton *b) { this->gen_switch_on_button_ = b; }
  void set_fault_reset_button(SmartgenButton *b) { this->fault_reset_button_ = b; }

 protected:
  // UART / RS485 configuration
  uint8_t address_{1};
  uint32_t baud_rate_{9600};
  int tx_pin_{-1};
  int rx_pin_{-1};
  int flow_control_pin_{-1};
  int uart_num_{1};
  uart_port_t uart_port_{UART_NUM_1};

  // Communication helpers
  void init_uart_();
  bool send_and_receive_(uint8_t *request, size_t req_len, uint8_t *response, size_t *resp_len, size_t expected_len);
  bool read_coils_(uint16_t start_address, uint16_t quantity, uint8_t *coil_data, size_t *data_len);
  bool read_holding_registers_(uint16_t start_address, uint16_t quantity, uint16_t *reg_data);
  void set_flow_control_(bool transmit);

  // CRC-16 Modbus
  static uint16_t crc16_(const uint8_t *data, size_t length);

  // Data processing
  void process_coil_data_(const uint8_t *data, size_t data_len, uint16_t start_address, uint16_t quantity);
  void process_register_data_(const uint16_t *data, uint16_t start_address, uint16_t quantity);

  // Helper to publish binary sensor from coil bit
  void publish_coil_bit_(binary_sensor::BinarySensor *bs, const uint8_t *data, uint16_t bit_offset);

  // Communication state
  uint8_t comm_failures_{0};
  static const uint8_t MAX_FAILURES = 5;

  // ===== SENSOR POINTERS =====
  sensor::Sensor *gen_voltage_a_sensor_{nullptr};
  sensor::Sensor *gen_voltage_b_sensor_{nullptr};
  sensor::Sensor *gen_voltage_c_sensor_{nullptr};
  sensor::Sensor *gen_voltage_ab_sensor_{nullptr};
  sensor::Sensor *gen_voltage_bc_sensor_{nullptr};
  sensor::Sensor *gen_voltage_ca_sensor_{nullptr};
  sensor::Sensor *gen_frequency_sensor_{nullptr};
  sensor::Sensor *phase_a_current_sensor_{nullptr};
  sensor::Sensor *phase_b_current_sensor_{nullptr};
  sensor::Sensor *phase_c_current_sensor_{nullptr};
  sensor::Sensor *water_temp_sensor_{nullptr};
  sensor::Sensor *water_temp_resistance_sensor_{nullptr};
  sensor::Sensor *oil_pressure_sensor_{nullptr};
  sensor::Sensor *oil_pressure_resistance_sensor_{nullptr};
  sensor::Sensor *aux_sensor_1_sensor_{nullptr};
  sensor::Sensor *aux_sensor_1_resistance_sensor_{nullptr};
  sensor::Sensor *engine_speed_sensor_{nullptr};
  sensor::Sensor *battery_voltage_sensor_{nullptr};
  sensor::Sensor *charge_voltage_sensor_{nullptr};
  sensor::Sensor *controller_year_sensor_{nullptr};
  sensor::Sensor *controller_month_sensor_{nullptr};
  sensor::Sensor *controller_day_sensor_{nullptr};
  sensor::Sensor *controller_weekday_sensor_{nullptr};
  sensor::Sensor *controller_hour_sensor_{nullptr};
  sensor::Sensor *controller_minute_sensor_{nullptr};
  sensor::Sensor *controller_second_sensor_{nullptr};
  sensor::Sensor *engine_running_status_sensor_{nullptr};
  sensor::Sensor *engine_delay_sensor_{nullptr};
  sensor::Sensor *auto_running_status_sensor_{nullptr};
  sensor::Sensor *auto_delay_sensor_{nullptr};
  sensor::Sensor *engine_total_hours_sensor_{nullptr};
  sensor::Sensor *engine_total_minutes_sensor_{nullptr};
  sensor::Sensor *engine_total_seconds_sensor_{nullptr};
  sensor::Sensor *total_start_times_sensor_{nullptr};
  sensor::Sensor *total_energy_sensor_{nullptr};
  sensor::Sensor *software_version_sensor_{nullptr};
  sensor::Sensor *hardware_version_sensor_{nullptr};
  sensor::Sensor *phase_a_angle_sensor_{nullptr};
  sensor::Sensor *phase_b_angle_sensor_{nullptr};
  sensor::Sensor *phase_c_angle_sensor_{nullptr};
  sensor::Sensor *phase_a_active_power_sensor_{nullptr};
  sensor::Sensor *phase_b_active_power_sensor_{nullptr};
  sensor::Sensor *phase_c_active_power_sensor_{nullptr};
  sensor::Sensor *total_active_power_sensor_{nullptr};
  sensor::Sensor *reactive_power_sensor_{nullptr};
  sensor::Sensor *apparent_power_sensor_{nullptr};
  sensor::Sensor *power_factor_sensor_{nullptr};
  sensor::Sensor *output_load_percent_sensor_{nullptr};
  sensor::Sensor *release_year_sensor_{nullptr};
  sensor::Sensor *release_month_sensor_{nullptr};
  sensor::Sensor *release_day_sensor_{nullptr};

  // ===== BINARY SENSOR POINTERS =====
  binary_sensor::BinarySensor *common_alarm_bs_{nullptr};
  binary_sensor::BinarySensor *common_warning_bs_{nullptr};
  binary_sensor::BinarySensor *common_shutdown_bs_{nullptr};
  binary_sensor::BinarySensor *gen_on_load_bs_{nullptr};
  binary_sensor::BinarySensor *emergency_stop_bs_{nullptr};
  binary_sensor::BinarySensor *overspeed_shutdown_bs_{nullptr};
  binary_sensor::BinarySensor *underspeed_shutdown_bs_{nullptr};
  binary_sensor::BinarySensor *loss_speed_signal_shutdown_bs_{nullptr};
  binary_sensor::BinarySensor *gen_overfrequency_shutdown_bs_{nullptr};
  binary_sensor::BinarySensor *gen_underfrequency_shutdown_bs_{nullptr};
  binary_sensor::BinarySensor *gen_overvoltage_shutdown_bs_{nullptr};
  binary_sensor::BinarySensor *gen_undervoltage_shutdown_bs_{nullptr};
  binary_sensor::BinarySensor *gen_overcurrent_shutdown_bs_{nullptr};
  binary_sensor::BinarySensor *crank_failure_shutdown_bs_{nullptr};
  binary_sensor::BinarySensor *high_temp_shutdown_in_bs_{nullptr};
  binary_sensor::BinarySensor *low_op_shutdown_in_bs_{nullptr};
  binary_sensor::BinarySensor *no_gen_shutdown_bs_{nullptr};
  binary_sensor::BinarySensor *external_shutdown_input_bs_{nullptr};
  binary_sensor::BinarySensor *low_fuel_level_shutdown_in_bs_{nullptr};
  binary_sensor::BinarySensor *low_coolant_level_shutdown_in_bs_{nullptr};
  binary_sensor::BinarySensor *high_temp_warning_in_bs_{nullptr};
  binary_sensor::BinarySensor *low_op_warning_in_bs_{nullptr};
  binary_sensor::BinarySensor *stop_failure_warning_bs_{nullptr};
  binary_sensor::BinarySensor *low_fuel_level_warning_in_bs_{nullptr};
  binary_sensor::BinarySensor *charging_failure_warning_bs_{nullptr};
  binary_sensor::BinarySensor *battery_undervoltage_warning_bs_{nullptr};
  binary_sensor::BinarySensor *battery_overvoltage_warning_bs_{nullptr};
  binary_sensor::BinarySensor *external_warning_input_bs_{nullptr};
  binary_sensor::BinarySensor *loss_speed_signal_warning_bs_{nullptr};
  binary_sensor::BinarySensor *low_coolant_level_warning_in_bs_{nullptr};
  binary_sensor::BinarySensor *temp_sensor_open_warning_bs_{nullptr};
  binary_sensor::BinarySensor *op_sensor_open_warning_bs_{nullptr};
  binary_sensor::BinarySensor *aux_sensor_open_warning_bs_{nullptr};
  binary_sensor::BinarySensor *aux_sensor_warning_bs_{nullptr};
  binary_sensor::BinarySensor *aux_sensor_shutdown_bs_{nullptr};
  binary_sensor::BinarySensor *in_auto_mode_bs_{nullptr};
  binary_sensor::BinarySensor *in_manual_mode_bs_{nullptr};
  binary_sensor::BinarySensor *in_stop_mode_bs_{nullptr};
  binary_sensor::BinarySensor *temp_sensor_open_shutdown_bs_{nullptr};
  binary_sensor::BinarySensor *op_sensor_open_shutdown_bs_{nullptr};
  binary_sensor::BinarySensor *low_engine_oil_level_shutdown_in_bs_{nullptr};
  binary_sensor::BinarySensor *aux_sensor_open_shutdown_bs_{nullptr};
  binary_sensor::BinarySensor *emergency_stop_input_bs_{nullptr};
  binary_sensor::BinarySensor *aux_input_1_bs_{nullptr};
  binary_sensor::BinarySensor *aux_input_2_bs_{nullptr};
  binary_sensor::BinarySensor *aux_input_3_bs_{nullptr};
  binary_sensor::BinarySensor *aux_input_4_bs_{nullptr};
  binary_sensor::BinarySensor *aux_input_5_bs_{nullptr};
  binary_sensor::BinarySensor *aux_input_6_bs_{nullptr};
  binary_sensor::BinarySensor *start_relay_output_bs_{nullptr};
  binary_sensor::BinarySensor *fuel_relay_output_bs_{nullptr};
  binary_sensor::BinarySensor *aux_output_1_bs_{nullptr};
  binary_sensor::BinarySensor *aux_output_2_bs_{nullptr};
  binary_sensor::BinarySensor *aux_output_3_bs_{nullptr};
  binary_sensor::BinarySensor *aux_output_4_bs_{nullptr};
  binary_sensor::BinarySensor *high_temp_shutdown_bs_{nullptr};
  binary_sensor::BinarySensor *low_op_shutdown_bs_{nullptr};
  binary_sensor::BinarySensor *high_temp_warning_bs_{nullptr};
  binary_sensor::BinarySensor *low_op_warning_bs_{nullptr};

  // ===== BUTTON POINTERS =====
  SmartgenButton *engine_start_button_{nullptr};
  SmartgenButton *engine_stop_button_{nullptr};
  SmartgenButton *engine_auto_button_{nullptr};
  SmartgenButton *engine_manual_button_{nullptr};
  SmartgenButton *gen_switch_off_button_{nullptr};
  SmartgenButton *gen_switch_on_button_{nullptr};
  SmartgenButton *fault_reset_button_{nullptr};
};

}  // namespace smartgen_hsc941
}  // namespace esphome
