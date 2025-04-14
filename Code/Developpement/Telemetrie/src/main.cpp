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
#include <SoftwareSerial.h>           //Lib pour UART software
#include <Wire.h>                     //Lib I2C
#include "trottinetteData.h"          //Lib pour données trottinette
#include "trottinetteAccelerometer.h" //Lib pour accéléromètre

//Pattes pour communication UART
#define RX_PIN 18
#define TX_PIN 17

//Pattes pour l'accelerometre
#define SDA_PIN 8
#define SCL_PIN 9

//Pattes pour ecran
#define LED_PIN 2 //Patte qui controle la backlight de l'ecran

EspSoftwareSerial::UART jsonSerial; //Interface UART pour recevoir JSON

void setup() {
  Serial.begin(115200); //Port pour moniteur série
  jsonSerial.begin(115200, EspSoftwareSerial::SWSERIAL_8N1, RX_PIN, TX_PIN);  //Port pour recevoir objet JSON
  while(!Serial) {} //Attend que le moniteur série soit initialisé

  pinMode(LED_PIN, OUTPUT);

  Wire.begin(SDA_PIN, SCL_PIN); //Initialise l'interface I2C
  initAccel();  //Initialise l'accelerometre
}

void loop() {
  //Calcule et retourne le pitch
  pitch = getPitch();

  //Affiche le pitch au moniteur série
  Serial.print("Pitch: ");
  Serial.println(pitch);

  if (pitch >= 30 || pitch <= -30) {  //Si la trottinette est penchée sur le côté
    digitalWrite(LED_PIN, HIGH);      //Éteind l'écran
  }
  else {
    digitalWrite(LED_PIN, LOW);       //Sinon, allume l'écran
  }

  jsonString = jsonSerial.readStringUntil('\n');  //Lit le message recu jusqu'a \n
  jsonErrorMessage = jsonString;
  jsonError = deserializeJson(doc, jsonString);   //Décode l'objet

  if (!jsonError) {         //Si le JSON est décodé avec succès
    getData(doc);           //Enregistre les valeurs contenues dans l'objet json décodé
    printData(trottinette); //Affiche les données de la trottinette
  }
  else {  //Si erreur de décodage
    Serial.println("JSON ERROR:");
    Serial.println(jsonErrorMessage);
  }
}