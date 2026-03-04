#include "smartgen_hsc941.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"
#include "esphome/core/hal.h"
#include <cstring>
#include <esp_timer.h>
#include <esp_rom_sys.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace esphome {
namespace smartgen_hsc941 {

static const char *const TAG = "smartgen_hsc941";

// Modbus function codes
static const uint8_t FC_READ_COILS = 0x01;
static const uint8_t FC_READ_HOLDING_REGISTERS = 0x03;
static const uint8_t FC_WRITE_SINGLE_COIL = 0x05;

// Timing constants
static const uint32_t RESPONSE_TIMEOUT_MS = 500;
static const uint32_t INTER_FRAME_DELAY_MS = 5;
static const uint32_t TX_SETTLE_US = 200;

// ============================================================
//  SmartgenButton implementation
// ============================================================
void SmartgenButton::press_action() {
  if (this->parent_ != nullptr) {
    ESP_LOGI(TAG, "Button pressed: writing coil %u = ON", this->coil_address_);
    this->parent_->write_coil(this->coil_address_, true);
  }
}

// ============================================================
//  CRC-16 (Modbus)
// ============================================================
uint16_t SmartgenHSC941::crc16_(const uint8_t *data, size_t length) {
  uint16_t crc = 0xFFFF;
  for (size_t i = 0; i < length; i++) {
    crc ^= (uint16_t) data[i];
    for (int j = 0; j < 8; j++) {
      if (crc & 0x0001) {
        crc >>= 1;
        crc ^= 0xA001;
      } else {
        crc >>= 1;
      }
    }
  }
  return crc;
}

// ============================================================
//  UART Initialization
// ============================================================
void SmartgenHSC941::init_uart_() {
  this->uart_port_ = static_cast<uart_port_t>(this->uart_num_);

  uart_config_t uart_config = {};
  uart_config.baud_rate = (int) this->baud_rate_;
  uart_config.data_bits = UART_DATA_8_BITS;
  uart_config.parity = UART_PARITY_DISABLE;
  uart_config.stop_bits = UART_STOP_BITS_1;
  uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE;
  uart_config.rx_flow_ctrl_thresh = 0;
  uart_config.source_clk = UART_SCLK_DEFAULT;

  ESP_LOGD(TAG, "Configuring UART%d: TX=%d, RX=%d, baud=%u",
           this->uart_num_, this->tx_pin_, this->rx_pin_, this->baud_rate_);

  esp_err_t err;

  err = uart_param_config(this->uart_port_, &uart_config);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "uart_param_config failed: %s", esp_err_to_name(err));
    this->mark_failed();
    return;
  }

  err = uart_set_pin(this->uart_port_, this->tx_pin_, this->rx_pin_,
                     UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "uart_set_pin failed: %s", esp_err_to_name(err));
    this->mark_failed();
    return;
  }

  err = uart_driver_install(this->uart_port_, 256, 256, 0, nullptr, 0);
  if (err != ESP_OK) {
    ESP_LOGE(TAG, "uart_driver_install failed: %s", esp_err_to_name(err));
    this->mark_failed();
    return;
  }

  // Configure flow control pin if specified
  if (this->flow_control_pin_ >= 0) {
    gpio_config_t io_conf = {};
    io_conf.pin_bit_mask = (1ULL << this->flow_control_pin_);
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);
    this->set_flow_control_(false);  // Start in receive mode
  }

  ESP_LOGI(TAG, "UART%d initialized successfully", this->uart_num_);
}

// ============================================================
//  Flow Control (RS485 direction)
// ============================================================
void SmartgenHSC941::set_flow_control_(bool transmit) {
  if (this->flow_control_pin_ >= 0) {
    gpio_set_level(static_cast<gpio_num_t>(this->flow_control_pin_), transmit ? 1 : 0);
    if (transmit) {
      esp_rom_delay_us(TX_SETTLE_US);
    }
  }
}

// ============================================================
//  Component Setup
// ============================================================
void SmartgenHSC941::setup() {
  ESP_LOGCONFIG(TAG, "Setting up SmartGen HSC941...");
  this->init_uart_();
}

void SmartgenHSC941::dump_config() {
  ESP_LOGCONFIG(TAG, "SmartGen HSC941:");
  ESP_LOGCONFIG(TAG, "  Address: %u", this->address_);
  ESP_LOGCONFIG(TAG, "  Baud Rate: %u", this->baud_rate_);
  ESP_LOGCONFIG(TAG, "  TX Pin: %d", this->tx_pin_);
  ESP_LOGCONFIG(TAG, "  RX Pin: %d", this->rx_pin_);
  ESP_LOGCONFIG(TAG, "  UART Num: %d", this->uart_num_);
  if (this->flow_control_pin_ >= 0)
    ESP_LOGCONFIG(TAG, "  Flow Control Pin: %d", this->flow_control_pin_);
  else
    ESP_LOGCONFIG(TAG, "  Flow Control: Auto (no DE/RE pin)");
}

