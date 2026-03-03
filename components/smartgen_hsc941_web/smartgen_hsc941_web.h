#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
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

class SmartgenHSC941Web : public Component {
 public:
  SmartgenHSC941Web() = default;

  void set_controller(smartgen_hsc941::SmartgenHSC941 *controller) { this->controller_ = controller; }
  void set_port(uint16_t port) { this->port_ = port; }
  void set_css_url(const std::string &url) { this->css_url_ = url; }
  void set_js_url(const std::string &url) { this->js_url_ = url; }
  void set_ambient_temp_sensor(sensor::Sensor *s) { this->ambient_temp_ = s; }
  void set_ambient_humidity_sensor(sensor::Sensor *s) { this->ambient_humidity_ = s; }
  void set_single_phase(bool v) { this->single_phase_ = v; }
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

  // Accessor for the controller
  smartgen_hsc941::SmartgenHSC941 *get_controller() { return this->controller_; }

  // Accessors for ambient temp and relays (used by handlers)
  sensor::Sensor *get_ambient_temp() { return this->ambient_temp_; }
  sensor::Sensor *get_ambient_humidity() { return this->ambient_humidity_; }
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

 protected:
  smartgen_hsc941::SmartgenHSC941 *controller_{nullptr};
  httpd_handle_t server_{nullptr};
  uint16_t port_{8080};
  std::string css_url_;
  std::string js_url_;
  sensor::Sensor *ambient_temp_{nullptr};
  sensor::Sensor *ambient_humidity_{nullptr};
  bool single_phase_{false};
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

  void start_server_();
  void stop_server_();

  // Dashboard HTML (stored in flash .rodata, not IRAM)
  static const char DASHBOARD_HTML[];
};

}  // namespace smartgen_hsc941_web
}  // namespace esphome
