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

ICM42670 IMU(Wire,0); //Crée l'objet de l'accelerateur en I2C

//Variables de l'accéléromètre
int ret;                //Variable pour initialiser l'accéléromètre
float x,y,z = 0.0;      //Valeurs X,Y,Z
int pitch = 0;          //Valeur de pitch
int pitch_offset = -41; //Offset pour le pitch