// ============================================================
//  Send/Receive Raw Modbus Frame
// ============================================================
bool SmartgenHSC941::send_and_receive_(uint8_t *request, size_t req_len,
                                        uint8_t *response, size_t *resp_len,
                                        size_t expected_len) {
  // Flush any stale data in RX buffer
  uart_flush_input(this->uart_port_);

  // Set to transmit mode
  this->set_flow_control_(true);

  // Send request
  int written = uart_write_bytes(this->uart_port_, request, req_len);
  if (written != (int) req_len) {
    ESP_LOGW(TAG, "UART write failed: wrote %d of %zu bytes", written, req_len);
    this->set_flow_control_(false);
    return false;
  }

  // Wait for transmission to complete
  esp_err_t err = uart_wait_tx_done(this->uart_port_, pdMS_TO_TICKS(100));
  if (err != ESP_OK) {
    ESP_LOGW(TAG, "uart_wait_tx_done failed: %s", esp_err_to_name(err));
  }

  // Switch to receive mode
  this->set_flow_control_(false);

  // Small inter-frame delay
  vTaskDelay(pdMS_TO_TICKS(INTER_FRAME_DELAY_MS));

  // Read response with timeout
  size_t total_read = 0;
  uint32_t start_time = (uint32_t)(esp_timer_get_time() / 1000);

  while (total_read < expected_len) {
    if (((uint32_t)(esp_timer_get_time() / 1000)) - start_time > RESPONSE_TIMEOUT_MS) {
      ESP_LOGW(TAG, "Response timeout after %zu bytes (expected %zu)", total_read, expected_len);
      *resp_len = total_read;
      return false;
    }

    int available = 0;
    uart_get_buffered_data_len(this->uart_port_, (size_t *) &available);

    if (available > 0) {
      size_t to_read = std::min((size_t) available, expected_len - total_read);
      int read = uart_read_bytes(this->uart_port_, response + total_read, to_read, pdMS_TO_TICKS(50));
      if (read > 0) {
        total_read += read;
      }
    } else {
      vTaskDelay(pdMS_TO_TICKS(1));
    }
  }

  *resp_len = total_read;

  // Verify CRC
  if (total_read >= 4) {
    uint16_t received_crc = (uint16_t)(response[total_read - 1] << 8) | response[total_read - 2];
    uint16_t calculated_crc = crc16_(response, total_read - 2);
    if (received_crc != calculated_crc) {
      ESP_LOGW(TAG, "CRC mismatch: received=0x%04X, calculated=0x%04X", received_crc, calculated_crc);
      return false;
    }
  }

  // Verify slave address
  if (response[0] != this->address_) {
    ESP_LOGW(TAG, "Address mismatch: expected %u, got %u", this->address_, response[0]);
    return false;
  }

  return true;
}

// ============================================================
//  Read Coils (Function Code 01H)
// ============================================================
bool SmartgenHSC941::read_coils_(uint16_t start_address, uint16_t quantity,
                                  uint8_t *coil_data, size_t *data_len) {
  uint8_t request[8];
  request[0] = this->address_;
  request[1] = FC_READ_COILS;
  request[2] = (start_address >> 8) & 0xFF;
  request[3] = start_address & 0xFF;
  request[4] = (quantity >> 8) & 0xFF;
  request[5] = quantity & 0xFF;
  uint16_t crc = crc16_(request, 6);
  request[6] = crc & 0xFF;        // CRC LSB
  request[7] = (crc >> 8) & 0xFF; // CRC MSB

  // Response: address(1) + function(1) + byte_count(1) + data(N) + crc(2)
  uint8_t byte_count = (quantity + 7) / 8;
  size_t expected_len = 3 + byte_count + 2;
  uint8_t response[64];
  size_t resp_len = 0;

  if (!this->send_and_receive_(request, 8, response, &resp_len, expected_len)) {
    return false;
  }

  // Verify function code
  if (response[1] != FC_READ_COILS) {
    ESP_LOGW(TAG, "Unexpected function code in coil response: 0x%02X", response[1]);
    return false;
  }

  // Copy coil data
  *data_len = response[2];
  memcpy(coil_data, &response[3], *data_len);

  return true;
}

// ============================================================
//  Read Holding Registers (Function Code 03H)
// ============================================================
bool SmartgenHSC941::read_holding_registers_(uint16_t start_address, uint16_t quantity,
                                              uint16_t *reg_data) {
  uint8_t request[8];
  request[0] = this->address_;
  request[1] = FC_READ_HOLDING_REGISTERS;
  request[2] = (start_address >> 8) & 0xFF;
  request[3] = start_address & 0xFF;
  request[4] = (quantity >> 8) & 0xFF;
  request[5] = quantity & 0xFF;
  uint16_t crc = crc16_(request, 6);
  request[6] = crc & 0xFF;
  request[7] = (crc >> 8) & 0xFF;

  // Response: address(1) + function(1) + byte_count(1) + data(2*N) + crc(2)
  size_t expected_len = 3 + (quantity * 2) + 2;
  uint8_t response[256];
  size_t resp_len = 0;

  if (!this->send_and_receive_(request, 8, response, &resp_len, expected_len)) {
    return false;
  }

  if (response[1] != FC_READ_HOLDING_REGISTERS) {
    ESP_LOGW(TAG, "Unexpected function code in register response: 0x%02X", response[1]);
    return false;
  }

  // Parse register data (big-endian)
  for (uint16_t i = 0; i < quantity; i++) {
    reg_data[i] = ((uint16_t) response[3 + i * 2] << 8) | response[3 + i * 2 + 1];
  }

  return true;
}

