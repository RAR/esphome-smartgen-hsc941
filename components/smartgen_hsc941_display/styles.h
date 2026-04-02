#ifndef EEZ_LVGL_UI_STYLES_H
#define EEZ_LVGL_UI_STYLES_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

// Style: Guages Container
void add_style_guages_container(lv_obj_t *obj);
void remove_style_guages_container(lv_obj_t *obj);

// Style: Guage
void add_style_guage(lv_obj_t *obj);
void remove_style_guage(lv_obj_t *obj);

// Style: Guage Panel
void add_style_guage_panel(lv_obj_t *obj);
void remove_style_guage_panel(lv_obj_t *obj);

#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_STYLES_H*/