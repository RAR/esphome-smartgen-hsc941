#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "../smartgen_hsc941/smartgen_hsc941.h"
#include <esp_http_server.h>
#include <string>
#include <array>

namespace esphome {
namespace smartgen_hsc941_web {

static const uint8_t MAX_RELAYS = 8;

struct RelayInfo {
  switch_::Switch *sw{nullptr};
  std::string name;
};

class SmartgenHSC941Web : public Component {
 public:
  SmartgenHSC941Web() = default;

  void set_controller(smartgen_hsc941::SmartgenHSC941 *controller) { this->controller_ = controller; }
  void set_port(uint16_t port) { this->port_ = port; }
  void set_css_url(const std::string &url) { this->css_url_ = url; }
  void set_js_url(const std::string &url) { this->js_url_ = url; }
  void set_ambient_temp_sensor(sensor::Sensor *s) { this->ambient_temp_ = s; }
  void set_relay(uint8_t index, switch_::Switch *sw, const std::string &name) {
    if (index < MAX_RELAYS) {
      this->relays_[index].sw = sw;
      this->relays_[index].name = name;
    }
  }

  void setup() override;
  void loop() override {}
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }

  // HTTP request handlers (static so esp_http_server can call them)
  static esp_err_t handle_root_(httpd_req_t *req);
  static esp_err_t handle_api_status_(httpd_req_t *req);
  static esp_err_t handle_api_command_(httpd_req_t *req);
  static esp_err_t handle_api_relay_(httpd_req_t *req);

  // Accessor for the controller
  smartgen_hsc941::SmartgenHSC941 *get_controller() { return this->controller_; }

  // Accessors for ambient temp and relays (used by handlers)
  sensor::Sensor *get_ambient_temp() { return this->ambient_temp_; }
  const std::array<RelayInfo, MAX_RELAYS> &get_relays() const { return this->relays_; }

 protected:
  smartgen_hsc941::SmartgenHSC941 *controller_{nullptr};
  httpd_handle_t server_{nullptr};
  uint16_t port_{8080};
  std::string css_url_;
  std::string js_url_;
  sensor::Sensor *ambient_temp_{nullptr};
  std::array<RelayInfo, MAX_RELAYS> relays_{};

  void start_server_();
  void stop_server_();

  // Dashboard HTML (stored in flash .rodata, not IRAM)
  static const char DASHBOARD_HTML[];
};

}  // namespace smartgen_hsc941_web
}  // namespace esphome
