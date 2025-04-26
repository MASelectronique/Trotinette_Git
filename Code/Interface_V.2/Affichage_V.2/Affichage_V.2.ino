/*
À faire :   
          -Changer le fonctionnement des bouton dans configuration
          -Activer la fnnction temperature 
          -Faire changement de vitesse + changement du cadran

    Fait par : Marc-Antoine Sauvé et Lucas Lalumière Longpré

    Brief : Ce code permet de gérer l’interface graphique et la collecte de données d'une trottinette connectée avec un écran tactile. Il utilise la bibliothèque LVGL
    pour créer l’interface utilisateur, TFT_eSPI pour gérer l’écran, et des capteurs via I2C (accéléromètre). Il affiche la vitesse, la température, la tension de batterie
    et la commande d’accélération, en rafraîchissant dynamiquement ces données. L’écran peut afficher différentes pages (ex. : principale ou configuration) en fonction 
    de boutons tactiles. Le code lit aussi les données JSON via une liaison série pour mettre à jour l’affichage. Enfin, des modes débogage optionnels permettent de 
    suivre les touchers ou le pitch de l’appareil via le moniteur série.

    Date : 23 avril 2024
*/

#include <lvgl.h>             //libraries graphique
#include <TFT_eSPI.h>         //Librairie pour l'écran tactile
#include <ui.h>               // Fichier généré par EEZ studio
#include <SoftwareSerial.h>   //Permet l'utilisation de patte UART non programmé à 
#include <Wire.h>                //Lib I2C

#include "trottinetteAccelerometer.h"
#include "trottinetteData.h"

#define BYTE_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565))

//---------------------------------Section DEBUG ---------------------------------//
#define modeDebug_JSON 0 //Print dans le moniteur série le JSON ou l'ereur si le JSON n'a pas été bien reçu
#define modeDebug_touch 0 // Print dans le moniteur série la valeur d'où on appuie sur l'écran
#define modeDebug_pitch 0 //Print dans le moniteur série 
#define modeDebug_temperature 0 //Print la température en degrés Celcius et Fahrenheit dans le moniteur série
#define mondeDubug_vitesse 0 // Print les valeur en kmh et en mp sur le moniteur série

#define avec_PCB_controleur 0 // 0 : permet de générer des données aléatoires pour l'affichage sans être connecter au PCB contrôleur
                               // 1 : permet de décoder le JSON envoyé par le PCB contrôleur
                              
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

//Pattes pour ecran
#define LED_PIN 2 //Patte qui controle la backlight de l'ecran

//Pattes pour communication UART
#define RX_PIN 18
#define TX_PIN 17

//Pattes pour l'accelerometre
#define SDA_PIN 8
#define SCL_PIN 9

//JSON
EspSoftwareSerial::UART jsonSerial; //Interface UART pour recevoir JSON

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
String vitesseString="";
int vitesseInt=0;
int vitesseInt_tamp=0;
char vitesseChar[4]="";
char vitesse_unite [5]="";

//Température
int temperatureInt = 0;
int temperatureInt_tamp =0;
char temperatureChar[4]="";
char symbole_temperature[4] = "°C";

//Charge de la Batterie
int tensionInt = 90;
char tensionChar[3]="";

//cmd (throttle)
int cmd = 0;
char cmdChar[3]="";

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

// Utilise millis() d'Arduino comme source de tic 
static uint32_t my_tick(void)
{
    return millis();
}


