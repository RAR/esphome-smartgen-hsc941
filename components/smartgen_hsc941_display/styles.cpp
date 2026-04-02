#include "styles.h"
#include "images.h"
#include "fonts.h"

#include "ui.h"
#include "screens.h"

//
// Style: Guages Container
//

void add_style_guages_container(lv_obj_t *obj) {
    (void)obj;
};

void remove_style_guages_container(lv_obj_t *obj) {
    (void)obj;
};

//
// Style: Guage
//

void add_style_guage(lv_obj_t *obj) {
    (void)obj;
};

void remove_style_guage(lv_obj_t *obj) {
    (void)obj;
};

//
// Style: Guage Panel
//

void add_style_guage_panel(lv_obj_t *obj) {
    (void)obj;
};

void remove_style_guage_panel(lv_obj_t *obj) {
    (void)obj;
};

//
//
//

void add_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*AddStyleFunc)(lv_obj_t *obj);
    static const AddStyleFunc add_style_funcs[] = {
        add_style_guages_container,
        add_style_guage,
        add_style_guage_panel,
    };
    add_style_funcs[styleIndex](obj);
}

void remove_style(lv_obj_t *obj, int32_t styleIndex) {
    typedef void (*RemoveStyleFunc)(lv_obj_t *obj);
    static const RemoveStyleFunc remove_style_funcs[] = {
        remove_style_guages_container,
        remove_style_guage,
        remove_style_guage_panel,
    };
    remove_style_funcs[styleIndex](obj);
}