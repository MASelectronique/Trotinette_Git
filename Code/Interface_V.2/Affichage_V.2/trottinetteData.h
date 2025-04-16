/*
  @file   trottinetteData.h, projet: Module de télémétrie
  @author Lucas Lalumière Longpré
  @date   10 Avril 2025
  @brief  Fichier header contenant les valeurs de la trottinette
          que nous devons afficher à l'écran. Contient aussi les
          variables et les fonctions pour reception de l'objet JSON
          envoyé par le contrôleur de la trottinette.

  Environnement:
      Développement: Visual Studio Code
*/
#include <Arduino.h>
#include <ArduinoJson.h>    //Lib pour JSON

struct Ctrl { //Struct qui contient les données du contrôleur
  int etat;
  int break_active;
  int fault_code;
  int cmd;  // À Afficher
};
  
struct Drive {  //Struct qui contient les données de la drive moteur
  int temperature;// À Afficher
  int tension_total;// À Afficher
  int courant_total;
  int energie;
  String vitesse; //string pour l'instant mais sera changé pour float   // À Afficher
};
  
struct Data { //Struct qui contient les données de la trottinette
  int model_num;
  Ctrl ctrl;
  Drive drive;
};
extern Data trottinette; //L'objet trottinette contenant tous les données de la trottinette

extern JsonDocument doc;   //Objet JSON reçu
extern String jsonString;  //Objet JSON décodé
extern String jsonErrorMessage;  //Json a affiché si erreur de décodage
extern DeserializationError jsonError;  //Variable qui retourne le résultat du décodage du JSON

void getData(JsonDocument);     //Fonction qui assigne les valeurs de l'objet json
void printData(const Data &t);  //Fonction pour afficher les données de la trottinette