// Si le PCB contrôleur n'envoie pas le JSON me le #define avec_PCB_controleur à 0
void generate_data(void)
{
  //Température  
  if (temperatureInt>=100)
  {
  temperatureInt = -20;
  }
  temperatureInt = temperatureInt +5;
  
  //Vitesse 
  if (vitesseInt>=50)
  {
  vitesseInt = 0;
  }
  vitesseInt= vitesseInt+4;

  //CMD 
  if (cmd>=100)
  {
    cmd = 0;
  }
  cmd = cmd +5;

  //Batterie
  if(tensionInt<=0)
  {
    tensionInt=100;
  }
  tensionInt= tensionInt-5;

/*
  //Print les données générées
  Serial.println("------Random data generated--------");
  Serial.print("Temperature : ");
  Serial.println(temperatureInt);

  Serial.print("Vitesse : ");
  Serial.println(vitesseInt);

  Serial.print("Tension : ");
  Serial.println(tensionInt);

  Serial.print("Cmd : ");
  Serial.println(cmd);

  Serial.println("-------------------------------");
  */
}


//Prend les données décodées du JSON envoyé par le PCB contrôleur
void refresh_data_affichage(void)
{
//Température
  temperatureInt=trottinette.drive.temperature;

//cmd (throttle)
  cmd=trottinette.ctrl.cmd/(2.55);
    
//Tension de la batterie
  tensionInt=trottinette.drive.tension_total;

//Vitesse
  vitesseString=trottinette.drive.vitesse;
}

//Permet le rapfraichissement des valeurs et couleur de l'affichage de la température sans avoir à décoder le JSON du PCB de contrôle
void temperature_refresh (int temperature)
{
  if (temperatureInt < LOW_TEMPERATURE)
  {
    lv_obj_set_style_text_color(objects.lb_temperature, lv_color_hex(color_low_temp), LV_PART_MAIN); // change la couleur
    lv_obj_set_style_border_color(objects.pnl_temperature, lv_color_hex(color_low_temp), LV_PART_MAIN);
    lv_obj_set_style_text_color(objects.lb_symbole_temperature, lv_color_hex(color_low_temp), LV_PART_MAIN);
  }
  else if (temperatureInt < HIGH_TEMPERATURE && temperatureInt>LOW_TEMPERATURE)
  {
    lv_obj_set_style_text_color(objects.lb_temperature, lv_color_hex(color_med_temp), LV_PART_MAIN); // change la couleur
    lv_obj_set_style_border_color(objects.pnl_temperature, lv_color_hex(color_med_temp), LV_PART_MAIN);
    lv_obj_set_style_text_color(objects.lb_symbole_temperature, lv_color_hex(color_med_temp), LV_PART_MAIN);
  }
  else if (temperatureInt > HIGH_TEMPERATURE)
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
       strcpy(vitesse_unite, vitesse_mph);
    }
  else
    {
      strcpy(vitesse_unite, vitesse_kmh); 
    } 

  if (lv_obj_has_state(objects.btn_temperature, LV_STATE_CHECKED)) 
  {
    strcpy(symbole_temperature, symbole_Fahrenheit);
  } 
  else 
  {
    strcpy(symbole_temperature, symbole_celsius);
  }
}