// ============================================================
//  Write Single Coil (Function Code 05H)
// ============================================================
bool SmartgenHSC941::write_coil(uint16_t coil_address, bool value) {
  uint8_t request[8];
  request[0] = this->address_;
  request[1] = FC_WRITE_SINGLE_COIL;
  request[2] = (coil_address >> 8) & 0xFF;
  request[3] = coil_address & 0xFF;
  request[4] = value ? 0xFF : 0x00;
  request[5] = value ? 0xFF : 0x00;
  uint16_t crc = crc16_(request, 6);
  request[6] = crc & 0xFF;
  request[7] = (crc >> 8) & 0xFF;

  ESP_LOGD(TAG, "Write coil %u = %s (frame: %02X %02X %02X %02X %02X %02X %02X %02X)",
           coil_address, value ? "ON" : "OFF",
           request[0], request[1], request[2], request[3],
           request[4], request[5], request[6], request[7]);

  // Response is echo of request (8 bytes)
  uint8_t response[8];
  size_t resp_len = 0;

  bool ok = this->send_and_receive_(request, 8, response, &resp_len, 8);

  if (ok) {
    ESP_LOGI(TAG, "Coil %u write successful", coil_address);
  } else {
    ESP_LOGW(TAG, "Coil %u write failed", coil_address);
  }

  return ok;
}

// ============================================================
//  Publish Coil Bit to Binary Sensor
// ============================================================
void SmartgenHSC941::publish_coil_bit_(binary_sensor::BinarySensor *bs,
                                        const uint8_t *data,
                                        uint16_t bit_offset) {
  if (bs == nullptr)
    return;
  uint16_t byte_idx = bit_offset / 8;
  uint8_t bit_idx = bit_offset % 8;
  bool value = (data[byte_idx] >> bit_idx) & 0x01;
  bs->publish_state(value);
}

// ============================================================
//  Process Coil Data (Function Code 01H results)
// ============================================================
void SmartgenHSC941::process_coil_data_(const uint8_t *data, size_t data_len,
                                         uint16_t start_address, uint16_t quantity) {
  // Map coil addresses to binary sensors
  // Each bit in data corresponds to a coil, starting from start_address
  // Bit 0 of byte 0 = start_address, bit 1 = start_address+1, etc.

  struct CoilMapping {
    uint16_t address;
    binary_sensor::BinarySensor *sensor;
  };

  CoilMapping mappings[] = {
    {0, this->common_alarm_bs_},
    {1, this->common_warning_bs_},
    {2, this->common_shutdown_bs_},
    {7, this->gen_on_load_bs_},
    {8, this->emergency_stop_bs_},
    {9, this->overspeed_shutdown_bs_},
    {10, this->underspeed_shutdown_bs_},
    {11, this->loss_speed_signal_shutdown_bs_},
    {12, this->gen_overfrequency_shutdown_bs_},
    {13, this->gen_underfrequency_shutdown_bs_},
    {14, this->gen_overvoltage_shutdown_bs_},
    {15, this->gen_undervoltage_shutdown_bs_},
    {16, this->gen_overcurrent_shutdown_bs_},
    {17, this->crank_failure_shutdown_bs_},
    {18, this->high_temp_shutdown_in_bs_},
    {19, this->low_op_shutdown_in_bs_},
    {20, this->no_gen_shutdown_bs_},
    {21, this->external_shutdown_input_bs_},
    {22, this->low_fuel_level_shutdown_in_bs_},
    {23, this->low_coolant_level_shutdown_in_bs_},
    {24, this->high_temp_warning_in_bs_},
    {25, this->low_op_warning_in_bs_},
    {27, this->stop_failure_warning_bs_},
    {28, this->low_fuel_level_warning_in_bs_},
    {29, this->charging_failure_warning_bs_},
    {30, this->battery_undervoltage_warning_bs_},
    {31, this->battery_overvoltage_warning_bs_},
    {32, this->external_warning_input_bs_},
    {33, this->loss_speed_signal_warning_bs_},
    {34, this->low_coolant_level_warning_in_bs_},
    {35, this->temp_sensor_open_warning_bs_},
    {36, this->op_sensor_open_warning_bs_},
    {37, this->aux_sensor_open_warning_bs_},
    {38, this->aux_sensor_warning_bs_},
    {40, this->aux_sensor_shutdown_bs_},
    {41, this->in_auto_mode_bs_},
    {42, this->in_manual_mode_bs_},
    {43, this->in_stop_mode_bs_},
    {44, this->temp_sensor_open_shutdown_bs_},
    {45, this->op_sensor_open_shutdown_bs_},
    {46, this->low_engine_oil_level_shutdown_in_bs_},
    {47, this->aux_sensor_open_shutdown_bs_},
    {48, this->emergency_stop_input_bs_},
    {49, this->aux_input_1_bs_},
    {50, this->aux_input_2_bs_},
    {51, this->aux_input_3_bs_},
    {52, this->aux_input_4_bs_},
    {53, this->aux_input_5_bs_},
    {54, this->aux_input_6_bs_},
    {56, this->start_relay_output_bs_},
    {57, this->fuel_relay_output_bs_},
    {58, this->aux_output_1_bs_},
    {59, this->aux_output_2_bs_},
    {60, this->aux_output_3_bs_},
    {61, this->aux_output_4_bs_},
    {64, this->high_temp_shutdown_bs_},
    {65, this->low_op_shutdown_bs_},
    {80, this->high_temp_warning_bs_},
    {81, this->low_op_warning_bs_},
  };

  for (auto &m : mappings) {
    if (m.sensor == nullptr)
      continue;
    // Check if this coil address is within our read range
    if (m.address >= start_address && m.address < start_address + quantity) {
      uint16_t bit_offset = m.address - start_address;
      this->publish_coil_bit_(m.sensor, data, bit_offset);
    }
  }
}

