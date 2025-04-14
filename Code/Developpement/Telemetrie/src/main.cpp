/*
  @file   main.cpp, projet: Module de télémétrie, Trottinette
  @author Lucas Lalumière Longpré
  @date   25 Février 2025
  @brief  Code pour le PCB de télémétrie, toutes les fonctionnalités
          du module sont programmées dans ce code. Comm par UART pour
          recevoir un objet JSON, fonctions pour décoder et afficher données
          trottinette et code pour calculer le pitch de l'accéléromètre.

  Environnement:
      Développement: Visual Studio Code
  Matériel: -MCU = ESP32-S3 WROOM-1U
            -Accéléromètre = ICM-42670-P
*/
#include <Arduino.h>
#include <SoftwareSerial.h> //Lib pour UART software
#include <Wire.h>           //Lib I2C
#include <ICM42670P.h>      //Lib pour accéléromètre
#include <math.h>         
#include "trottinetteData.h"       //Lib pour données trottinette

//Pattes pour communication UART
#define RX_PIN 18
#define TX_PIN 17

//Pattes pour l'accelerometre
#define SDA_PIN 8
#define SCL_PIN 9 
#define INT_PIN 5 //Interrupt

//Pattes pour ecran
#define LED_PIN 2 //Patte qui controle la backlight de l'ecran

EspSoftwareSerial::UART jsonSerial; //Interface UART pour recevoir JSON

int getPitch(); //Calcul du pitch selon valeurs 

ICM42670 IMU(Wire,0); //Crée l'objet de l'accelerateur en I2C

//Variables de l'accéléromètre
int ret;                //Variable pour initialiser l'accéléromètre
float x,y,z = 0.0;      //Valeurs X,Y,Z
int pitch = 0;          //Valeur de pitch
int pitch_offset = -41; //Offset pour le pitch

void setup() {
  Serial.begin(115200); //Port pour moniteur série
  jsonSerial.begin(115200, EspSoftwareSerial::SWSERIAL_8N1, RX_PIN, TX_PIN);  //Port pour recevoir objet JSON
  while(!Serial) {} //Attend que le moniteur série soit initialisé

  //pinMode(INT_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);

  Wire.begin(SDA_PIN, SCL_PIN); //Initialise l'interface I2C

  //Initialise l'accelerateur ICM42670P
  ret = IMU.begin();
  if (ret != 0) {
    Serial.print("ICM42670 initialization failed: ");
    Serial.println(ret);
    while(1); //Si erreur d'init, on bloque le programme
  }

  // Accel ODR = 100 Hz and Full Scale Range = 16G
  IMU.startAccel(100,16);
}

void loop() {
  //Calcule et retourne le pitch
  pitch = getPitch();

  // Print results
  Serial.print("Pitch: ");
  Serial.println(pitch);

  if (pitch >= 30 || pitch <= -30) {
    digitalWrite(LED_PIN, HIGH);
  }
  else {
    digitalWrite(LED_PIN, LOW);
  }

  jsonString = jsonSerial.readStringUntil('\n');  //Lit le message recu jusqu'a \n
  jsonErrorMessage = jsonString;
  jsonError = deserializeJson(doc, jsonString);  //Décode l'objet

  if (!jsonError) { //Si le JSON est décodé avec succès
    getData(doc);           //Enregistre les valeurs contenues dans l'objet json décodé
    printData(trottinette); //Affiche les données de la trottinette
  }
  else {  //Si erreur de décodage
    Serial.println("JSON ERROR:");
    Serial.println(jsonErrorMessage);
  }
}

//Nom:    getPitch
//Param:  Aucun
//Return: int pitch, valeur du pitch calculée
//Brief:  Calcule le pitch selon les valeurs x,y,z
int getPitch() {
  inv_imu_sensor_event_t imu_event;

  //Retourne les valeurs x,y,z
  IMU.getDataFromRegisters(imu_event);
  
  //Enregistre les valeurs x,y,z
  x = imu_event.accel[0];
  y = imu_event.accel[1];
  z = imu_event.accel[2];

  //Calcul du pitch
  pitch = atan2(y, sqrt(x * x + z * z)) * 180.0 / PI;  //Convertie x,y,z en pitch, puis passe de radians à degrés
  pitch -= pitch_offset;  //Soustraction de l'offset

  return pitch;
}