#ifndef EEZ_LVGL_UI_SCREENS_H
#define EEZ_LVGL_UI_SCREENS_H

#include <lvgl.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _objects_t {
    lv_obj_t *main;
    lv_obj_t *configuration;
    lv_obj_t *obj0;
    lv_obj_t *obj1;
    lv_obj_t *pnl_temperature;
    lv_obj_t *bar_puissance;
    lv_obj_t *bar_tension;
    lv_obj_t *arc_speed;
    lv_obj_t *scale_speed;
    lv_obj_t *lb_speed;
    lv_obj_t *lv_pourcent_bat;
    lv_obj_t *lb_pourcent_trot;
    lv_obj_t *lb_temperature;
    lv_obj_t *lb_tension;
    lv_obj_t *lb_puissance;
    lv_obj_t *lb_symbole_temperature;
    lv_obj_t *lb_unite_vitesse;
    lv_obj_t *imgbtn_config;
    lv_obj_t *obj2;
    lv_obj_t *lb_accel;
    lv_obj_t *obj3;
    lv_obj_t *btn_vitesse;
    lv_obj_t *lb_configuration;
    lv_obj_t *lb_conf_vitesse;
    lv_obj_t *lb_conf_choix_vitesse;
    lv_obj_t *lb_conf_choix_vitesse_1;
    lv_obj_t *lb_conf_choix_vitesse_2;
    lv_obj_t *lb_conf_vitesse_1;
    lv_obj_t *lb_conf_vitesse_2;
    lv_obj_t *btn_retour_main;
    lv_obj_t *btn_heure;
    lv_obj_t *btn_temperature;
} objects_t;

extern objects_t objects;

enum ScreensEnum {
    SCREEN_ID_MAIN = 1,
    SCREEN_ID_CONFIGURATION = 2,
};

void create_screen_main();
void tick_screen_main();

void create_screen_configuration();
void tick_screen_configuration();

void tick_screen_by_id(enum ScreensEnum screenId);
void tick_screen(int screen_index);

void create_screens();


#ifdef __cplusplus
}
#endif

#endif /*EEZ_LVGL_UI_SCREENS_H*/