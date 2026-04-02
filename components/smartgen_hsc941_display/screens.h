#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Screens

enum ScreensEnum {
    _SCREEN_ID_FIRST = 1,
    SCREEN_ID_MAIN = 1,
    _SCREEN_ID_LAST = 1
};

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *tabs;
    lv_obj_t *overview;
    lv_obj_t *guage_container;
    lv_obj_t *volt_guage_panel;
    lv_obj_t *obj0;
    lv_obj_t *voltage_guage;
    lv_obj_t *voltage_guage_label;
    lv_obj_t *obj1;
    lv_obj_t *freq_guage_panel;
    lv_obj_t *obj2;
    lv_obj_t *hz_guage;
    lv_obj_t *hz_guage_label;
    lv_obj_t *obj3;
    lv_obj_t *rpm_guage_panel;
    lv_obj_t *obj4;
    lv_obj_t *rpm_guage;
    lv_obj_t *rpm_guage_label;
    lv_obj_t *obj5;
    lv_obj_t *hz_guage_panel;
    lv_obj_t *obj6;
    lv_obj_t *load_guage;
    lv_obj_t *load_guage_label;
    lv_obj_t *obj7;
    lv_obj_t *hz_guage_panel_1;
    lv_obj_t *obj8;
    lv_obj_t *fuel_guage;
    lv_obj_t *fuel_guage_label;
    lv_obj_t *obj9;
    lv_obj_t *obj10;
    lv_obj_t *obj11;
    lv_obj_t *water_temp_label;
    lv_obj_t *obj12;
    lv_obj_t *oil_pressure_label;
    lv_obj_t *obj13;
    lv_obj_t *battery_voltage_label;
    lv_obj_t *obj14;
    lv_obj_t *charge_voltage_label;
    lv_obj_t *obj15;
    lv_obj_t *aux_sensor_label;
    lv_obj_t *obj16;
    lv_obj_t *engine_state_label;
    lv_obj_t *obj17;
    lv_obj_t *auto_state_label;
    lv_obj_t *obj18;
    lv_obj_t *estimated_runtime_label;
    lv_obj_t *obj19;
    lv_obj_t *alarm_list;
    lv_obj_t *l1_panel;
    lv_obj_t *l1_label;
    lv_obj_t *l1_volts_label;
    lv_obj_t *l1_amp_watt_label;
    lv_obj_t *l2_panel;
    lv_obj_t *l1_label_1;
    lv_obj_t *l2_volts_label;
    lv_obj_t *l2_amp_watt_label;
    lv_obj_t *l3_panel;
    lv_obj_t *l1_label_2;
    lv_obj_t *l3_volt_label;
    lv_obj_t *l3_amp_watt_label;
    lv_obj_t *l1_l2_panel;
    lv_obj_t *obj20;
    lv_obj_t *l1_l2_label;
    lv_obj_t *l1_l3_panel;
    lv_obj_t *obj21;
    lv_obj_t *l1_l3_label;
    lv_obj_t *l2_l3_panel;
    lv_obj_t *obj22;
    lv_obj_t *l2_l3_label;
    lv_obj_t *obj23;
    lv_obj_t *active_power_label;
    lv_obj_t *obj24;
    lv_obj_t *reactive_power_label;
    lv_obj_t *obj25;
    lv_obj_t *apparent_power_label;
    lv_obj_t *obj26;
    lv_obj_t *power_factor_label;
    lv_obj_t *obj27;
    lv_obj_t *frequency_label;
    lv_obj_t *event_log;
    lv_obj_t *event_log_list;
    lv_obj_t *button_container;
    lv_obj_t *start_button;
    lv_obj_t *obj28;
    lv_obj_t *obj29;
    lv_obj_t *stop_button;
    lv_obj_t *obj30;
    lv_obj_t *auto_button;
    lv_obj_t *obj31;
    lv_obj_t *manual_button;
    lv_obj_t *obj32;
    lv_obj_t *obj33;
    lv_obj_t *xfer_on_button;
    lv_obj_t *obj34;
    lv_obj_t *xfer_off_button;
    lv_obj_t *obj35;
    lv_obj_t *header_status_panel_1;
    lv_obj_t *header_serial_connectivity_status;
    lv_obj_t *header_status_panel;
    lv_obj_t *header_status_text;
    lv_obj_t *header_label;
    lv_obj_t *header_time_label;
} objects_t;

extern objects_t objects;

void create_screen_main();
void tick_screen_main();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/