// ============================================================
//  Process Register Data (Function Code 03H results)
// ============================================================
void SmartgenHSC941::process_register_data_(const uint16_t *data,
                                             uint16_t start_address,
                                             uint16_t quantity) {
  // Helper lambda: get register value with bounds check
  auto get_reg = [&](uint16_t addr) -> int32_t {
    if (addr < start_address || addr >= start_address + quantity)
      return -1;  // Out of range sentinel
    return data[addr - start_address];
  };

  // Helper: get register as signed 16-bit
  auto get_reg_signed = [&](uint16_t addr) -> int32_t {
    if (addr < start_address || addr >= start_address + quantity)
      return -999999;
    int16_t val = (int16_t) data[addr - start_address];
    return val;
  };

  // Helper: publish if sensor exists and value is valid
  auto pub = [](sensor::Sensor *s, float value) {
    if (s != nullptr)
      s->publish_state(value);
  };

  // SmartGen uses several sentinel values for "no data / sensor not connected":
  //   0x7FFE (32766), 0x7FFF (32767), 0xFFFF (65535)
  auto pub_if_valid = [&pub](sensor::Sensor *s, uint16_t raw, float ratio) {
    if (s == nullptr)
      return;
    if (raw == 0x7FFE || raw == 0x7FFF || raw == 0xFFFF) {
      s->publish_state(NAN);  // No valid data / sensor not connected
      return;
    }
    pub(s, (float) raw * ratio);
  };

  auto pub_signed = [&pub](sensor::Sensor *s, int16_t raw, float ratio) {
    if (s == nullptr)
      return;
    // 0x7FFE (32766), 0x7FFF (32767), 0x8000 (-32768) = sentinels
    if (raw == 32766 || raw == 32767 || raw == -32768) {
      s->publish_state(NAN);
      return;
    }
    pub(s, (float) raw * ratio);
  };

  int32_t v;

  // Register 0007: Gen. UA (ratio 1, V)
  v = get_reg(0x0007);
  if (v >= 0) pub_if_valid(this->gen_voltage_a_sensor_, (uint16_t) v, 1.0f);

  // Register 0008: Gen. UB
  v = get_reg(0x0008);
  if (v >= 0) pub_if_valid(this->gen_voltage_b_sensor_, (uint16_t) v, 1.0f);

  // Register 0009: Gen. UC
  v = get_reg(0x0009);
  if (v >= 0) pub_if_valid(this->gen_voltage_c_sensor_, (uint16_t) v, 1.0f);

  // Register 0010: Gen. UAB
  v = get_reg(0x000A);
  if (v >= 0) pub_if_valid(this->gen_voltage_ab_sensor_, (uint16_t) v, 1.0f);

  // Register 0011: Gen. UBC
  v = get_reg(0x000B);
  if (v >= 0) pub_if_valid(this->gen_voltage_bc_sensor_, (uint16_t) v, 1.0f);

  // Register 0012: Gen. UCA
  v = get_reg(0x000C);
  if (v >= 0) pub_if_valid(this->gen_voltage_ca_sensor_, (uint16_t) v, 1.0f);

  // Register 0013: Gen. Frequency (ratio 0.01 Hz)
  v = get_reg(0x000D);
  if (v >= 0) pub_if_valid(this->gen_frequency_sensor_, (uint16_t) v, 0.01f);

  // Register 0014: Phase A Current (ratio 0.1 A)
  v = get_reg(0x000E);
  if (v >= 0) pub_if_valid(this->phase_a_current_sensor_, (uint16_t) v, 0.1f);

  // Register 0015: Phase B Current
  v = get_reg(0x000F);
  if (v >= 0) pub_if_valid(this->phase_b_current_sensor_, (uint16_t) v, 0.1f);

  // Register 0016: Phase C Current
  v = get_reg(0x0010);
  if (v >= 0) pub_if_valid(this->phase_c_current_sensor_, (uint16_t) v, 0.1f);

  // Register 0017: Water Temp (ratio 1, °C)
  v = get_reg(0x0011);
  if (v >= 0) pub_if_valid(this->water_temp_sensor_, (uint16_t) v, 1.0f);

  // Register 0018: Water Temp Resistance (ratio 0.1, Ω)
  v = get_reg(0x0012);
  if (v >= 0) pub_if_valid(this->water_temp_resistance_sensor_, (uint16_t) v, 0.1f);

  // Register 0019: Oil Pressure (ratio 1, kPa)
  v = get_reg(0x0013);
  if (v >= 0) pub_if_valid(this->oil_pressure_sensor_, (uint16_t) v, 1.0f);

  // Register 0020: OP Resistance (ratio 0.1, Ω)
  v = get_reg(0x0014);
  if (v >= 0) pub_if_valid(this->oil_pressure_resistance_sensor_, (uint16_t) v, 0.1f);

  // Register 0021: Aux Sensor 1 (ratio 1)
  v = get_reg(0x0015);
  if (v >= 0) pub_if_valid(this->aux_sensor_1_sensor_, (uint16_t) v, 1.0f);

  // Register 0022: Aux Sensor 1 Resistance (ratio 0.1, Ω)
  v = get_reg(0x0016);
  if (v >= 0) pub_if_valid(this->aux_sensor_1_resistance_sensor_, (uint16_t) v, 0.1f);

  // Register 0023: Speed (ratio 1, r/min)
  v = get_reg(0x0017);
  if (v >= 0) pub_if_valid(this->engine_speed_sensor_, (uint16_t) v, 1.0f);

  // Register 0024: Battery Voltage (ratio 0.1, V)
  v = get_reg(0x0018);
  if (v >= 0) pub_if_valid(this->battery_voltage_sensor_, (uint16_t) v, 0.1f);

  // Register 0025: D+ Voltage (ratio 0.1, V)
  v = get_reg(0x0019);
  if (v >= 0) pub_if_valid(this->charge_voltage_sensor_, (uint16_t) v, 0.1f);

  // Register 0026: Controller Year
  v = get_reg(0x001A);
  if (v >= 0) pub(this->controller_year_sensor_, (float) (uint16_t) v);

  // Register 0027: Controller Month
  v = get_reg(0x001B);
  if (v >= 0) pub(this->controller_month_sensor_, (float) (uint16_t) v);

  // Register 0028: Controller Day
  v = get_reg(0x001C);
  if (v >= 0) pub(this->controller_day_sensor_, (float) (uint16_t) v);

  // Register 0029: Controller Weekday
  v = get_reg(0x001D);
  if (v >= 0) pub(this->controller_weekday_sensor_, (float) (uint16_t) v);

  // Register 0030: Controller Hour
  v = get_reg(0x001E);
  if (v >= 0) pub(this->controller_hour_sensor_, (float) (uint16_t) v);

  // Register 0031: Controller Minute
  v = get_reg(0x001F);
  if (v >= 0) pub(this->controller_minute_sensor_, (float) (uint16_t) v);

  // Register 0032: Controller Second
  v = get_reg(0x0020);
  if (v >= 0) pub(this->controller_second_sensor_, (float) (uint16_t) v);

  // Register 0034: Engine Running Status
  v = get_reg(0x0022);
  if (v >= 0) pub(this->engine_running_status_sensor_, (float) (uint16_t) v);

  // Register 0035: Delay
  v = get_reg(0x0023);
  if (v >= 0) pub(this->engine_delay_sensor_, (float) (uint16_t) v);

  // Register 0036: Auto Running Status
  v = get_reg(0x0024);
  if (v >= 0) pub(this->auto_running_status_sensor_, (float) (uint16_t) v);

  // Register 0037: Auto Delay
  v = get_reg(0x0025);
  if (v >= 0) pub(this->auto_delay_sensor_, (float) (uint16_t) v);

  // Registers 0042-0043: Engine Accumulated Running Time (hours) = MSB*65536 + LSB
  {
    int32_t msb = get_reg(0x002A);
    int32_t lsb = get_reg(0x002B);
    if (msb >= 0 && lsb >= 0) {
      uint32_t total_hours = ((uint32_t)(uint16_t) msb) * 65536 + (uint16_t) lsb;
      pub(this->engine_total_hours_sensor_, (float) total_hours);
    }
  }

  // Register 0044: Running Time Minutes
  v = get_reg(0x002C);
  if (v >= 0) pub(this->engine_total_minutes_sensor_, (float) (uint16_t) v);

  // Register 0045: Running Time Seconds
  v = get_reg(0x002D);
  if (v >= 0) pub(this->engine_total_seconds_sensor_, (float) (uint16_t) v);

  // Registers 0046-0047: Accumulated Start Times = MSB*65536 + LSB
  {
    int32_t msb = get_reg(0x002E);
    int32_t lsb = get_reg(0x002F);
    if (msb >= 0 && lsb >= 0) {
      uint32_t total_starts = ((uint32_t)(uint16_t) msb) * 65536 + (uint16_t) lsb;
      pub(this->total_start_times_sensor_, (float) total_starts);
    }
  }

  // Registers 0048-0049: Accumulated Electric Energy (kWh) = MSB*65536 + LSB
  {
    int32_t msb = get_reg(0x0030);
    int32_t lsb = get_reg(0x0031);
    if (msb >= 0 && lsb >= 0) {
      uint32_t total_energy = ((uint32_t)(uint16_t) msb) * 65536 + (uint16_t) lsb;
      pub(this->total_energy_sensor_, (float) total_energy);
    }
  }

  // Register 0050: Software Version
  v = get_reg(0x0032);
  if (v >= 0) pub(this->software_version_sensor_, (float) (uint16_t) v);

  // Register 0051: Hardware Version
  v = get_reg(0x0033);
  if (v >= 0) pub(this->hardware_version_sensor_, (float) (uint16_t) v);

  // Register 0052: Phase A Angle (signed)
  v = get_reg_signed(0x0034);
  if (v != -999999) pub_signed(this->phase_a_angle_sensor_, (int16_t) v, 1.0f);

  // Register 0053: Phase B Angle (signed)
  v = get_reg_signed(0x0035);
  if (v != -999999) pub_signed(this->phase_b_angle_sensor_, (int16_t) v, 1.0f);

  // Register 0054: Phase C Angle (signed)
  v = get_reg_signed(0x0036);
  if (v != -999999) pub_signed(this->phase_c_angle_sensor_, (int16_t) v, 1.0f);

  // Register 0055: Phase A Active Power (signed, ratio 0.1 kW)
  v = get_reg_signed(0x0037);
  if (v != -999999) pub_signed(this->phase_a_active_power_sensor_, (int16_t) v, 0.1f);

  // Register 0056: Phase B Active Power
  v = get_reg_signed(0x0038);
  if (v != -999999) pub_signed(this->phase_b_active_power_sensor_, (int16_t) v, 0.1f);

  // Register 0057: Phase C Active Power
  v = get_reg_signed(0x0039);
  if (v != -999999) pub_signed(this->phase_c_active_power_sensor_, (int16_t) v, 0.1f);

  // Register 0058: Total Active Power (signed, ratio 0.1 kW)
  v = get_reg_signed(0x003A);
  if (v != -999999) pub_signed(this->total_active_power_sensor_, (int16_t) v, 0.1f);

  // Register 0059: Reactive Power (signed, ratio 0.1 kVAR)
  v = get_reg_signed(0x003B);
  if (v != -999999) pub_signed(this->reactive_power_sensor_, (int16_t) v, 0.1f);

  // Register 0060: Apparent Power (signed, ratio 0.1 kVA)
  v = get_reg_signed(0x003C);
  if (v != -999999) pub_signed(this->apparent_power_sensor_, (int16_t) v, 0.1f);

  // Register 0061: Power Factor (ratio 0.01)
  v = get_reg(0x003D);
  if (v >= 0) pub_if_valid(this->power_factor_sensor_, (uint16_t) v, 0.01f);

  // Register 0062: Output Load Percentage
  v = get_reg(0x003E);
  if (v >= 0) pub_if_valid(this->output_load_percent_sensor_, (uint16_t) v, 1.0f);

  // Register 0063: Release Year
  v = get_reg(0x003F);
  if (v >= 0) pub(this->release_year_sensor_, (float) (uint16_t) v);

  // Register 0064: Release Month
  v = get_reg(0x0040);
  if (v >= 0) pub(this->release_month_sensor_, (float) (uint16_t) v);

  // Register 0065: Release Day
  v = get_reg(0x0041);
  if (v >= 0) pub(this->release_day_sensor_, (float) (uint16_t) v);
}

