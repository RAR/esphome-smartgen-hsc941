#include "smartgen_hsc941_display.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"
#include <cstdio>
#include <cmath>

namespace esphome {
namespace smartgen_hsc941_display {

static const char *const TAG = "smartgen_hsc941_display";

// Static hub pointer for C button callbacks
static smartgen_hsc941::SmartgenHSC941 *s_hub = nullptr;

// Button event callbacks
static void btn_start_cb(lv_event_t *e) { if (s_hub) s_hub->write_coil(0, true); }
static void btn_stop_cb(lv_event_t *e) { if (s_hub) s_hub->write_coil(1, true); }
static void btn_auto_cb(lv_event_t *e) { if (s_hub) s_hub->write_coil(3, true); }
static void btn_manual_cb(lv_event_t *e) { if (s_hub) s_hub->write_coil(4, true); }
static void btn_xfer_on_cb(lv_event_t *e) { if (s_hub) s_hub->write_coil(6, true); }
static void btn_xfer_off_cb(lv_event_t *e) { if (s_hub) s_hub->write_coil(5, true); }

static void pulse_anim_cb(void *obj, int32_t v) {
  lv_obj_set_style_outline_opa((lv_obj_t *)obj, (lv_opa_t)v, 0);
}

static void set_btn_active(lv_obj_t *btn, bool active) {
  bool currently_active = lv_anim_get(btn, pulse_anim_cb) != NULL;

  if (active && !currently_active) {
    // Becoming active — set up outline and start animation
    lv_color_t border_col = lv_obj_get_style_border_color(btn, 0);
    lv_obj_set_style_outline_color(btn, border_col, 0);
    lv_obj_set_style_outline_width(btn, 2, 0);
    lv_obj_set_style_outline_pad(btn, 2, 0);

    lv_anim_t a;
    lv_anim_init(&a);
    lv_anim_set_var(&a, btn);
    lv_anim_set_values(&a, LV_OPA_40, LV_OPA_COVER);
    lv_anim_set_time(&a, 1000);
    lv_anim_set_playback_time(&a, 1000);
    lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
    lv_anim_set_exec_cb(&a, pulse_anim_cb);
    lv_anim_start(&a);
  } else if (!active && currently_active) {
    // Becoming inactive — stop animation and hide outline
    lv_anim_del(btn, pulse_anim_cb);
    lv_obj_set_style_outline_width(btn, 0, 0);
    lv_obj_set_style_outline_opa(btn, LV_OPA_TRANSP, 0);
  }
  // If state unchanged, do nothing — let animation run undisturbed
}

void SmartgenHSC941Display::setup() {
  ESP_LOGI(TAG, "Initializing EEZ Studio UI...");
  s_hub = this->hub_;

  ui_init();

  // Wire button callbacks
  lv_obj_add_event_cb(objects.start_button, btn_start_cb, LV_EVENT_CLICKED, nullptr);
  lv_obj_add_event_cb(objects.stop_button, btn_stop_cb, LV_EVENT_CLICKED, nullptr);
  lv_obj_add_event_cb(objects.auto_button, btn_auto_cb, LV_EVENT_CLICKED, nullptr);
  lv_obj_add_event_cb(objects.manual_button, btn_manual_cb, LV_EVENT_CLICKED, nullptr);
  lv_obj_add_event_cb(objects.xfer_on_button, btn_xfer_on_cb, LV_EVENT_CLICKED, nullptr);
  lv_obj_add_event_cb(objects.xfer_off_button, btn_xfer_off_cb, LV_EVENT_CLICKED, nullptr);

  ESP_LOGI(TAG, "EEZ UI initialized, buttons wired");
}

void SmartgenHSC941Display::loop() {
  ui_tick();

  uint32_t now = millis();
  if (now - this->last_update_ms_ < UPDATE_INTERVAL_MS)
    return;
  this->last_update_ms_ = now;

  this->update_gauges_();
  this->update_engine_data_();
  this->update_power_tab_();
  this->update_alarms_();
  this->update_status_bar_();
  this->update_event_log_();
}

// ============================================================
//  Gauge Updates
// ============================================================
void SmartgenHSC941Display::update_gauges_() {
  if (!this->hub_)
    return;

  auto *vs = this->hub_->get_gen_voltage_a_sensor();
  if (vs && vs->has_state()) {
    lv_arc_set_value(objects.voltage_guage, (int) vs->state);
    char buf[8];
    snprintf(buf, sizeof(buf), "%.1f", vs->state);
    lv_label_set_text(objects.voltage_guage_label, buf);
  }

  auto *fs = this->hub_->get_gen_frequency_sensor();
  if (fs && fs->has_state()) {
    lv_arc_set_value(objects.hz_guage, (int)(fs->state * 10));
    char buf[8];
    snprintf(buf, sizeof(buf), "%.1f", fs->state);
    lv_label_set_text(objects.hz_guage_label, buf);
  }

  auto *rs = this->hub_->get_engine_speed_sensor();
  if (rs && rs->has_state()) {
    lv_arc_set_value(objects.rpm_guage, (int) rs->state);
    char buf[8];
    snprintf(buf, sizeof(buf), "%.0f", rs->state);
    lv_label_set_text(objects.rpm_guage_label, buf);
  }

  auto *ls = this->hub_->get_output_load_percent_sensor();
  if (ls && ls->has_state()) {
    lv_arc_set_value(objects.load_guage, (int) ls->state);
    char buf[8];
    snprintf(buf, sizeof(buf), "%.0f", ls->state);
    lv_label_set_text(objects.load_guage_label, buf);
  }

  // Fuel gauge + runtime estimate — pull config from web component
  sensor::Sensor *fuel_src = this->fuel_sensor_;
  float tank_size = 0;
  float burn_rate = 0;

  if (this->web_) {
    if (!fuel_src) fuel_src = this->web_->get_fuel_level_sensor();
    tank_size = this->web_->get_tank_size();
    burn_rate = this->web_->get_effective_burn_rate();
  }

  if (fuel_src && fuel_src->has_state() && tank_size > 0) {
    float pct = fuel_src->state;
    float remaining = pct * tank_size / 100.0f;
    lv_arc_set_value(objects.fuel_guage, (int) remaining);
    char buf[12];
    snprintf(buf, sizeof(buf), "%.0f", remaining);
    lv_label_set_text(objects.fuel_guage_label, buf);

    if (burn_rate > 0 && remaining > 0) {
      float hours = remaining / burn_rate;
      char rt_buf[16];
      if (hours >= 24) {
        snprintf(rt_buf, sizeof(rt_buf), "%.0fd %.0fh", floorf(hours / 24), fmodf(hours, 24));
      } else {
        snprintf(rt_buf, sizeof(rt_buf), "%.1f h", hours);
      }
      lv_label_set_text(objects.estimated_runtime_label, rt_buf);
    } else {
      lv_label_set_text(objects.estimated_runtime_label, "--");
    }
  }
}

// ============================================================
//  Engine Data Updates
// ============================================================
void SmartgenHSC941Display::update_engine_data_() {
  if (!this->hub_)
    return;

  char buf[16];

  auto *wt = this->hub_->get_water_temp_sensor();
  if (wt && wt->has_state()) {
    snprintf(buf, sizeof(buf), "%.0f °C", wt->state);
    lv_label_set_text(objects.water_temp_label, buf);
  }

  auto *op = this->hub_->get_oil_pressure_sensor();
  if (op && op->has_state()) {
    snprintf(buf, sizeof(buf), "%.0f kPa", op->state);
    lv_label_set_text(objects.oil_pressure_label, buf);
  }

  auto *bv = this->hub_->get_battery_voltage_sensor();
  if (bv && bv->has_state()) {
    snprintf(buf, sizeof(buf), "%.1f V", bv->state);
    lv_label_set_text(objects.battery_voltage_label, buf);
  }

  auto *cv = this->hub_->get_charge_voltage_sensor();
  if (cv && cv->has_state()) {
    snprintf(buf, sizeof(buf), "%.1f V", cv->state);
    lv_label_set_text(objects.charge_voltage_label, buf);
  }

  auto *aux = this->hub_->get_aux_sensor_1();
  if (aux && aux->has_state()) {
    snprintf(buf, sizeof(buf), "%.1f", aux->state);
    lv_label_set_text(objects.aux_sensor_label, buf);
  }

  auto *es = this->hub_->get_engine_running_status_sensor();
  if (es && es->has_state()) {
    int state = (int) es->state;
    const char *text = "Unknown";
    switch (state) {
      case 0: text = "Stopped"; break;
      case 1: text = "Pre-Heat"; break;
      case 2: text = "Pre-Crank"; break;
      case 3: text = "Cranking"; break;
      case 4: text = "Safety On"; break;
      case 5: text = "Cooling Down"; break;
      case 6: text = "Running"; break;
    }
    lv_label_set_text(objects.engine_state_label, text);
  }

  if (this->hub_->is_in_auto_mode())
    lv_label_set_text(objects.auto_state_label, "Auto");
  else if (this->hub_->is_in_manual_mode())
    lv_label_set_text(objects.auto_state_label, "Manual");
  else if (this->hub_->is_in_stop_mode())
    lv_label_set_text(objects.auto_state_label, "Stop");

  // Highlight active buttons
  // Stop/Auto/Manual are mutually exclusive modes
  set_btn_active(objects.stop_button, this->hub_->is_in_stop_mode());
  set_btn_active(objects.auto_button, this->hub_->is_in_auto_mode());
  set_btn_active(objects.manual_button, this->hub_->is_in_manual_mode());

  // Disable start button in stop/auto mode (engine controlled automatically or stopped)
  bool can_start = this->hub_->is_in_manual_mode();
  if (can_start) {
    lv_obj_clear_state(objects.start_button, LV_STATE_DISABLED);
    lv_obj_set_style_opa(objects.start_button, LV_OPA_COVER, 0);
  } else {
    lv_obj_add_state(objects.start_button, LV_STATE_DISABLED);
    lv_obj_set_style_opa(objects.start_button, LV_OPA_40, 0);
  }

  // Transfer switch — only in manual mode
  set_btn_active(objects.xfer_on_button, this->hub_->is_gen_on_load());
  set_btn_active(objects.xfer_off_button, !this->hub_->is_gen_on_load());

  if (can_start) {
    lv_obj_clear_state(objects.xfer_on_button, LV_STATE_DISABLED);
    lv_obj_clear_state(objects.xfer_off_button, LV_STATE_DISABLED);
    lv_obj_set_style_opa(objects.xfer_on_button, LV_OPA_COVER, 0);
    lv_obj_set_style_opa(objects.xfer_off_button, LV_OPA_COVER, 0);
  } else {
    lv_obj_add_state(objects.xfer_on_button, LV_STATE_DISABLED);
    lv_obj_add_state(objects.xfer_off_button, LV_STATE_DISABLED);
    lv_obj_set_style_opa(objects.xfer_on_button, LV_OPA_40, 0);
    lv_obj_set_style_opa(objects.xfer_off_button, LV_OPA_40, 0);
  }
}

// ============================================================
//  Power Tab Updates
// ============================================================
void SmartgenHSC941Display::update_power_tab_() {
  if (!this->hub_)
    return;

  char buf[24];
  bool single = this->web_ ? this->web_->is_single_phase() : true;

  // L1 panel — voltage, current + power
  auto *va = this->hub_->get_gen_voltage_a_sensor();
  if (va && va->has_state()) {
    snprintf(buf, sizeof(buf), "%.1f V", va->state);
    lv_label_set_text(objects.l1_volts_label, buf);
  }
  auto *ia = this->hub_->get_phase_a_current_sensor();
  auto *pa = this->hub_->get_phase_a_active_power_sensor();
  {
    float amps = (ia && ia->has_state()) ? ia->state : 0;
    float kw = (pa && pa->has_state()) ? pa->state : 0;
    snprintf(buf, sizeof(buf), "%.1fA | %.1fkW", amps, kw);
    lv_label_set_text(objects.l1_amp_watt_label, buf);
  }

  // Split phase: L1 + L2 visible, L3 hidden
  // 3-phase: all visible
  if (single) {
    // Split phase — show L2, hide L3
    lv_obj_clear_flag(objects.l2_panel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.l3_panel, LV_OBJ_FLAG_HIDDEN);

    // Show L1-L2 (240V), hide L1-L3 and L2-L3
    lv_obj_clear_flag(objects.l1_l2_panel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.l1_l3_panel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_add_flag(objects.l2_l3_panel, LV_OBJ_FLAG_HIDDEN);
  } else {
    lv_obj_clear_flag(objects.l2_panel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(objects.l3_panel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(objects.l1_l2_panel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(objects.l1_l3_panel, LV_OBJ_FLAG_HIDDEN);
    lv_obj_clear_flag(objects.l2_l3_panel, LV_OBJ_FLAG_HIDDEN);
  }

  // L2 panel
  auto *vb = this->hub_->get_gen_voltage_b_sensor();
  if (vb && vb->has_state()) {
    snprintf(buf, sizeof(buf), "%.1f V", vb->state);
    lv_label_set_text(objects.l2_volts_label, buf);
  }
  {
    auto *ib = this->hub_->get_phase_b_current_sensor();
    auto *pb = this->hub_->get_phase_b_active_power_sensor();
    float amps = (ib && ib->has_state()) ? ib->state : 0;
    float kw = (pb && pb->has_state()) ? pb->state : 0;
    snprintf(buf, sizeof(buf), "%.1fA | %.1fkW", amps, kw);
    lv_label_set_text(objects.l2_amp_watt_label, buf);
  }

  // Line-line voltages
  auto *vab = this->hub_->get_gen_voltage_ab_sensor();
  if (vab && vab->has_state()) {
    snprintf(buf, sizeof(buf), "%.0f V", vab->state);
    lv_label_set_text(objects.l1_l2_label, buf);
  }
  if (!single) {
    auto *vbc = this->hub_->get_gen_voltage_bc_sensor();
    if (vbc && vbc->has_state()) {
      snprintf(buf, sizeof(buf), "%.0f V", vbc->state);
      lv_label_set_text(objects.l1_l3_label, buf);
    }
    auto *vca = this->hub_->get_gen_voltage_ca_sensor();
    if (vca && vca->has_state()) {
      snprintf(buf, sizeof(buf), "%.0f V", vca->state);
      lv_label_set_text(objects.l2_l3_label, buf);
    }
  }

  // Total active power
  auto *tp = this->hub_->get_total_active_power_sensor();
  if (tp && tp->has_state()) {
    snprintf(buf, sizeof(buf), "%.1f kW", tp->state);
    lv_label_set_text(objects.active_power_label, buf);
  }

  // Reactive power
  auto *rp = this->hub_->get_reactive_power_sensor();
  if (rp && rp->has_state()) {
    snprintf(buf, sizeof(buf), "%.1f kVAR", rp->state);
    lv_label_set_text(objects.reactive_power_label, buf);
  }

  // Apparent power
  auto *ap = this->hub_->get_apparent_power_sensor();
  if (ap && ap->has_state()) {
    snprintf(buf, sizeof(buf), "%.1f kVA", ap->state);
    lv_label_set_text(objects.apparent_power_label, buf);
  }

  // Power factor
  auto *pf = this->hub_->get_power_factor_sensor();
  if (pf && pf->has_state()) {
    snprintf(buf, sizeof(buf), "%.2f", pf->state);
    lv_label_set_text(objects.power_factor_label, buf);
  }

  // Frequency
  auto *freq = this->hub_->get_gen_frequency_sensor();
  if (freq && freq->has_state()) {
    snprintf(buf, sizeof(buf), "%.1f Hz", freq->state);
    lv_label_set_text(objects.frequency_label, buf);
  }
}

// ============================================================
//  Alarm Updates
// ============================================================
void SmartgenHSC941Display::update_alarms_() {
  if (!this->hub_)
    return;

  // Build a bitmask of current alarm state to avoid unnecessary redraws
  uint8_t alarm_state = 0;
  if (this->hub_->is_any_shutdown()) alarm_state |= 1;
  if (this->hub_->is_emergency_stop()) alarm_state |= 2;
  if (this->hub_->is_crank_failure()) alarm_state |= 4;

  if (alarm_state == this->prev_alarm_state_)
    return;
  this->prev_alarm_state_ = alarm_state;

  lv_obj_clean(objects.alarm_list);

  auto add_alarm = [](lv_obj_t *list, const char *text, uint32_t color) {
    lv_obj_t *item = lv_list_add_text(list, text);
    lv_obj_set_style_bg_color(item, lv_color_hex(0x282b30), 0);
    lv_obj_set_style_bg_opa(item, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(item, lv_color_hex(color), 0);
  };

  if (alarm_state & 1)
    add_alarm(objects.alarm_list, LV_SYMBOL_WARNING " SHUTDOWN ACTIVE", 0xef4444);
  if (alarm_state & 2)
    add_alarm(objects.alarm_list, LV_SYMBOL_WARNING " EMERGENCY STOP", 0xef4444);
  if (alarm_state & 4)
    add_alarm(objects.alarm_list, LV_SYMBOL_WARNING " Crank Failure", 0xf59e0b);

  if (alarm_state == 0)
    add_alarm(objects.alarm_list, LV_SYMBOL_OK " All Normal", 0x22c55e);
}

// ============================================================
//  Status Bar Updates
// ============================================================
void SmartgenHSC941Display::update_status_bar_() {
  if (!this->hub_)
    return;

  // Serial connectivity
  if (this->hub_->is_connected()) {
    lv_label_set_text(objects.header_serial_connectivity_status, LV_SYMBOL_OK " Connected");
    lv_obj_set_style_text_color(objects.header_serial_connectivity_status,
                                 lv_color_hex(0x22c55e), 0);
    lv_obj_set_style_border_color(objects.header_status_panel_1,
                                   lv_color_hex(0x22c55e), 0);
  } else {
    lv_label_set_text(objects.header_serial_connectivity_status, LV_SYMBOL_CLOSE " Disconnected");
    lv_obj_set_style_text_color(objects.header_serial_connectivity_status,
                                 lv_color_hex(0xef4444), 0);
    lv_obj_set_style_border_color(objects.header_status_panel_1,
                                   lv_color_hex(0xef4444), 0);
  }

  // Mode / alarm status
  const char *status_text = "Offline";
  uint32_t status_color = 0xef4444;

  if (this->hub_->is_connected()) {
    if (this->hub_->is_any_shutdown()) {
      status_text = LV_SYMBOL_WARNING " ALARM";
      status_color = 0xef4444;
    } else if (this->hub_->is_in_auto_mode()) {
      status_text = "Auto";
      status_color = 0x22c55e;
    } else if (this->hub_->is_in_manual_mode()) {
      status_text = "Manual";
      status_color = 0xf59e0b;
    } else if (this->hub_->is_in_stop_mode()) {
      status_text = "Stop";
      status_color = 0xef4444;
    }
  }

  lv_label_set_text(objects.header_status_text, status_text);
  lv_obj_set_style_text_color(objects.header_status_text, lv_color_hex(status_color), 0);
  lv_obj_set_style_border_color(objects.header_status_panel, lv_color_hex(status_color), 0);

  // Time
  auto now = ::time(nullptr);
  auto *tm = ::localtime(&now);
  if (tm->tm_year > 100) {
    char buf[12];
    int hour = tm->tm_hour % 12;
    if (hour == 0) hour = 12;
    snprintf(buf, sizeof(buf), "%d:%02d %s",
             hour, tm->tm_min,
             tm->tm_hour >= 12 ? "PM" : "AM");
    lv_label_set_text(objects.header_time_label, buf);
  }
}

// ============================================================
//  Event Log — reads from web component's shared log
// ============================================================
void SmartgenHSC941Display::update_event_log_() {
  if (!this->web_)
    return;

  auto &log = this->web_->get_event_log();

  // Only rebuild if count changed
  if (log.size() == this->prev_event_count_)
    return;
  this->prev_event_count_ = log.size();

  lv_obj_clean(objects.event_log_list);

  // Show most recent events first (up to 50)
  size_t start = log.size() > 50 ? log.size() - 50 : 0;
  for (size_t i = log.size(); i > start; i--) {
    auto &entry = log[i - 1];

    // Format timestamp with date
    auto *tm = localtime(&entry.timestamp);
    char buf[128];
    if (tm->tm_year > 100) {
      snprintf(buf, sizeof(buf), "%02d/%02d %02d:%02d:%02d  %s",
               tm->tm_mon + 1, tm->tm_mday,
               tm->tm_hour, tm->tm_min, tm->tm_sec,
               entry.message.c_str());
    } else {
      snprintf(buf, sizeof(buf), "%s", entry.message.c_str());
    }

    lv_obj_t *item = lv_list_add_text(objects.event_log_list, buf);
    lv_obj_set_style_bg_color(item, lv_color_hex(0x282b30), 0);
    lv_obj_set_style_bg_opa(item, LV_OPA_COVER, 0);

    // Color-code: red for errors, amber for warnings, default for info
    auto &msg = entry.message;
    uint32_t color = 0x9a9ea6;  // default grey
    if (msg.find("SHUTDOWN") != std::string::npos ||
        msg.find("EMERGENCY") != std::string::npos ||
        msg.find("FAILED") != std::string::npos) {
      color = 0xef4444;  // red
    } else if (msg.find("WARNING") != std::string::npos ||
               msg.find("warning") != std::string::npos ||
               msg.find("STOP mode") != std::string::npos) {
      color = 0xf59e0b;  // amber
    } else if (msg.find("cleared") != std::string::npos ||
               msg.find("started") != std::string::npos ||
               msg.find("Running") != std::string::npos) {
      color = 0x22c55e;  // green
    }
    lv_obj_set_style_text_color(item, lv_color_hex(color), 0);
  }

  if (log.empty()) {
    lv_obj_t *item = lv_list_add_text(objects.event_log_list, "No events");
    lv_obj_set_style_bg_color(item, lv_color_hex(0x282b30), 0);
    lv_obj_set_style_bg_opa(item, LV_OPA_COVER, 0);
    lv_obj_set_style_text_color(item, lv_color_hex(0x6b7394), 0);
  }
}

}  // namespace smartgen_hsc941_display
}  // namespace esphome
