#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl/lvgl.h>

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
    lv_obj_t *monitoring;
    lv_obj_t *voltage_guage_container;
    lv_obj_t *volt_guage_panel;
    lv_obj_t *obj0;
    lv_obj_t *voltage_guage;
    lv_obj_t *voltage_guage_label;
    lv_obj_t *obj1;
    lv_obj_t *frequency_guage_container;
    lv_obj_t *freq_guage_panel;
    lv_obj_t *obj2;
    lv_obj_t *hz_guage;
    lv_obj_t *hz_guage_label;
    lv_obj_t *obj3;
    lv_obj_t *rpm_guage_container;
    lv_obj_t *rpm_guage_panel;
    lv_obj_t *obj4;
    lv_obj_t *rpm_guage;
    lv_obj_t *rpm_guage_label;
    lv_obj_t *obj5;
    lv_obj_t *load_guage_container;
    lv_obj_t *load_guage_panel;
    lv_obj_t *obj6;
    lv_obj_t *load_guage;
    lv_obj_t *load_guage_label;
    lv_obj_t *obj7;
    lv_obj_t *controls;
    lv_obj_t *event_log;
    lv_obj_t *header_label;
    lv_obj_t *header_status_panel;
    lv_obj_t *header_status_text;
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