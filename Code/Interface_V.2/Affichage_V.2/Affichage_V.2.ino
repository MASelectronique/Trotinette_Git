/*
À faire :   
          -Séparer toutes les variables globales en constantes et en objects
          -Modifier les fonctions pour pouvoir envoyer l'object en paramètre
          -Changer le fonctionnement des bouton dans configuration
          -Supprimer le bouton de configuration de l'heure
          -define en majuscule
          -lib string besoin?
          -Au lieu de la fonction generate_random pour créer un affichage où les valeurs changent, d'écortiquer le JSON envoyé par le PCB de contrôle du client

Brief : Ce code permet un d'afficher sur l'écran 
*/

#include <lvgl.h>
#include <string.h> // pour l'affichage graphique de text
#include <TFT_eSPI.h>      //Librairie pour l'écran tactile
#include <ui.h> // Fichier généré par EEZ studio
#include <SoftwareSerial.h>

//#include "trottinetteAccelerometer.h"
//#include "trottinetteData.h"

#define BYTE_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565))

//Température
#define LOW_TEMPERATURE 60
#define HIGH_TEMPERATURE 75

//Widget arc
#define arc_min 0
#define arc_max 50

#define bar_tension_min 0
#define bar_tension_max 100

#define color_low_temp 0x0bff00 // Vert
#define color_med_temp 0xfaff00 // Jaune
#define color_high_temp 0xff0000 // Rouge

#define bar_puissance_min 0
#define bar_puissance_max 100


//Set to your screen resolution and rotation
#define TFT_HOR_RES   240
#define TFT_VER_RES   320
#define TFT_ROTATION  LV_DISPLAY_ROTATION_0

#define LED_PIN 2

//LVGL draw into this buffer, 1/10 screen size usually works well. The size is in bytes
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))

uint32_t draw_buf[DRAW_BUF_SIZE / 4];

const char vitesse_kmh [5] = "Km/h";
const char vitesse_mph [5]="mph";
const char* symbole_celsius = "°C";
const char* symbole_Fahrenheit = "°F";

//Touch screen
uint16_t calData[5] = { 243, 3618, 357, 3612, 4 };  //Valeurs de calibration de l'écran

//Vitesse
float_t vitesseFloat=0;
char vitesseChar[4]="";
char vitesse_texte [5]="";

//Température
float_t temperatureFloat = 0;
char temperatureChar[4]="";
char symbole_temperature[4] = "°C";

//Charge de la Batterie
float_t tensionFloat = 90;
char tensionChar[3]="";

//Puissance
int32_t bar_puissance=0;
float_t puissanceFloat = 0;
char puissanceChar[3]="";

TFT_eSPI tft = TFT_eSPI(); //Déclare un objet pour l'écran tactile

//Elle redéfinit la manière dont les messages de log sont affichés en utilisant Serial.println sur un microcontrôleur (comme un ESP32 ou un Arduino).
#if LV_USE_LOG != 0
void my_print( lv_log_level_t level, const char * buf )
{
    LV_UNUSED(level);
    Serial.println(buf);
    Serial.flush();
}
#endif

// LVGL l'appelle lorsqu'une image rendue doit être copiée sur l'affichage //
void my_disp_flush( lv_display_t *disp, const lv_area_t *area, uint8_t * px_map)
{
    lv_display_flush_ready(disp);
}

// Utiliser millis() d'Arduino comme source de tic 
static uint32_t my_tick(void)
{
    return millis();
}

//Permet de générer des valeurs pour créer un affichage en mouvement 
void generate_random(void)
{
    if (vitesseFloat==arc_max)
    {
      vitesseFloat=0;
    }
    vitesseFloat++;

    if (temperatureFloat==100)
    {
      temperatureFloat=0;
    }
    temperatureFloat++;
    temperatureFloat++;

    if (tensionFloat<=bar_tension_min)
    {
      tensionFloat=bar_tension_max;
    }
    tensionFloat= tensionFloat -2;

    if (puissanceFloat>=bar_puissance_max)
    {
      puissanceFloat=bar_puissance_min;
    }
    puissanceFloat=puissanceFloat + 2;
}

