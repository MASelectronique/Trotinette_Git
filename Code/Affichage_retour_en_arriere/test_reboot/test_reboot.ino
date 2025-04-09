/*Using LVGL with Arduino requires some extra steps:
 *Be sure to read the docs here: https://docs.lvgl.io/master/integration/framework/arduino.html  */

 #include <lvgl.h>
 #include <Arduino.h>
 #include <Wire.h>
 //#include <SPI.h>
 #include <math.h>
 #include <TFT_eSPI.h>      //Librairie pour l'écran tactile
 #include "esp_timer.h"
 //#include "ui.h"
 #include "icon_config.c"
 
 #define TFT_HOR_RES   240
 #define TFT_VER_RES   320
 #define TFT_ROTATION  LV_DISPLAY_ROTATION_0

 #define LED_PIN 2
 
 #define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))
 uint32_t draw_buf[DRAW_BUF_SIZE / 4];
 
 lv_obj_t *speed_arc;
 lv_obj_t *speed_text;
 lv_obj_t *btn_conf;
 //lv_obj_t *icon_config;
  
 static lv_style_t style_btn;
 static lv_style_t style_button_pressed;
 static lv_style_t style_button_red;
 String text = "";
 

 static lv_color_t darken(const lv_color_filter_dsc_t * dsc, lv_color_t color, lv_opa_t opa)
{
    LV_UNUSED(dsc);
    return lv_color_darken(color, opa);
}
 
 void update_speed_gauge(int speed) {
     lv_arc_set_value(speed_arc, speed);
     text = String(speed);
     lv_label_set_text(speed_text,text.c_str());
 }
  void create_speed_gauge() {
     speed_arc = lv_arc_create(lv_scr_act());
     lv_obj_set_size(speed_arc, 200, 200);
     lv_arc_set_rotation(speed_arc, 135);
     lv_arc_set_bg_angles(speed_arc, 0, 270);
     lv_arc_set_range(speed_arc, 0, 100);
     lv_arc_set_value(speed_arc, 0);
     lv_obj_center(speed_arc);
     lv_obj_remove_style(speed_arc, NULL, LV_PART_KNOB);

    // Créer un label simple
    speed_text = lv_label_create(lv_scr_act());  // Utiliser lv_scr_act() pour obtenir l'écran actif
    lv_label_set_text(speed_text, "0");
    lv_obj_align(speed_text, LV_ALIGN_CENTER, 0, 0);
     

 }
 /*void create_button_config() {
   //Create an image button//
   //Create a transition animation on width transformation and recolor.//
   static lv_style_prop_t tr_prop[] = {LV_STYLE_TRANSFORM_WIDTH, LV_STYLE_IMAGE_RECOLOR_OPA, 0};
   static lv_style_transition_dsc_t tr;
   lv_style_transition_dsc_init(&tr, tr_prop, lv_anim_path_linear, 200, 0, NULL);

   static lv_style_t style_def;
   lv_style_init(&style_def);
   lv_style_set_text_color(&style_def, lv_color_white());
   lv_style_set_transition(&style_def, &tr);

   //Darken the button when pressed and make it wider//
   static lv_style_t style_pr;
   lv_style_init(&style_pr);
   lv_style_set_image_recolor_opa(&style_pr, LV_OPA_30);
   lv_style_set_image_recolor(&style_pr, lv_color_black());
   lv_style_set_transform_width(&style_pr, 20);

   lv_obj_t * imageBtnConfig = lv_imagebutton_create(lv_screen_active());
   lv_imagebutton_set_src(imageBtnConfig, LV_IMAGEBUTTON_STATE_RELEASED, NULL,&icon_config,NULL);
   lv_imagebutton_set_src(imageBtnConfig, LV_IMAGEBUTTON_STATE_PRESSED, NULL,&icon_config,NULL);
   lv_obj_add_style(imageBtnConfig, &style_def, 0);
   lv_obj_add_style(imageBtnConfig, &style_pr, LV_STATE_PRESSED);

   lv_obj_set_width(imageBtnConfig, 100);
   lv_obj_align(imageBtnConfig, LV_ALIGN_TOP_LEFT, 0, 0);
}
*/

 #if LV_USE_LOG != 0
 void my_print( lv_log_level_t level, const char * buf ) {
     LV_UNUSED(level);
     Serial.println(buf);
     Serial.flush();
 }
 #endif
 
 void my_disp_flush( lv_display_t *disp, const lv_area_t *area, uint8_t * px_map) {
     lv_display_flush_ready(disp);
 }
 
 void my_touchpad_read( lv_indev_t * indev, lv_indev_data_t * data ) {}
 
 static uint32_t my_tick(void) {
     return millis();
 }
 static void style_init(void)
{
    /*Create a simple button style*/
    lv_style_init(&style_btn);
    lv_style_set_radius(&style_btn, 10);
    lv_style_set_bg_opa(&style_btn, LV_OPA_COVER);
    lv_style_set_bg_color(&style_btn, lv_palette_lighten(LV_PALETTE_GREY, 3));
    lv_style_set_bg_grad_color(&style_btn, lv_palette_main(LV_PALETTE_GREY));
    lv_style_set_bg_grad_dir(&style_btn, LV_GRAD_DIR_VER);

    lv_style_set_border_color(&style_btn, lv_color_black());
    lv_style_set_border_opa(&style_btn, LV_OPA_20);
    lv_style_set_border_width(&style_btn, 2);

    lv_style_set_text_color(&style_btn, lv_color_black());

    /*Create a style for the pressed state.
     *Use a color filter to simply modify all colors in this state*/
    static lv_color_filter_dsc_t color_filter;
    lv_color_filter_dsc_init(&color_filter, darken);
    lv_style_init(&style_button_pressed);
    lv_style_set_color_filter_dsc(&style_button_pressed, &color_filter);
    lv_style_set_color_filter_opa(&style_button_pressed, LV_OPA_20);

    /*Create a red style. Change only some colors.*/
    lv_style_init(&style_button_red);
    lv_style_set_bg_color(&style_button_red, lv_palette_main(LV_PALETTE_RED));
    lv_style_set_bg_grad_color(&style_button_red, lv_palette_lighten(LV_PALETTE_RED, 3));
}

