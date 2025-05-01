/*
  @file   trottinetteAccelerometer.cpp, projet: Module de télémétrie
  @author Lucas Lalumière Longpré
  @date   10 Avril 2025
  @brief   Fichier d’en-tête définissant les fonctions pour utiliser l'accéléromètre
 *         ICM42670 du module de télémétrie d’une trottinette électrique.
 * Environnement de développement :
 *     - IDE : Visual Studio Code
 *     - Communication : I2C
 *
 * Fonctions principales :
 *     - initAccel() : Initialise l'accéléromètre avec une fréquence d’échantillonnage de 100 Hz et une plage de ±16G.
 *     - getPitch()  : Calcule et retourne l'angle de pitch basé sur les valeurs d'accélération (x, y, z).
 */

#include "trottinetteAccelerometer.h"

ICM42670 IMU(Wire,0); //Crée l'objet de l'accelerateur en I2C

//Variables de l'accéléromètre
int ret;
float x,y,z = 0;      //Valeurs X,Y,Z
int pitch = 0;          //Valeur de pitch
int pitch_offset = -41; //Offset pour le pitch

//Nom:    initAccel
//Param:  Aucun
//Return: Aucun
//Brief:  Initialise l'accéléromètre
void initAccel() {
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

//Nom:    getPitch
//Param:  Aucun
//Return: int pitch, valeur du pitch calculée
//Brief:  Calcule le pitch selon les valeurs x,y,z
int getPitch() {
    inv_imu_sensor_event_t imu_event;
  
    //Retourne les valeurs x,y,z
    IMU.getDataFromRegisters(imu_event);
    
    //Enregistre les valeurs x,y,z
    x = imu_event.accel[0];
    y = imu_event.accel[1];
    z = imu_event.accel[2];
  
    //Calcul du pitch
    pitch = atan2(y, sqrt(x * x + z * z)) * 180.0 / PI;  //Convertie x,y,z en pitch, puis passe de radians à degrés
    //pitch -= pitch_offset;  //Soustraction de l'offset
  
    return pitch;
}