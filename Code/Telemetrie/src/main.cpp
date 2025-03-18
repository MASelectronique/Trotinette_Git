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

#define RX_PIN 36
#define TX_PIN 37

JsonDocument doc;   //Objet JSON reçu
String jsonString;  //Objet JSON décodé
DeserializationError jsonError;  //Variable qui retourne le résultat du décodage du JSON

struct Ctrl { //Struct qui contient les données du contrôleur
  int etat;
  int break_active;
  int fault_code;
  int cmd;
};

struct Drive {  //Struct qui contient les données de la drive moteur
  float temperature;
  float tension_total;
  float courant_total;
  float energie;
  String vitesse; //string pour l'instant mais sera changé pour float
};

struct Data { //Struct qui contient les données de la trottinette
  int model_num;
  Ctrl ctrl;
  Drive drive;
};

void printData(const Data &t);

Data trottinette;

void setup() {
  Serial.begin(115200); //Moniteur série à 115200 bauds
  Serial1.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);  //Interface UART1 à 115200 bauds
}

void loop() {
  if (Serial1.available()) {
    jsonString = Serial1.readString();  //Lit objet reçu par UART
    jsonError = deserializeJson(doc, jsonString);  //Décode l'objet
  }

  if (!jsonError) { //Si le JSON est décodé avec succès
    trottinette.model_num = doc["model_num"];                     //Numéro de la trottinette

    trottinette.ctrl.etat = doc["ctrl"]["etat"];                  //État de la trottinette
    trottinette.ctrl.break_active = doc["ctrl"]["break_active"];  //État du frein
    trottinette.ctrl.fault_code = doc["ctrl"]["fault_code"];      //Code d'erreur
    trottinette.ctrl.cmd = doc["ctrl"]["cmd"];                    //Pourcentage du throttle

    trottinette.drive.temperature = doc["drive"]["temperature"];      //Température du moteur
    trottinette.drive.tension_total = doc["drive"]["tension_total"];  //Tension totale au moteur
    trottinette.drive.courant_total = doc["drive"]["courant_total"];  //Courant total au moteur
    trottinette.drive.energie = doc["drive"]["energie"];              //Puissance consommée par le moteur
    trottinette.drive.vitesse = doc["drive"]["vitesse"].as<String>(); //Vitesse de la trottinette
  }

  printData(trottinette); //Affiche les données de la trottinette
  delay(500); //Délai de 500ms
}

//Nom: printData
//Param: const Data &t, la struct de type Data
//Return: aucun
//Brief: Affiche les valeurs contenues dans la struct de type Data
void printData(const Data &t) {
  Serial.println("==== Trottinette Data ====");
  Serial.print("Model Num: "); Serial.println(t.model_num);

  Serial.println("-- Ctrl --");
  Serial.print("Etat: "); Serial.println(t.ctrl.etat);
  Serial.print("Break Active: "); Serial.println(t.ctrl.break_active);
  Serial.print("Fault Code: "); Serial.println(t.ctrl.fault_code);
  Serial.print("Cmd: "); Serial.println(t.ctrl.cmd);

  Serial.println("-- Drive --");
  Serial.print("Temperature: "); Serial.println(t.drive.temperature);
  Serial.print("Tension Total: "); Serial.println(t.drive.tension_total);
  Serial.print("Courant Total: "); Serial.println(t.drive.courant_total);
  Serial.print("Energie: "); Serial.println(t.drive.energie);
  Serial.print("Vitesse: "); Serial.println(t.drive.vitesse);
  Serial.println("==========================\n\n");
}
