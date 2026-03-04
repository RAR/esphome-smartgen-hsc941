#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/switch/switch.h"
#include "../smartgen_hsc941/smartgen_hsc941.h"
#include <esp_http_server.h>
#include <nvs_flash.h>
#include <nvs.h>
#include <esp_spiffs.h>
#include <string>
#include <array>
#include <vector>
#include <ctime>

namespace esphome {
namespace smartgen_hsc941_web {

static const uint8_t MAX_RELAYS = 8;

struct RelayInfo {
  switch_::Switch *sw{nullptr};
  std::string name;
};

// ── Exercise schedule ────────────────────────────────────────
enum class ExerciseState : uint8_t {
  IDLE = 0,       // Not running
  STARTING,       // Sent manual+start, waiting for engine RPM > threshold
  RUNNING,        // Engine confirmed running, counting down duration
  COOLDOWN,       // Duration expired, opening transfer switch
  STOPPING,       // Sent stop, waiting for engine to spin down
  FAILED,         // Crank failure or shutdown detected — will auto-reset to IDLE
};

struct ExerciseConfig {
  bool enabled{false};
  uint8_t day{1};           // 0=Sun 1=Mon … 6=Sat  (7 = every day)
  uint8_t hour{10};
  uint8_t minute{0};
  uint16_t duration_min{15};
  bool load_transfer{false}; // Close transfer switch during exercise
};

// ── Relay thermostat ─────────────────────────────────────────
// sensor_key: "water_temp", "aux1", "ambient_temp", or "" (disabled)
struct RelayThermostat {
  bool enabled{false};
  std::string sensor_key;     // JSON key for the sensor in /api/status
  float on_below{5.0f};       // Turn relay ON when temp drops below this
  float off_above{10.0f};     // Turn relay OFF when temp rises above this
};

// ── Event log ────────────────────────────────────────────────
static const uint16_t MAX_LOG_ENTRIES = 500;  // ring buffer capacity

struct EventEntry {
  time_t timestamp{0};
  std::string message;
};

// ── Maintenance tracker ─────────────────────────────────────
static const uint8_t MAX_MAINT_ITEMS = 5;

struct MaintenanceItem {
  std::string name;
  uint16_t interval_hours{250};
  float last_reset_hours{0};
};

// ── Runtime History ─────────────────────────────────────────
struct RuntimeDay {
  char date[11];   // "YYYY-MM-DD"
  float hours;
};

class SmartgenHSC941Web : public Component {
 public:
  SmartgenHSC941Web() = default;

  void set_controller(smartgen_hsc941::SmartgenHSC941 *controller) { this->controller_ = controller; }
  void set_port(uint16_t port) { this->port_ = port; }
  void set_css_url(const std::string &url) { this->css_url_ = url; }
  void set_js_url(const std::string &url) { this->js_url_ = url; }
  void set_ambient_temp_sensor(sensor::Sensor *s) { this->ambient_temp_ = s; }
  void set_ambient_temp_name(const std::string &n) { this->ambient_temp_name_ = n; }
  void set_ambient_humidity_sensor(sensor::Sensor *s) { this->ambient_humidity_ = s; }
  void set_ambient_humidity_name(const std::string &n) { this->ambient_humidity_name_ = n; }
  void set_single_phase(bool v) { this->single_phase_ = v; }
  void set_tank_size(float v) { this->tank_size_liters_ = v; }
  void set_burn_rate(float v) { this->burn_rate_lph_ = v; }
  void set_fuel_level_sensor(sensor::Sensor *s) { this->fuel_level_sensor_ = s; }
  void set_fuel_type(const std::string &t) { this->fuel_type_ = t; }
  void set_fuel_unit(const std::string &u) { this->fuel_unit_ = u; }
  void set_language(const std::string &l) { this->language_ = l; }
  void set_mains_sensor(binary_sensor::BinarySensor *s) { this->mains_sensor_ = s; }
  void set_buzzer_pin(int pin) { this->buzzer_pin_ = (int8_t)pin; }
  void set_pin_code(const std::string &code) { this->pin_code_ = code; }
  void set_relay(uint8_t index, switch_::Switch *sw, const std::string &name) {
    if (index < MAX_RELAYS) {
      this->relays_[index].sw = sw;
      this->relays_[index].name = name;
    }
  }

