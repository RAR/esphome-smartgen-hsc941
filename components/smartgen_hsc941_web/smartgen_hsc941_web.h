#pragma once

#include "esphome/core/component.h"
#include "../smartgen_hsc941/smartgen_hsc941.h"
#include <esp_http_server.h>
#include <string>

namespace esphome {
namespace smartgen_hsc941_web {

class SmartgenHSC941Web : public Component {
 public:
  SmartgenHSC941Web() = default;

  void set_controller(smartgen_hsc941::SmartgenHSC941 *controller) { this->controller_ = controller; }
  void set_port(uint16_t port) { this->port_ = port; }
  void set_css_url(const std::string &url) { this->css_url_ = url; }
  void set_js_url(const std::string &url) { this->js_url_ = url; }

  void setup() override;
  void loop() override {}
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::AFTER_WIFI; }

  // HTTP request handlers (static so esp_http_server can call them)
  static esp_err_t handle_root_(httpd_req_t *req);
  static esp_err_t handle_api_status_(httpd_req_t *req);
  static esp_err_t handle_api_command_(httpd_req_t *req);

  // Accessor for the controller
  smartgen_hsc941::SmartgenHSC941 *get_controller() { return this->controller_; }

 protected:
  smartgen_hsc941::SmartgenHSC941 *controller_{nullptr};
  httpd_handle_t server_{nullptr};
  uint16_t port_{8080};
  std::string css_url_;
  std::string js_url_;

  void start_server_();
  void stop_server_();

  // Dashboard HTML (stored in flash .rodata, not IRAM)
  static const char DASHBOARD_HTML[];
};

}  // namespace smartgen_hsc941_web
}  // namespace esphome
