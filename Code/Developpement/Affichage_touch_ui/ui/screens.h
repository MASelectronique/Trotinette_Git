#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *lb_speed;
    lv_obj_t *arc_speed;
    lv_obj_t *bar_tension;
    lv_obj_t *lb_tension;
    lv_obj_t *lb_puissance;
    lv_obj_t *scale_speed;
    lv_obj_t *bar_puissance;
    lv_obj_t *lb_temperature;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
};

void create_screen_main();
void tick_screen_main();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/