/**
 * Create styles from scratch for buttons.
 */
void lv_example_get_started_3(void)
{
    /*Initialize the style*/
    style_init();

    /*Create a button and use the new styles*/
    lv_obj_t * btn = lv_button_create(lv_screen_active());
    /* Remove the styles coming from the theme
     * Note that size and position are also stored as style properties
     * so lv_obj_remove_style_all will remove the set size and position too */
    lv_obj_remove_style_all(btn);
    lv_obj_set_pos(btn, 10, 10);
    lv_obj_set_size(btn, 120, 50);
    lv_obj_add_style(btn, &style_btn, 0);
    lv_obj_add_style(btn, &style_button_pressed, LV_STATE_PRESSED);

    /*Add a label to the button*/
    lv_obj_t * label = lv_label_create(btn);
    lv_label_set_text(label, "Button");
    lv_obj_center(label);

    /*Create another button and use the red style too*/
    lv_obj_t * btn2 = lv_button_create(lv_screen_active());
    lv_obj_remove_style_all(btn2);                      /*Remove the styles coming from the theme*/
    lv_obj_set_pos(btn2, 10, 80);
    lv_obj_set_size(btn2, 120, 50);
    lv_obj_add_style(btn2, &style_btn, 0);
    lv_obj_add_style(btn2, &style_button_red, 0);
    lv_obj_add_style(btn2, &style_button_pressed, LV_STATE_PRESSED);
    lv_obj_set_style_radius(btn2, LV_RADIUS_CIRCLE, 0); /*Add a local style too*/

    label = lv_label_create(btn2);
    lv_label_set_text(label, "Button 2");
    lv_obj_center(label);
}
 
 //objects_t objects;
 
 void setup() {
     Serial.begin(115200);
     Serial.println("LVGL Initialized");
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN,LOW);
 
     lv_init();
     lv_tick_set_cb(my_tick);
 
     #if LV_USE_LOG != 0
         lv_log_register_print_cb(my_print);
     #endif
 
     lv_display_t * disp;
     disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf, sizeof(draw_buf));
     lv_display_set_rotation(disp, TFT_ROTATION);
 
     lv_indev_t * indev = lv_indev_create();
     lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);
     lv_indev_set_read_cb(indev, my_touchpad_read);

 

     lv_example_get_started_3();
     //create_speed_gauge();
 }
 
 void loop() {
     //static int speed = 0;
     //speed = (speed + 5) % 101;
     //update_speed_gauge(speed);
     lv_timer_handler();
     delay(500);
 }
 