/*
	Fait par Marc-Antoine Sauvé et Lucas Lalumière Longpré
	Date 23 avril 2025

	Brief : Ce code est un template permettant d'avoir la base pour décodé un JSON, initialiser l'écran ILI9341, initialiser les fichiers 
	ui générer par EEZ Studio ainsi que le décodage des données envoyé par l'accéléromètre.
*/

#include <lvgl.h>				 // Pour la libraries LVGL
#include <string.h>              // pour l'affichage graphique de text
#include <TFT_eSPI.h>            //Librairie pour l'écran tactile
#include <ui.h>                  // Fichier généré par EEZ studio
#include <SoftwareSerial.h>		 //Étant donné que nous utilisons d'autre patte que celles fait pour la communication UART, on doit utiliser un autre moniteur série
#include <Wire.h>                //Lib I2C

#define BYTE_PER_PIXEL (LV_COLOR_FORMAT_GET_SIZE(LV_COLOR_FORMAT_RGB565))

#include "trottinetteAccelerometer.h"
#include "trottinetteData.h"

//Set to your screen resolution and rotation
#define TFT_HOR_RES   240
#define TFT_VER_RES   320
#define TFT_ROTATION  LV_DISPLAY_ROTATION_0 //Permet de choisir la rotation de l'écran

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

//LVGL écrit dans ce buffer pour créer l'écran 1/10 de la grosseur de l'écran fonctionne généralement bien
#define DRAW_BUF_SIZE (TFT_HOR_RES * TFT_VER_RES / 10 * (LV_COLOR_DEPTH / 8))

uint32_t draw_buf[DRAW_BUF_SIZE / 4];


//**Action requise - Calibration de l`écran**//
/*
1) Télécharger le programme touch_calibration.ino de la section code --> Developpement --> Touch_calibration du github
2) Faite le branchement du module de télémétrie ainsi que du module d'affichage. Vous référer à la section 6.5 du manuel technique.
3) Programmer l'ESP32 du module de télémétrie avec le code touch_calibration.ino. Ce programme permet davoir les données nécessaire pour la calibration de l'écran dans votre programme.
4) Des flèches vous indiquera où appuyer pour que le programme puisse vous générer les données de calibration.
5) Changer les #### de la ligne ci-dessous par les valeurs générées.
*/

//Touch screen
uint16_t calData[5] = { ####, ####, ####, ####, #### };  //Valeurs de calibration de l'écran

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
/*Le tick dans LVGL sert à mesurer le temps écoulé pour gérer les animations, les délais et la gestion des événements basés sur le temps.
Il représente un compteur de millisecondes qui permet à la bibliothèque de savoir combien de temps s'est écoulé depuis le démarrage du système. 
*/
static uint32_t my_tick(void)
{
    return millis();
}


/* 
Pour plus d'informations, aller voir dans la section setup() après la ligne : ui_init();

//Exemple d'évènement appeler lorsque un bouton est appuyé
void event_handler_imgbtn_config(lv_event_t * e)
{
   //Faite les action que vous désiré lorsque le bouton est appuyé
   
}
*/

//Permet la gestion de la lecture en x et y de l'écran
void my_touchpad_read( lv_indev_t * indev, lv_indev_data_t * data )
  {
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

void setup()
{
  //Communication Serie
    Serial.begin(115200);

  //JSON
    jsonSerial.begin(115200, EspSoftwareSerial::SWSERIAL_8N1, RX_PIN, TX_PIN);  //Port pour recevoir objet JSON
    while(!Serial) {} //Attend que le moniteur série soit initialisé

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

/*
Si vous voulez créer des évènements comme une action qui se passe lorsqu'on appuie sur un bouton, ça doit être ici (après le ui_init).

Exemple : lv_obj_add_event_cb(objects.imgbtn_config, event_handler_imgbtn_config, LV_EVENT_PRESSED, NULL);
Création de l'évènement (nom de votre object générer dans EEZ Studio,
						Fonction à appeler lorsque l’événement spécifié se produit,
						Type d’événement LVGL écouté,données utilisateur que tu peux récupérer plus tard dans ton callback via lv_event_get_user_data(e) (optionnel) )

*/

/* 
Si vous voulez changer des paramètres des widgets que vous avez créer dans EEZ-Studio, ça doit être ici (après le ui_init).
// Paramétrage des widgets à mettre à jour avec notre code
lv_arc_set_range(nom de l'object, valeur minimum, valeur maximum); // setup max et minimum de l'arc
*/

Serial.println( "Setup done" ); // Ligne de test
}

void loop()
{
  //Calcule et retourne le pitch
  pitch = getPitch();

  if (pitch >= 30 || pitch <= -30) {  //Si la trottinette est penchée sur le côté
    digitalWrite(LED_PIN, HIGH);      //Éteind l'écran
  }
  else {
    digitalWrite(LED_PIN, LOW);       //Sinon, allume l'écran
  }

  jsonString = jsonSerial.readStringUntil('\n');  //Lit le message recu jusqu'a \n
  jsonErrorMessage = jsonString;
  jsonError = deserializeJson(doc, jsonString);   //Décode l'objet

  if (!jsonError) //Si le JSON est décodé avec succès
  {         
    getData(doc);           //Enregistre les valeurs contenues dans l'objet json décodé
    printData(trottinette); //Affiche les données de la trottinette
  }
  else {  //Si erreur de décodage
    Serial.println("JSON ERROR:");
    Serial.println(jsonErrorMessage);
  }

/////////////////////Mise à jour de l'écran///////////////////////////////
/*Exemple de modification d'un label générer par EEZ Studio
  dtostrf(tempreratureInt, 4, 0, temperatureChar);//Prend la valeur du JSON et la transforme en CHAR pour LVGL *Attention, tempreratureInt redevient à 0 après cette ligne**
  lv_label_set_text_fmt(objects.lb_temperature,temperatureChar); // Change le texte du label
*/

//Gestion par lvgl et du UI
  lv_timer_handler(); // let the GUI do its work 
  ui_tick();
  delay(15); // let this time pass 
}