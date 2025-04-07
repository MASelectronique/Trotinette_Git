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
#include <SoftwareSerial.h>
#include <ArduinoJson.h>  //Lib pour JSON
#include <Wire.h>
#include <ICM42670P.h>    //Lib pour accéléromètre
#include <math.h>

#define RX_PIN 18
#define TX_PIN 17

#define SDA_PIN 8
#define SCL_PIN 9
#define INT_PIN 5

#define LED_PIN 2

JsonDocument doc;   //Objet JSON reçu
String jsonString;  //Objet JSON décodé
String jsonErrorMessage;  //Json a affiché si erreur de décodage
DeserializationError jsonError;  //Variable qui retourne le résultat du décodage du JSON
EspSoftwareSerial::UART jsonSerial; //Interface UART pour recevoir JSON

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

void printData(const Data &t);  //Fonction pour afficher les données de la trottinette

Data trottinette; //L'objet trottinette contenant tous les données de la trottinette

ICM42670 IMU(Wire,0); //Crée l'objet de l'accelerateur en I2C
float x,y,z = 0.0;
int pitch = 0;
int pitch_offset = -41;

void setup() {
  int ret;
  Serial.begin(115200); //Port pour moniteur série
  jsonSerial.begin(9600, EspSoftwareSerial::SWSERIAL_8N1, RX_PIN, TX_PIN);  //Port pour recevoir objet JSON
  while(!Serial) {} //Attend que le moniteur série soit initialisé

  pinMode(INT_PIN, INPUT_PULLUP);
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
  inv_imu_sensor_event_t imu_event;

  // Get last event
  IMU.getDataFromRegisters(imu_event);
  
  x = imu_event.accel[0];
  y = imu_event.accel[1];
  z = imu_event.accel[2];

  // Calculate pitch and roll
  pitch = atan2(y, sqrt(x * x + z * z)) * 180.0 / PI;  // Convert radians to degrees

  pitch -= pitch_offset;

  // Print results
  Serial.print("Pitch: ");
  Serial.println(pitch);

  if (pitch >= 30 || pitch <= -30) {
    digitalWrite(LED_PIN, HIGH);
  }
  else {
    digitalWrite(LED_PIN, LOW);
  }

  // Run @ ODR 100Hz
  delay(1000);

  /*jsonString = jsonSerial.readStringUntil('\n');  //Lit le message recu jusqu'a \n
  jsonErrorMessage = jsonString;
  jsonError = deserializeJson(doc, jsonString);  //Décode l'objet

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
    printData(trottinette); //Affiche les données de la trottinette
  }
  else {  //Si erreur de décodage
    Serial.println("json error:");
    Serial.println(jsonErrorMessage);
  }*/

}

//Nom: printData
//Param: const Data &t, la struct de type Data
//Return: aucun
//Brief: Affiche les valeurs contenues dans la struct de type Data
void printData(const Data &t) {
  Serial.println("==== Trottinette Data ====\n");
  Serial.print("Model Num: "); Serial.println(t.model_num);

  Serial.print("-- Ctrl --\n");
  Serial.print("Etat: "); Serial.println(t.ctrl.etat);
  Serial.print("Break Active: "); Serial.println(t.ctrl.break_active);
  Serial.print("Fault Code: "); Serial.println(t.ctrl.fault_code);
  Serial.print("Cmd: "); Serial.println(t.ctrl.cmd);

  Serial.print("-- Drive --\n");
  Serial.print("Temperature: "); Serial.println(t.drive.temperature);
  Serial.print("Tension Total: "); Serial.println(t.drive.tension_total);
  Serial.print("Courant Total: "); Serial.println(t.drive.courant_total);
  Serial.print("Energie: "); Serial.println(t.drive.energie);
  Serial.print("Vitesse: "); Serial.println(t.drive.vitesse);
  Serial.print("==========================\n\n");
}