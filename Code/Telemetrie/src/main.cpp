/*
  @file   main.cpp, projet: Module de télémétrie
  @author Lucas Lalumière Longpré
  @date   25 Février 2025
  @brief  

  Environnement:
      Développement: Visual Studio Code
  Matériel: -MCU = ESP32-S3 WROOM-1U
            -Accéléromètre = ICM-42670-P
*/
#include <Arduino.h>
#include <ArduinoJson.h>  //Lib pour JSON
#include <ICM42670P.h>    //Lib pour accéléromètre

#define RX_PIN D7
#define TX_PIN D6

JsonDocument doc;   //Objet JSON reçu
String jsonString;  //Objet JSON décodé
DeserializationError jsonError;  //Variable qui retourne le résultat du décodage du JSON

struct Ctrl {
  int etat;
  int break_active;
  int fault_code;
  int cmd;
};

struct Drive {
  float temperature;
  float tension_total;
  float courant_total;
  float energie;
  float vitesse;
};

struct Data {
  int model_num;
  Ctrl ctrl;
  Drive drive;
};

void setup() {
  Serial.begin(115200); //Moniteur série à 115200 bauds
  Serial1.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);  //Interface UART1 à 115200 bauds
}

void loop() {
  if (Serial1.available()) {
    jsonString = Serial1.readString();  //Lit objet reçu par UART
    Serial.println(jsonString); //Affiche objet reçu au moniteur série
    jsonError = deserializeJson(doc, jsonString);  //Décode l'objet
  }

  if (!jsonError) { //Si le JSON est décodé avec succès

  }
}
