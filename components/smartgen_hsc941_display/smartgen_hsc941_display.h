#pragma once

#include "esphome/core/component.h"
#include "esphome/components/smartgen_hsc941/smartgen_hsc941.h"
#include "esphome/components/smartgen_hsc941_web/smartgen_hsc941_web.h"

// EEZ Studio generated UI (files copied flat into component dir)
extern "C" {
#include "ui.h"
#include "screens.h"
#include "vars.h"
#include "actions.h"
}

namespace esphome {
namespace smartgen_hsc941_display {

class SmartgenHSC941Display : public Component {
 public:
  void set_hub(smartgen_hsc941::SmartgenHSC941 *hub) { this->hub_ = hub; }
  void set_web(smartgen_hsc941_web::SmartgenHSC941Web *web) { this->web_ = web; }
  void set_fuel_sensor(sensor::Sensor *s) { this->fuel_sensor_ = s; }

  float get_setup_priority() const override { return setup_priority::LATE; }
  void setup() override;
  void loop() override;

 protected:
  smartgen_hsc941::SmartgenHSC941 *hub_{nullptr};
  smartgen_hsc941_web::SmartgenHSC941Web *web_{nullptr};
  sensor::Sensor *fuel_sensor_{nullptr};

  // Update LVGL widgets from hub sensor data
  void update_gauges_();
  void update_engine_data_();
  void update_power_tab_();
  void update_alarms_();
  void update_status_bar_();
  void update_event_log_();

  // Track previous state to avoid unnecessary redraws
  uint32_t last_update_ms_{0};
  static const uint32_t UPDATE_INTERVAL_MS = 1000;
  uint8_t prev_alarm_state_{0xFF};  // force first update
  size_t prev_event_count_{0};
};

}  // namespace smartgen_hsc941_display
}  // namespace esphome