//Permet le rapfraichissement des valeurs et couleur de l'affichage de la température sans avoir à décoder le JSON du PCB de contrôle
void temperature_refresh (float_t temperature)
{
  if (temperatureFloat < LOW_TEMPERATURE)
  {
    lv_obj_set_style_text_color(objects.lb_temperature, lv_color_hex(color_low_temp), LV_PART_MAIN); // change la couleur
    lv_obj_set_style_border_color(objects.pnl_temperature, lv_color_hex(color_low_temp), LV_PART_MAIN);
    lv_obj_set_style_text_color(objects.lb_symbole_temperature, lv_color_hex(color_low_temp), LV_PART_MAIN);
  }
  else if (temperatureFloat < HIGH_TEMPERATURE && temperatureFloat>LOW_TEMPERATURE)
  {
    lv_obj_set_style_text_color(objects.lb_temperature, lv_color_hex(color_med_temp), LV_PART_MAIN); // change la couleur
    lv_obj_set_style_border_color(objects.pnl_temperature, lv_color_hex(color_med_temp), LV_PART_MAIN);
    lv_obj_set_style_text_color(objects.lb_symbole_temperature, lv_color_hex(color_med_temp), LV_PART_MAIN);
  }
  else if (temperatureFloat > HIGH_TEMPERATURE)
  {
    lv_obj_set_style_text_color(objects.lb_temperature, lv_color_hex(color_high_temp), LV_PART_MAIN); // change la couleur
    lv_obj_set_style_border_color(objects.pnl_temperature, lv_color_hex(color_high_temp), LV_PART_MAIN);
    lv_obj_set_style_text_color(objects.lb_symbole_temperature, lv_color_hex(color_high_temp), LV_PART_MAIN);
  }
}

//Est l'action permettant à la page de configuration d'apparaitre sur l'écran lorsque le bouton de configuration est pesé
void event_handler_imgbtn_config(lv_event_t * e)
{
    lv_scr_load(objects.configuration);  // Affiche l'écran de configuration
}

//Est l'action permettant à la page de principale d'apparaitre sur l'écran lorsque le bouton retour est pesé
void event_handler_btn_retour_main(lv_event_t * e) 
{
    lv_scr_load(objects.main);  // Affiche l'écran de configuration
}

//Permet la gestion de la lecture en x et y de l'écran
void my_touchpad_read( lv_indev_t * indev, lv_indev_data_t * data )
  {
      //For example  ("my_..." functions needs to be implemented by you)
      uint16_t x, y;
      bool touched = tft.getTouch(&x, &y);
  
      if(!touched) {
          data->state = LV_INDEV_STATE_RELEASED;
      } else {
          data->state = LV_INDEV_STATE_PRESSED;
  
          data->point.x = x;
          data->point.y = y;
      }
}

//////////////////////à changer pour changer le fonctionnement des bouton dans configuration////////////////////////////////////////////////
//Permet de changer savoir
void event_handler_btn_vitesse(lv_event_t * e) {
    // Ce code sera exécuté lorsque le bouton est appuyé
    //Serial.println( "Bouton 'btn_vitesse' appuyé !"); //Au besoin pour un debug
}



void gestion_btn_conf()
{
  if(lv_obj_has_state(objects.btn_vitesse, LV_STATE_CHECKED))
    {
       strcpy(vitesse_texte, vitesse_mph);
    }
  else
    {
      strcpy(vitesse_texte, vitesse_kmh); 
    } 
    //Serial.println(vitesse_texte);

  if (lv_obj_has_state(objects.btn_temperature, LV_STATE_CHECKED)) 
  {
    strcpy(symbole_temperature, symbole_Fahrenheit);
  } 
  else 
  {
    strcpy(symbole_temperature, symbole_celsius);
  }
}
//////////////////////////////////////////////////////////////////////////////////////////