void setup()
{
  if(modeDebug_touch||modeDebug_pitch || modeDebug_temperature|| mondeDubug_vitesse)
  {
    //Communication Serie
    Serial.begin(115200);
    Serial.println( "Communication serie active" );
  }
  
  jsonSerial.begin(115200, EspSoftwareSerial::SWSERIAL_8N1, RX_PIN, TX_PIN);  //Port pour recevoir objet JSON

  if(modeDebug_JSON)
  {
  //JSON
    while(!Serial) {} //Attend que le moniteur série soit initialisé
  }
  //I2C
    Wire.begin(SDA_PIN, SCL_PIN); //Initialise l'interface I2C
    initAccel();  //Initialise l'accelerometre

  //Permet d'allumer l'écran en tout temps
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN,LOW);
    
  //Touch
    tft.setTouch(calData);  //Calibre l'écran avec les valeurs de calibration

  //LVGL
    lv_init();
    lv_tick_set_cb(my_tick);// Permet d'animer des objets, gérer les délais, animations, gestions tactiles. (Tout ce qui dépend du temps).
    lv_display_t * disp; //Créé un objet display pour que LVGL puisse avoir un espace de travail (sandbox)

    // register print function for debugging 
    #if LV_USE_LOG != 0
        lv_log_register_print_cb( my_print );
    #endif

    
  //TFT_eSPI
    #if LV_USE_TFT_ESPI
      //TFT_eSPI can be enabled lv_conf.h to initialize the display in a simple way
      disp = lv_tft_espi_create(TFT_HOR_RES, TFT_VER_RES, draw_buf, sizeof(draw_buf));
      lv_display_set_rotation(disp, TFT_ROTATION);
    #else
      //Else create a display yourself
      disp = lv_display_create(TFT_HOR_RES, TFT_VER_RES);
      lv_display_set_flush_cb(disp, my_disp_flush);
      lv_display_set_buffers(disp, draw_buf, NULL, sizeof(draw_buf), LV_DISPLAY_RENDER_MODE_PARTIAL);
    #endif
   
  lv_indev_t * indev = lv_indev_create();           //Create an input device
  lv_indev_set_type(indev, LV_INDEV_TYPE_POINTER);  //Touch pad is a pointer-like device
  lv_indev_set_read_cb(indev, my_touchpad_read);    //Set your driver function

  ui_init(); //Création des widgets (gestion du fichier ui)

  //Création des évènement des boutons
    lv_obj_add_event_cb(objects.imgbtn_config, event_handler_imgbtn_config, LV_EVENT_PRESSED, NULL);   //Assign an event callback
    lv_obj_add_event_cb(objects.btn_retour_main, event_handler_btn_retour_main, LV_EVENT_PRESSED, NULL);   //Assign an event callback
    lv_obj_add_event_cb(objects.btn_vitesse, event_handler_btn_vitesse, LV_EVENT_PRESSED, NULL);   //Assign an event callback

  // Paramétrage des widgets à mettre à jour avec notre code
    lv_arc_set_range(objects.arc_speed, arc_min, arc_max_kmh); // setup max et minimum de l'arc
    lv_scale_set_range(objects.scale_speed, arc_min, arc_max_kmh); // setup du scale visuel
    lv_bar_set_range(objects.bar_tension, bar_tension_min, bar_tension_max); // setup min et max de la bare de tension
    lv_bar_set_range(objects.bar_puissance, bar_cmd_min, bar_cmd_max); // setup min et max de la bare de tension

    Serial.println( "Setup done" ); // Ligne de test
}

