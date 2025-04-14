/*
  @file   trottinette.h, projet: Module de télémétrie
  @author Lucas Lalumière Longpré
  @date   10 Avril 2025
  @brief  Fichier header contenant les valeurs de la trottinette
          que nous devons afficher à l'écran. Contient aussi les
          variables pour reception de JSON.

  Environnement:
      Développement: Visual Studio Code
*/
#include <Arduino.h>
#include <ArduinoJson.h>    //Lib pour JSON

struct Ctrl { //Struct qui contient les données du contrôleur
    int etat;
    int break_active;
    int fault_code;
    int cmd;
  };
  
  struct Drive {  //Struct qui contient les données de la drive moteur
    int temperature;
    int tension_total;
    int courant_total;
    int energie;
    String vitesse; //string pour l'instant mais sera changé pour float
  };
  
  struct Data { //Struct qui contient les données de la trottinette
    int model_num;
    Ctrl ctrl;
    Drive drive;
  };

Data trottinette; //L'objet trottinette contenant tous les données de la trottinette

JsonDocument doc;   //Objet JSON reçu
String jsonString;  //Objet JSON décodé
String jsonErrorMessage;  //Json a affiché si erreur de décodage
DeserializationError jsonError;  //Variable qui retourne le résultat du décodage du JSON

void getData(JsonDocument);     //Fonction qui assigne les valeurs de l'objet json
void printData(const Data &t);  //Fonction pour afficher les données de la trottinette