  void setup() override;
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }

  // HTTP request handlers (static so esp_http_server can call them)
  static esp_err_t handle_root_(httpd_req_t *req);
  static esp_err_t handle_api_status_(httpd_req_t *req);
  static esp_err_t handle_api_command_(httpd_req_t *req);
  static esp_err_t handle_api_relay_(httpd_req_t *req);
  static esp_err_t handle_api_exercise_get_(httpd_req_t *req);
  static esp_err_t handle_api_exercise_post_(httpd_req_t *req);
  static esp_err_t handle_api_thermostat_get_(httpd_req_t *req);
  static esp_err_t handle_api_thermostat_post_(httpd_req_t *req);
  static esp_err_t handle_api_eventlog_get_(httpd_req_t *req);
  static esp_err_t handle_api_eventlog_post_(httpd_req_t *req);
  static esp_err_t handle_manifest_(httpd_req_t *req);
  static esp_err_t handle_api_maintenance_get_(httpd_req_t *req);
  static esp_err_t handle_api_maintenance_post_(httpd_req_t *req);
  static esp_err_t handle_api_fuel_get_(httpd_req_t *req);
  static esp_err_t handle_api_fuel_post_(httpd_req_t *req);
  static esp_err_t handle_api_runtime_history_(httpd_req_t *req);
  static esp_err_t handle_api_battery_history_(httpd_req_t *req);
  static esp_err_t handle_api_buzzer_post_(httpd_req_t *req);

  // Accessor for the controller
  smartgen_hsc941::SmartgenHSC941 *get_controller() { return this->controller_; }

  // Accessors for ambient temp and relays (used by handlers)
  sensor::Sensor *get_ambient_temp() { return this->ambient_temp_; }
  const std::string &get_ambient_temp_name() const { return this->ambient_temp_name_; }
  sensor::Sensor *get_ambient_humidity() { return this->ambient_humidity_; }
  const std::string &get_ambient_humidity_name() const { return this->ambient_humidity_name_; }
  const std::array<RelayInfo, MAX_RELAYS> &get_relays() const { return this->relays_; }

  // Exercise accessors (used by handlers)
  const ExerciseConfig &get_exercise_config() const { return this->exercise_cfg_; }
  ExerciseState get_exercise_state() const { return this->exercise_state_; }
  uint32_t get_exercise_remaining_sec() const;
  std::string get_exercise_last_run() const { return this->exercise_last_run_; }
  std::string get_exercise_fail_reason() const { return this->exercise_fail_reason_; }

  // Thermostat accessors (used by handlers)
  const std::array<RelayThermostat, MAX_RELAYS> &get_thermostats() const { return this->relay_thermostats_; }
  float get_sensor_value_by_key_(const std::string &key) const;

  // Event log
  void log_event(const std::string &message);
  const std::vector<EventEntry> &get_event_log() const { return this->event_log_; }

  // Maintenance
  const std::array<MaintenanceItem, MAX_MAINT_ITEMS> &get_maint_items() const { return this->maint_items_; }

  // Fuel
  float get_tank_size() const { return this->tank_size_liters_; }
  float get_burn_rate() const { return this->burn_rate_lph_; }
  float get_last_fill_hours() const { return this->last_fill_hours_; }
  sensor::Sensor *get_fuel_level_sensor() { return this->fuel_level_sensor_; }
  const std::string &get_fuel_type() const { return this->fuel_type_; }
  const std::string &get_fuel_unit() const { return this->fuel_unit_; }

  // Runtime history
  const std::vector<RuntimeDay> &get_runtime_history() const { return this->runtime_history_; }

  // Language
  const std::string &get_language() const { return this->language_; }

  // Mains sensor
  binary_sensor::BinarySensor *get_mains_sensor() { return this->mains_sensor_; }

