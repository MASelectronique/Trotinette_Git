#ifndef TROTTINETTE_AFFICHAGE_H
#define TROTTINETTE_AFFICHAGE_H

#include <lvgl.h>
#include <Arduino.h> // Pour millis()
#include <TFT_eSPI.h>         //Librairie pour l'écran tactile

// Fonctions de base LVGL
void my_disp_flush(lv_display_t *disp, const lv_area_t *area, uint8_t *px_map);
//void my_touchpad_read(lv_indev_t * indev, lv_indev_data_t * data);
//uint32_t my_tick(void);

/*
// Fonctions de gestion de données
void generate_data(void);
void refresh_data_affichage(void);
void temperature_refresh(int temperature);
void gestion_btn_conf(void);
*/

// Événements de boutons
//void event_handler_imgbtn_config(lv_event_t * e);
//void event_handler_btn_retour_main(lv_event_t * e);
//void event_handler_btn_vitesse(lv_event_t * e);
#endif


////////////////////////////////     DEFINE     /////////////////////////
//Température 
#define LOW_TEMPERATURE 60 //Limite pour l'affichege de la température devienne jaune
#define HIGH_TEMPERATURE 75//Limite pour l'affichege de la température devienne rouge

//Widget arc (cadran de la vitesse)
#define arc_min 0
#define arc_max_kmh 50
#define arc_max_mph 33

//Batterie
#define bar_tension_min 0
#define bar_tension_max 100

#define color_low_temp 0x0bff00 // Vert
#define color_med_temp 0xfaff00 // Jaune
#define color_high_temp 0xff0000 // Rouge

#define bar_cmd_min 0
#define bar_cmd_max 100

//Set to your screen resolution and rotation
#define TFT_HOR_RES   240
#define TFT_VER_RES   320
#define TFT_ROTATION  LV_DISPLAY_ROTATION_0

//LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))

//Pattes pour ecran
#define LED_PIN 2 //Patte qui controle la backlight de l'ecran

//Pattes pour communication UART
#define RX_PIN 18
#define TX_PIN 17

//Patte pour detection de chute
#define FALL_DETECT_PIN 15

//Pattes pour l'accelerometre
#define SDA_PIN 8
#define SCL_PIN 9