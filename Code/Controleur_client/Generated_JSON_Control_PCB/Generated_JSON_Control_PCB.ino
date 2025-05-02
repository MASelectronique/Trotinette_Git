/**
* @file   main.cpp
* @author Samuel Faucher
* @date   Decembre 2024
* @brief  Simulateur de messages informatif, extran du projet Escooter
*/
 
#include <Arduino.h>
#include <ArduinoJson.h>
 
#define RX_PIN 50
#define TX_PIN 21

#define FALL_DETECTION_PIN 18

#define BAUD 115200
 
#define MODEL_NUMBER 0006
 
// None blocking timing (msec)
unsigned long millis_init;
unsigned long millis_new;
const unsigned long PERIOD = 1000;

String speedMode = "Lapin";
 
void setup() {
  // On board serial comm: https://wiki.seeedstudio.com/xiao_esp32s3_pin_multiplexing/
  Serial1.begin(BAUD,SERIAL_8N1,RX_PIN,TX_PIN);
  
  pinMode(FALL_DETECTION_PIN, INPUT);

  // For serial debug
  //Serial.begin(115200);
 
  millis_init = millis();
}
 
void loop() {
  millis_new = millis();
  if (millis_new - millis_init >= PERIOD)
  {
    // broadcast values to student module (JSON - https://github.com/bblanchon/ArduinoJson) -------
    JsonDocument msg;  // needs to be static? (https://arduinojson.org/v6/api/jsondocument/createnestedobject/)
    msg["model_num"] = MODEL_NUMBER;


    JsonObject ctrl = msg["ctrl"].to<JsonObject>();
    ctrl["etat"] = random(3);
    ctrl["break_active"] = random(1);
    ctrl["speed_mode"] = speedMode;
    ctrl["fault_code"] = random(65535);
    ctrl["cmd"] = random(100);
 
    JsonObject drive = msg["drive"].to<JsonObject>();
    drive["temperature"] = ((float)random(-9000, 9000)) / 100.0;
    drive["tension_total"] = ((float)random(0, 9000)) / 100.0;
    drive["courant_total"] = ((float)random(-9000, 9000)) / 100.0;
    drive["energie"] = ((float)random(0, 9000)) / 100.0;
    drive["vitesse"] = ((float)random(0, 5000)) / 100.0;    
 
    serializeJson(msg, Serial1);
    Serial1.println();
 
    millis_init = millis();

    if (digitalRead (FALL_DETECTION_PIN))s
    {
      Serial.println ("Trottinette tombe");
    }
  }
}