// ============================================================
//  Update (called every polling interval)
// ============================================================
void SmartgenHSC941::update() {
  ESP_LOGD(TAG, "Polling SmartGen HSC941 at address %u...", this->address_);

  bool any_success = false;

  // ---- Read Coils (Function Code 01H) ----
  // Read coils 0-87 in one request (88 coils = 11 bytes)
  {
    uint8_t coil_data[16] = {0};
    size_t data_len = 0;
    // Read 88 coils starting from address 0
    if (this->read_coils_(0, 88, coil_data, &data_len)) {
      ESP_LOGD(TAG, "Read %zu bytes of coil data", data_len);
      this->process_coil_data_(coil_data, data_len, 0, 88);
      any_success = true;
    } else {
      ESP_LOGW(TAG, "Failed to read coils 0-87");
    }

    // Small delay between requests
    vTaskDelay(pdMS_TO_TICKS(INTER_FRAME_DELAY_MS));
  }

  // ---- Read Holding Registers (Function Code 03H) ----
  // Read registers 0x0007 to 0x0041 (59 registers) in one batch
  // This covers all data from Gen UA (0x0007) to Release Day (0x0041)
  {
    const uint16_t reg_start = 0x0007;
    const uint16_t reg_count = 0x0041 - 0x0007 + 1;  // 59 registers
    uint16_t reg_data[64] = {0};

    if (this->read_holding_registers_(reg_start, reg_count, reg_data)) {
      ESP_LOGD(TAG, "Read %u holding registers starting at 0x%04X", reg_count, reg_start);
      this->process_register_data_(reg_data, reg_start, reg_count);
      any_success = true;
    } else {
      ESP_LOGW(TAG, "Failed to read holding registers 0x%04X-0x%04X", reg_start, reg_start + reg_count - 1);
    }
  }

  // Track communication health
  if (any_success) {
    this->comm_failures_ = 0;
    this->status_clear_warning();
  } else {
    this->comm_failures_++;
    if (this->comm_failures_ >= MAX_FAILURES) {
      this->status_set_warning("Communication failure");
      ESP_LOGE(TAG, "Multiple consecutive communication failures (%u)", this->comm_failures_);
    }
  }
}

