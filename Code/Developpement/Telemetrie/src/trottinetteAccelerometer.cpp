/*
  @file   trottinetteAccelerometer.cpp, projet: Module de télémétrie
  @author Lucas Lalumière Longpré
  @date   10 Avril 2025
  @brief  Fichier header contenant les fonctions 
          pour se servir de l'accéléromètre du module de télémétrie.

  Environnement:
      Développement: Visual Studio Code
*/
#include "trottinetteAccelerometer.h"

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
    pitch -= pitch_offset;  //Soustraction de l'offset
  
    return pitch;
}