void loop()
{
  if(modeDebug_touch)
  {
     //Test pour faire afficher la positionnement de touché de l'utilisateur 
    uint16_t t_x = 0, t_y = 0; //Variables pour storer les coordonnées du "touch"
    bool pressed = tft.getTouch(&t_x, &t_y);  //Vérifie si l'écran a été touché
    if (pressed == true)  //Si l'utilisateur touche l'écran
    { 
      Serial.print("x : ");
      Serial.print(t_x);
      Serial.print(", y : ");
      Serial.println(t_y);
    }
  }
  
  pitch = getPitch();//Calcule et retourne le pitch

  if(modeDebug_pitch)
  {
    //Affiche le pitch au moniteur série
      Serial.print("Pitch: ");
      Serial.println(pitch);
  }

  if (pitch >= 30 || pitch <= -30) 
  {  //Si la trottinette est penchée sur le côté
    digitalWrite(LED_PIN, HIGH);      //Éteind l'écran
  }
  else 
  {
    digitalWrite(LED_PIN, LOW);       //Sinon, allume l'écran
  }

  jsonString = jsonSerial.readStringUntil('\n');  //Lit le message recu jusqu'a \n
  jsonErrorMessage = jsonString;
  jsonError = deserializeJson(doc, jsonString);   //Décode l'objet

  if (!jsonError) //Si le JSON est décodé avec succès
  {         
    getData(doc);           //Enregistre les valeurs contenues dans l'objet json décodé
    if(modeDebug_JSON)
    { 
      printData(trottinette); //Affiche les données de la trottinette
    }
  }
  else {  //Si erreur de décodage
    if(modeDebug_JSON)
    { 
      Serial.println("JSON ERROR:");
      Serial.println(jsonErrorMessage);
    }
  }
  gestion_btn_conf();

  if (avec_PCB_controleur)//Avec ou sans PCB contrôleur
  {
    refresh_data_affichage(); //
  }
  else //Si on n'a pas le PCB controleur et qu'on veux générer des données 
  {
    generate_data();
  }
  
/////////////////////Mise à jour de l'écran///////////////////////////////

//Température
  if(modeDebug_temperature) // Permet de voir si la conversion de température est bonne 
  {
    Serial.println("Temperature Celsius : ");
    Serial.println(temperatureInt);
    temperatureInt_tamp = (temperatureInt*9/5)+32; // Formule pour passer de °C à °F
    Serial.println("Temperature Fahrenheit : ");
    Serial.println(temperatureInt_tamp);
  }
  if (strcmp(symbole_temperature, "°F") == 0)// Si la température doir être en °F
  {
    temperatureInt_tamp = (temperatureInt*9/5)+32; // Formule pour passer de °C à °F
    temperature_refresh(temperatureInt_tamp);
    itoa(temperatureInt_tamp, temperatureChar, 10);
  }
  else
  {
    temperature_refresh(temperatureInt);
    itoa(temperatureInt, temperatureChar, 10);
  }

  lv_label_set_text_fmt(objects.lb_symbole_temperature,symbole_temperature);
  lv_label_set_text_fmt(objects.lb_temperature,temperatureChar);

//Cmd (throttle)
  lv_bar_set_value(objects.bar_puissance, cmd, LV_ANIM_OFF); // à laisser avant la ligne dtostrf
  itoa(cmd, cmdChar, 10);
  lv_label_set_text_fmt(objects.lb_puissance,cmdChar);

//Tension de la batterie
  lv_bar_set_value(objects.bar_tension, tensionInt, LV_ANIM_OFF);  //// Change immédiatement à 50 (sans animation)
  itoa(tensionInt, tensionChar, 10);
  lv_label_set_text_fmt(objects.lb_tension,tensionChar);

//Vitesse   
  if(mondeDubug_vitesse)
  {
    Serial.print("Vitesse km/h : ");
    Serial.println(vitesseInt);
    vitesseInt_tamp = vitesseInt / 1.609; // calcul permettant de passer de km/h à mph
    Serial.print("Vitesse mph : ");
    Serial.println(vitesseInt_tamp);
  }
  if(avec_PCB_controleur)
  {
    vitesseInt =vitesseString.toInt();
  }
  if(strcmp(vitesse_unite, "mph") == 0)
  {
    vitesseInt_tamp = vitesseInt / 1.609; // calcul permettant de passer de km/h à mph
    itoa(vitesseInt_tamp, vitesseChar, 10);
    lv_arc_set_value(objects.arc_speed,vitesseInt_tamp);
    lv_arc_set_range(objects.arc_speed, arc_min, arc_max_mph); // setup max et minimum de l'arc
    lv_scale_set_range(objects.scale_speed, arc_min, arc_max_mph); // setup du scale visuel
  }
  else
  {
    itoa(vitesseInt, vitesseChar, 10);
    lv_arc_set_value(objects.arc_speed,vitesseInt);
    lv_arc_set_range(objects.arc_speed, arc_min, arc_max_kmh); // setup max et minimum de l'arc
    lv_scale_set_range(objects.scale_speed, arc_min, arc_max_kmh); // setup du scale visuel
  }
  lv_label_set_text_fmt(objects.lb_speed,vitesseChar);
  lv_label_set_text_fmt(objects.lb_unite_vitesse,vitesse_unite); /////////////////////////////////possibilité de ne pas le faire à chaque fois

//Gestion par lvgl et du UI
  lv_timer_handler(); // let the GUI do its work 
  ui_tick();
  delay(10); // let this time pass 
}