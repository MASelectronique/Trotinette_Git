/*
  @file   trottinetteAccelerometer.h, projet: Module de télémétrie
  @author Lucas Lalumière Longpré
  @date   14 Avril 2025
  @brief  Fichier header contenant les variables et les prototypes de fonctions 
          pour se servir de l'accéléromètre du module de télémétrie.
  Environnement:
      Développement: Visual Studio Code
*/
#include <Arduino.h>
#include <ICM42670P.h>      //Lib pour accéléromètre
#include <math.h>           //Utilisé pour calcul de pitch

#define INT_PIN 5 //Interrupt

void initAccel();   //Initialise l'accéléromètre
int getPitch(); //Calcul du pitch selon valeurs 

extern int pitch;  //Variable de pitch