// ============================================================
//  JSON Builders for Web UI
// ============================================================

static void json_append_sensor(std::string &out, const char *key, sensor::Sensor *s, bool &first) {
  if (s == nullptr)
    return;
  if (!first) out += ',';
  first = false;
  out += '"';
  out += key;
  out += "\":";
  if (std::isnan(s->state)) {
    out += "null";
  } else {
    char buf[32];
    snprintf(buf, sizeof(buf), "%.2f", s->state);
    out += buf;
  }
}

static void json_append_bs(std::string &out, const char *key, binary_sensor::BinarySensor *s, bool &first) {
  if (s == nullptr)
    return;
  if (!first) out += ',';
  first = false;
  out += '"';
  out += key;
  out += "\":";
  out += s->state ? "true" : "false";
}

void SmartgenHSC941::build_sensors_json(std::string &out) {
  out += '{';
  bool first = true;
  json_append_sensor(out, "gen_va", gen_voltage_a_sensor_, first);
  json_append_sensor(out, "gen_vb", gen_voltage_b_sensor_, first);
  json_append_sensor(out, "gen_vc", gen_voltage_c_sensor_, first);
  json_append_sensor(out, "gen_vab", gen_voltage_ab_sensor_, first);
  json_append_sensor(out, "gen_vbc", gen_voltage_bc_sensor_, first);
  json_append_sensor(out, "gen_vca", gen_voltage_ca_sensor_, first);
  json_append_sensor(out, "gen_freq", gen_frequency_sensor_, first);
  json_append_sensor(out, "ia", phase_a_current_sensor_, first);
  json_append_sensor(out, "ib", phase_b_current_sensor_, first);
  json_append_sensor(out, "ic", phase_c_current_sensor_, first);
  json_append_sensor(out, "water_temp", water_temp_sensor_, first);
  json_append_sensor(out, "water_temp_r", water_temp_resistance_sensor_, first);
  json_append_sensor(out, "oil_press", oil_pressure_sensor_, first);
  json_append_sensor(out, "oil_press_r", oil_pressure_resistance_sensor_, first);
  json_append_sensor(out, "aux1", aux_sensor_1_sensor_, first);
  json_append_sensor(out, "aux1_r", aux_sensor_1_resistance_sensor_, first);
  json_append_sensor(out, "rpm", engine_speed_sensor_, first);
  json_append_sensor(out, "batt_v", battery_voltage_sensor_, first);
  json_append_sensor(out, "charge_v", charge_voltage_sensor_, first);
  json_append_sensor(out, "ctrl_year", controller_year_sensor_, first);
  json_append_sensor(out, "ctrl_month", controller_month_sensor_, first);
  json_append_sensor(out, "ctrl_day", controller_day_sensor_, first);
  json_append_sensor(out, "ctrl_wday", controller_weekday_sensor_, first);
  json_append_sensor(out, "ctrl_hour", controller_hour_sensor_, first);
  json_append_sensor(out, "ctrl_min", controller_minute_sensor_, first);
  json_append_sensor(out, "ctrl_sec", controller_second_sensor_, first);
  json_append_sensor(out, "eng_status", engine_running_status_sensor_, first);
  json_append_sensor(out, "eng_delay", engine_delay_sensor_, first);
  json_append_sensor(out, "auto_status", auto_running_status_sensor_, first);
  json_append_sensor(out, "auto_delay", auto_delay_sensor_, first);
  json_append_sensor(out, "total_hours", engine_total_hours_sensor_, first);
  json_append_sensor(out, "total_min", engine_total_minutes_sensor_, first);
  json_append_sensor(out, "total_sec", engine_total_seconds_sensor_, first);
  json_append_sensor(out, "total_starts", total_start_times_sensor_, first);
  json_append_sensor(out, "total_kwh", total_energy_sensor_, first);
  json_append_sensor(out, "sw_ver", software_version_sensor_, first);
  json_append_sensor(out, "hw_ver", hardware_version_sensor_, first);
  json_append_sensor(out, "angle_a", phase_a_angle_sensor_, first);
  json_append_sensor(out, "angle_b", phase_b_angle_sensor_, first);
  json_append_sensor(out, "angle_c", phase_c_angle_sensor_, first);
  json_append_sensor(out, "pa_kw", phase_a_active_power_sensor_, first);
  json_append_sensor(out, "pb_kw", phase_b_active_power_sensor_, first);
  json_append_sensor(out, "pc_kw", phase_c_active_power_sensor_, first);
  json_append_sensor(out, "total_kw", total_active_power_sensor_, first);
  json_append_sensor(out, "kvar", reactive_power_sensor_, first);
  json_append_sensor(out, "kva", apparent_power_sensor_, first);
  json_append_sensor(out, "pf", power_factor_sensor_, first);
  json_append_sensor(out, "load_pct", output_load_percent_sensor_, first);
  json_append_sensor(out, "rel_year", release_year_sensor_, first);
  json_append_sensor(out, "rel_month", release_month_sensor_, first);
  json_append_sensor(out, "rel_day", release_day_sensor_, first);
  out += '}';
}