void setup()
{
    Serial.begin(115200);

    //Permet d'allumer l'écran
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN,LOW);
    
    //Touch
    tft.setTouch(calData);  //Calibre l'écran avec les valeurs de calibration
       // Initialisation du périphérique tactile
 
    lv_init();
    
    /*Set a tick source so that LVGL will know how much time elapsed. */
    lv_tick_set_cb(my_tick);

    /* register print function for debugging */
    #if LV_USE_LOG != 0
        lv_log_register_print_cb( my_print );
    #endif

    lv_display_t * disp;

  #if LV_USE_TFT_ESPI
    /*TFT_eSPI can be enabled lv_conf.h to initialize the display in a simple way*/
    disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf, sizeof(draw_buf));
    lv_display_set_rotation(disp, TFT_ROTATION);
  #else
    /*Else create a display yourself*/
    disp = lv_display_create(TFT_HOR_RES, TFT_VER_RES);
    lv_display_set_flush_cb(disp, my_disp_flush);
    lv_display_set_buffers(disp, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
  #endif
  
  Serial.println( "Setup done" );

  lv_indev_t * indev = lv_indev_create();           /*Create an input device*/
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);  /*Touch pad is a pointer-like device*/
  lv_indev_set_read_cb(indev, my_touchpad_read);    /*Set your driver function*/

  ui_init(); //Création des widgets

  //Event des bouton
    lv_obj_add_event_cb(objects.imgbtn_config, event_handler_imgbtn_config, LV_EVENT_PRESSED, NULL);   //Assign an event callback
    lv_obj_add_event_cb(objects.btn_retour_main, event_handler_btn_retour_main, LV_EVENT_PRESSED, NULL);   //Assign an event callback
    lv_obj_add_event_cb(objects.btn_vitesse, event_handler_btn_vitesse, LV_EVENT_PRESSED, NULL);   //Assign an event callback


  ////////////// Paramétrage des widgets //////////
    lv_arc_set_range(objects.arc_speed, arc_min, arc_max); // setup max et minimum de l'arc
    lv_scale_set_range(objects.scale_speed, arc_min, arc_max); // setup du scale visuel
    lv_bar_set_range(objects.bar_tension, bar_tension_min, bar_tension_max); // setup min et max de la bare de tension
    lv_bar_set_range(objects.bar_puissance, bar_puissance_min, bar_puissance_max); // setup min et max de la bare de tension
}



void loop()
{
  uint16_t t_x = 0, t_y = 0; //Variables pour storer les coordonnées du "touch"
  bool pressed = tft.getTouch(&t_x, &t_y);  //Vérifie si l'écran a été touché

  if (pressed == true)  //Si l'utilisateur touche l'écran
  { 
    //tft.setCursor(15,150);
    //tft.print("SCREEN TOUCHED");  //Affiche un message à l'écran
    
    Serial.print("x : ");
    Serial.print(t_x);
    Serial.print(", y : ");
    Serial.println(t_y);
  }
  
  gestion_btn_conf();

  //À enlever lors de l'utilisation avec PCB et JSON décortiqué
    generate_random();

  //Température
    temperature_refresh(temperatureFloat);

    //Serial.println(texte_temperature);

    lv_label_set_text_fmt(objects.lb_symbole_temperature,symbole_temperature);

    dtostrf(temperatureFloat, 4, 0, temperatureChar);
    lv_label_set_text_fmt(objects.lb_temperature,temperatureChar);

  //Vitesse   
    dtostrf(vitesseFloat, 4, 0, vitesseChar);
    lv_label_set_text_fmt(objects.lb_speed,vitesseChar);
    lv_arc_set_value(objects.arc_speed, vitesseFloat);
    lv_label_set_text_fmt(objects.lb_unite_vitesse,vitesse_texte); /////////////////////////////////possibilité de ne pas le faire à chaque fois

  //Tension
    dtostrf(tensionFloat, 4, 0, tensionChar);
    lv_label_set_text_fmt(objects.lb_tension,tensionChar);
    lv_bar_set_value(objects.bar_tension, (int)tensionFloat, LV_ANIM_ON);  //// Change immédiatement à 50 (sans animation)
    //lv_bar_set_value(bar, 75, LV_ANIM_ON);   // Change progressivement à 75 (avec animation)

  //Puissance
    dtostrf(puissanceFloat, 4, 0, puissanceChar);
    lv_label_set_text_fmt(objects.lb_puissance,puissanceChar);
    lv_bar_set_value(objects.bar_puissance, (int)puissanceFloat, LV_ANIM_OFF);  //// Change immédiatement à 50 (sans animation)

    lv_timer_handler(); /* let the GUI do its work */
    ui_tick();
    delay(15); /* let this time pass */
}