 protected:
  smartgen_hsc941::SmartgenHSC941 *controller_{nullptr};
  httpd_handle_t server_{nullptr};
  uint16_t port_{8080};
  std::string css_url_;
  std::string js_url_;
  sensor::Sensor *ambient_temp_{nullptr};
  std::string ambient_temp_name_{"Ambient Temp"};
  sensor::Sensor *ambient_humidity_{nullptr};
  std::string ambient_humidity_name_{"Humidity"};
  bool single_phase_{false};
  std::string language_{"en"};
  binary_sensor::BinarySensor *mains_sensor_{nullptr};
  int8_t buzzer_pin_{-1};
  bool buzzer_active_{false};
  bool buzzer_silenced_{false};
  std::string pin_code_;
  std::array<RelayInfo, MAX_RELAYS> relays_{};

  // ── Exercise schedule ──
  ExerciseConfig exercise_cfg_{};
  ExerciseState exercise_state_{ExerciseState::IDLE};
  uint32_t exercise_phase_start_{0};    // millis() when current phase started
  uint32_t exercise_run_start_{0};      // millis() when RUNNING phase started (for duration)
  uint32_t last_exercise_check_{0};     // millis() of last time check
  std::string exercise_last_run_;       // human-readable timestamp
  std::string exercise_fail_reason_;    // reason for last failure
  bool exercise_triggered_today_{false};
  uint8_t exercise_last_trigger_day_{255};
  bool exercise_start_cmd_sent_{false}; // start coil already sent this STARTING phase

  void load_exercise_config_();
  void save_exercise_config_();
  void check_exercise_schedule_();
  void exercise_step_();
  void start_exercise_();
  void stop_exercise_();

  // ── Relay thermostat ──
  std::array<RelayThermostat, MAX_RELAYS> relay_thermostats_{};
  uint32_t last_thermostat_check_{0};

  void load_thermostat_config_();
  void save_thermostat_config_();
  void thermostat_step_();

  // ── Event log ──
  std::vector<EventEntry> event_log_;
  bool spiffs_mounted_{false};
  bool prev_connected_{false};
  bool prev_shutdown_{false};
  bool prev_warning_{false};
  bool prev_estop_{false};
  bool prev_engine_running_{false};
  bool prev_auto_mode_{false};
  bool prev_manual_mode_{false};
  bool prev_stop_mode_{false};
  bool prev_gen_on_load_{false};
  uint32_t last_alarm_check_{0};

  void init_spiffs_();
  void load_event_log_();
  void append_event_to_file_(const std::string &message, time_t ts);
  void clear_event_log_file_();
  void check_alarm_transitions_();

  // ── Buzzer ──
  void init_buzzer_();
  void buzzer_start_();
  void buzzer_stop_();

  // ── Maintenance tracker ──
  std::array<MaintenanceItem, MAX_MAINT_ITEMS> maint_items_{};
  void init_maint_defaults_();
  void load_maintenance_config_();
  void save_maintenance_config_();

  // ── Fuel estimator ──
  float tank_size_liters_{0};
  float burn_rate_lph_{0};
  float last_fill_hours_{0};
  sensor::Sensor *fuel_level_sensor_{nullptr};
  std::string fuel_type_{"diesel"};
  std::string fuel_unit_{"lph"};
  void load_fuel_config_();
  void save_fuel_config_();

  // ── Runtime history ──
  std::vector<RuntimeDay> runtime_history_;
  float prev_total_hours_{0};
  uint8_t prev_hist_day_{255};
  uint32_t last_hist_check_{0};
  void load_runtime_history_();
  void save_runtime_day_(const char *date, float hours);
  void check_runtime_history_();

  // ── Battery voltage history ──
  static const size_t BATT_HIST_SIZE = 240;  // 240 samples @ 30s = 2 hours
  float batt_hist_buf_[240]{};
  size_t batt_hist_head_{0};
  size_t batt_hist_count_{0};
  float batt_24h_min_{999.0f};
  float batt_24h_max_{0.0f};
  double batt_24h_sum_{0};
  uint32_t batt_24h_count_{0};
  uint32_t last_batt_sample_{0};
  void sample_battery_voltage_();

  void start_server_();
  void stop_server_();

  // Dashboard HTML (stored in flash .rodata, not IRAM)
  static const char DASHBOARD_HTML[];
};

}  // namespace smartgen_hsc941_web
}  // namespace esphome