void SmartgenHSC941::build_binary_sensors_json(std::string &out) {
  out += '{';
  bool first = true;
  json_append_bs(out, "alarm", common_alarm_bs_, first);
  json_append_bs(out, "warning", common_warning_bs_, first);
  json_append_bs(out, "shutdown", common_shutdown_bs_, first);
  json_append_bs(out, "on_load", gen_on_load_bs_, first);
  json_append_bs(out, "estop", emergency_stop_bs_, first);
  json_append_bs(out, "overspeed_sd", overspeed_shutdown_bs_, first);
  json_append_bs(out, "underspeed_sd", underspeed_shutdown_bs_, first);
  json_append_bs(out, "loss_speed_sd", loss_speed_signal_shutdown_bs_, first);
  json_append_bs(out, "overfreq_sd", gen_overfrequency_shutdown_bs_, first);
  json_append_bs(out, "underfreq_sd", gen_underfrequency_shutdown_bs_, first);
  json_append_bs(out, "overvolt_sd", gen_overvoltage_shutdown_bs_, first);
  json_append_bs(out, "undervolt_sd", gen_undervoltage_shutdown_bs_, first);
  json_append_bs(out, "overcurr_sd", gen_overcurrent_shutdown_bs_, first);
  json_append_bs(out, "crank_fail_sd", crank_failure_shutdown_bs_, first);
  json_append_bs(out, "hi_temp_sd_in", high_temp_shutdown_in_bs_, first);
  json_append_bs(out, "lo_op_sd_in", low_op_shutdown_in_bs_, first);
  json_append_bs(out, "no_gen_sd", no_gen_shutdown_bs_, first);
  json_append_bs(out, "ext_sd", external_shutdown_input_bs_, first);
  json_append_bs(out, "lo_fuel_sd_in", low_fuel_level_shutdown_in_bs_, first);
  json_append_bs(out, "lo_cool_sd_in", low_coolant_level_shutdown_in_bs_, first);
  json_append_bs(out, "hi_temp_w_in", high_temp_warning_in_bs_, first);
  json_append_bs(out, "lo_op_w_in", low_op_warning_in_bs_, first);
  json_append_bs(out, "stop_fail_w", stop_failure_warning_bs_, first);
  json_append_bs(out, "lo_fuel_w_in", low_fuel_level_warning_in_bs_, first);
  json_append_bs(out, "charge_fail_w", charging_failure_warning_bs_, first);
  json_append_bs(out, "batt_uv_w", battery_undervoltage_warning_bs_, first);
  json_append_bs(out, "batt_ov_w", battery_overvoltage_warning_bs_, first);
  json_append_bs(out, "ext_w", external_warning_input_bs_, first);
  json_append_bs(out, "loss_speed_w", loss_speed_signal_warning_bs_, first);
  json_append_bs(out, "lo_cool_w_in", low_coolant_level_warning_in_bs_, first);
  json_append_bs(out, "temp_open_w", temp_sensor_open_warning_bs_, first);
  json_append_bs(out, "op_open_w", op_sensor_open_warning_bs_, first);
  json_append_bs(out, "aux_open_w", aux_sensor_open_warning_bs_, first);
  json_append_bs(out, "aux_w", aux_sensor_warning_bs_, first);
  json_append_bs(out, "aux_sd", aux_sensor_shutdown_bs_, first);
  json_append_bs(out, "auto_mode", in_auto_mode_bs_, first);
  json_append_bs(out, "manual_mode", in_manual_mode_bs_, first);
  json_append_bs(out, "stop_mode", in_stop_mode_bs_, first);
  json_append_bs(out, "temp_open_sd", temp_sensor_open_shutdown_bs_, first);
  json_append_bs(out, "op_open_sd", op_sensor_open_shutdown_bs_, first);
  json_append_bs(out, "lo_oil_sd_in", low_engine_oil_level_shutdown_in_bs_, first);
  json_append_bs(out, "aux_open_sd", aux_sensor_open_shutdown_bs_, first);
  json_append_bs(out, "estop_in", emergency_stop_input_bs_, first);
  json_append_bs(out, "aux_in1", aux_input_1_bs_, first);
  json_append_bs(out, "aux_in2", aux_input_2_bs_, first);
  json_append_bs(out, "aux_in3", aux_input_3_bs_, first);
  json_append_bs(out, "aux_in4", aux_input_4_bs_, first);
  json_append_bs(out, "aux_in5", aux_input_5_bs_, first);
  json_append_bs(out, "aux_in6", aux_input_6_bs_, first);
  json_append_bs(out, "start_relay", start_relay_output_bs_, first);
  json_append_bs(out, "fuel_relay", fuel_relay_output_bs_, first);
  json_append_bs(out, "aux_out1", aux_output_1_bs_, first);
  json_append_bs(out, "aux_out2", aux_output_2_bs_, first);
  json_append_bs(out, "aux_out3", aux_output_3_bs_, first);
  json_append_bs(out, "aux_out4", aux_output_4_bs_, first);
  json_append_bs(out, "hi_temp_sd", high_temp_shutdown_bs_, first);
  json_append_bs(out, "lo_op_sd", low_op_shutdown_bs_, first);
  json_append_bs(out, "hi_temp_w", high_temp_warning_bs_, first);
  json_append_bs(out, "lo_op_w", low_op_warning_bs_, first);
  out += '}';
}

void SmartgenHSC941::build_status_json(std::string &out) {
  out += "{\"connected\":";
  out += this->is_connected() ? "true" : "false";
  out += ",\"sensors\":";
  this->build_sensors_json(out);
  out += ",\"binary\":";
  this->build_binary_sensors_json(out);
  out += '}';
}

}  // namespace smartgen_hsc941
}  // namespace esphome
