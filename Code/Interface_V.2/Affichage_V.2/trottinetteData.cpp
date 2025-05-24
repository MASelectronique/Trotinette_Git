/*
 * @file   trottinetteData.cpp, projet: Module de télémétrie
 * @brief   Implémentation des fonctions liées à la gestion des données de la trottinette.
 *          Ce fichier fait partie du projet de module de télémétrie. Il contient les fonctions
 *          permettant de traiter et d’afficher les données reçues en format JSON, en les stockant
 *          dans une structure de type `Data`.
 * 
 * Fonctions principales :
 *     - getData(JsonDocument doc) : Décode un objet JSON et remplit la structure `Data`.
 *     - printData(const Data &t)  : Affiche les données stockées dans la structure `Data` sur le moniteur série.
 * 
 * @author Lucas Lalumière Longpré
 * @date   10 avril 2025
 * 
 * Environnement de développement :
 *     - IDE : Visual Studio Code
 *     - Format de communication : JSON (via ArduinoJson)
 */
#include "trottinetteData.h"

Data trottinette; //L'objet trottinette contenant tous les données de la trottinette

JsonDocument doc;   //Objet JSON reçu
String jsonString;  //Objet JSON décodé
String jsonErrorMessage;  //Json a affiché si erreur de décodage
DeserializationError jsonError;  //Variable qui retourne le résultat du décodage du JSON

//Nom:    getData
//Param:  JsonDocument doc, l'objet json décodé
//Return: aucun
//Brief:  Assigne les valeurs json au struct de type Data
void getData(JsonDocument doc) {
  if (doc["drive"]["tension_total"].as<float>() != 0 &&
   doc["drive"]["temperature"].as<float>() != 0)
  {
    trottinette.model_num = doc["model_num"];                         //Numéro de la trottinette
  
    trottinette.ctrl.etat = doc["ctrl"]["etat"];                      //État de la trottinette
    trottinette.ctrl.break_active = doc["ctrl"]["break_active"];      //État du frein
    trottinette.ctrl.speed_mode = doc["ctrl"]["speed_mode"].as<String>();          //Mode de vitesse rapide (lapin) ou lente (tortue)
    trottinette.ctrl.fault_code = doc["ctrl"]["fault_code"];          //Code d'erreur
    trottinette.ctrl.cmd = doc["ctrl"]["cmd"];                        //Pourcentage du throttle
  
    trottinette.drive.temperature = doc["drive"]["temperature"];      //Température du moteur
    trottinette.drive.tension_total = doc["drive"]["tension_total"];  //Tension totale au moteur
    trottinette.drive.courant_total = doc["drive"]["courant_total"];  //Courant total au moteur
    trottinette.drive.energie = doc["drive"]["energie"];              //Puissance consommée par le moteur
    trottinette.drive.vitesse = doc["drive"]["vitesse"].as<String>(); //Vitesse de la trottinette
  }
  }
  
  //Nom:    printData
  //Param:  const Data &t, la struct de type Data
  //Return: aucun
  //Brief:  Affiche les valeurs contenues dans la struct de type Data
  void printData(const Data &t) {
    Serial.println("==== Trottinette Data ====\n");
    Serial.print("Model Num: "); Serial.println(t.model_num);
  
    Serial.print("-- Ctrl --\n");
    Serial.print("Etat: "); Serial.println(t.ctrl.etat);
    Serial.print("Break Active: "); Serial.println(t.ctrl.break_active);
    Serial.print("Speed mode: "); Serial.println(t.ctrl.speed_mode);
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
