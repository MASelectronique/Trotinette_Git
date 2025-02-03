/* Correction: 17/20
Wire.write(0x3B); utiliser des constantes pour les registres
int16_t AcX,AcY,AcZ;	mettre un g_ devant. Est-ce obligatoire d'avoir ces variables globales? Sinon, mettre locales.
 
 Expliquer d'où viennent ces valeurs.
 //Valeurs d'offset pour X,Y,Z  
  AcXoff = -950;
  AcYoff = -300;
  AcZoff = 0;

if (g_pitch >= 30 || g_pitch <= -30)  Utiliser des constantes

Faire des fonctions dans la boucle principales.
*/


/**
  @file   main.cpp, projet: Prototype affichage Trottinette électrique
  @author Lucas Lalumière Longpré
  @date   7 Octobre 2024
  @brief  Code pour le prototype du système d'affichage du
		      projet Trottinette électrique. Communique par I2C 
          avec un accéléromètre MPU-6050, à partir des valeurs
          X,Y,Z de l'accéléromètre, on calcule les valeurs de roulis
          et de tangage, ces valeurs sont utile pour la detection de 
          chute de la trottinette. Les deux valeurs sont affichées
          à l'écran et au terminal série. L'écran est tactile et affiche
          un message lorsqu'on appuie dessus. La DEL du MCU s'allume pour
          indiquer que la trottinette est tombée.

  Environnement:
      Développement: Visual Studio 2022
  Matériel: -MCU = ESP32-C3 DevkitC-02
            -Écran = ILI9341
            -Accéléromètre = MPU-6050
*/
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <math.h>
#include <TFT_eSPI.h>      //Librairie pour l'écran tactile

//Définition des paramètres pour l'écran
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
uint16_t calData[5] = { 610, 2638, 592, 3103, 4 };  //Valeurs de calibration de l'écran

//Définition des pattes pour communication I2C
#define SDA_PIN 18  
#define SCL_PIN 19 

//Définition des paramètres pour l'accéléromètre
const uint8_t MPU=0x68; //Adresse de l'accéléromètre MPU 6050
int16_t AcX,AcY,AcZ;	//Valeurs X,Y,Z de l'accéléromètre
double g_pitch,g_roll;	//Variables de roulis (roll) et de tangage (pitch)

TFT_eSPI tft = TFT_eSPI(); //Déclare un objet pour l'écran tactile

void getAngle(int Vx,int Vy,int Vz);  //Fonction qui calcule le roulis et le tangage de l'accéléromètre

void setup() {
  Serial.begin(115200);

  tft.init();	//Initialise l'écran tactile
  tft.setRotation(0); // 0 = (0,0) en haut à gauche/ 1 = (0,0) est bas à gauche/ 2 = (0,0) est haut à droite / 3 = en bas à droite
  //Vide l'écran et met la couleur de la police à noir
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setTouch(calData);  //Calibre l'écran avec les valeurs de calibration

  pinMode(LED_BUILTIN, OUTPUT);	//Initialise la DEL du MCU comme une sortie
  Wire.begin(SDA_PIN, SCL_PIN);	//Initialise la communication I2C
  Wire.beginTransmission(MPU);	//Commence la communication avec l'accéléromètre
  Wire.write(0x6B);   //Écrit au registre 0x6B PWR_MGMT1 (Power management)
  Wire.write(0);      //Met tous les bits de ce registre à 0, ce qui désactive le mode "sleep"
  Wire.endTransmission(true); //Arrête la transmission
}

void loop(){
  uint16_t t_x = 0, t_y = 0; //Variables pour storer les coordonnées du "touch"
  int AcXoff,AcYoff,AcZoff; //Variables d'offset pour X,Y,Z

  digitalWrite(LED_BUILTIN, LOW);	//Met la DEL à un niveau bas

  Wire.beginTransmission(MPU);  //Recommence la transmission avec l'accéléromètre
  Wire.write(0x3B); //Adresse le registre 0x3B (ACCEL_XOUT)
  Wire.endTransmission(false);  //Indique qu'on veut continuer la transmission
  Wire.requestFrom(MPU,(size_t)6,(bool)true);  //Fait une requête de données de 6 octets à l'accéléromètre en "repeated start"

  //Valeurs d'offset pour X,Y,Z
  AcXoff = -950;
  AcYoff = -300;
  AcZoff = 0;

  //Lit les données de l'accéléromètre
  AcX=(Wire.read()<<8|Wire.read()) + AcXoff;  //Combine les 8 bits LSB et les 8 bits MSB + le offset
  AcY=(Wire.read()<<8|Wire.read()) + AcYoff;
  AcZ=(Wire.read()<<8|Wire.read()) + AcZoff;

  //Calcule le roulis et tangage
  getAngle(AcX,AcY,AcZ);

  //Affiche les valeurs de roulis et de tangage au terminal série
  Serial.print("Angle: ");
  Serial.print("Pitch = "); Serial.print(g_pitch);
  Serial.print(" | Roll = "); Serial.println(g_roll);
  //Met la grosseur du tecte à 2, la couleur du texte est noire
  tft.setTextColor(TFT_BLACK);
  tft.setCursor(0,80);
  tft.setTextSize(2);
  //Affiche les valeurs de roulis et de tangage à l'écran
  tft.println("Angle: ");
  tft.print("Pitch = "); tft.println(g_pitch);
  tft.print("Roll = "); tft.println(g_roll);

  bool pressed = tft.getTouch(&t_x, &t_y);  //Vérifie si l'écran a été touché

  if (pressed == true) {  //Si l'utilisateur touche l'écran
    tft.setCursor(15,150);
    tft.print("SCREEN TOUCHED");  //Affiche un message à l'écran
  }

  if (g_pitch >= 30 || g_pitch <= -30)  { //Si le tangage est en haut de 30 ou plus bas que -30 (Trottinette tombée sur le côté)
    digitalWrite(LED_BUILTIN, HIGH);	//Allume la DEL du MCU
  }
  else if (g_roll >= 30 || g_roll <= -30) { //Si le roulis est en haut de 30 ou plus bas que -30 (Trottinette tombée vers l'avant ou vers l'arrière)
    digitalWrite(LED_BUILTIN, HIGH);
  }

  delay(500);
  tft.fillScreen(TFT_WHITE);  //Vide l'écran
}


/*Brief: Fonction pour convertir les valeurs x, y et z 
  provenant de l'accéléromètre en valeurs de roulis et 
  de tangage. Utile pour détecter si la trottinette est
  tombée sur le côté.
  Param: int Vx, Vy, Vz
  Return: Aucun
  */
void getAngle(int Vx,int Vy,int Vz) {
  double x = Vx;
  double y = Vy;
  double z = Vz;

  g_pitch = atan(x/sqrt((y*y) + (z*z)));  //Calcul du tangage en radian
  g_roll = atan(y/sqrt((x*x) + (z*z)));   //Calcul du roulis en radian
  //Convertie les valeurs en degré
  g_pitch = g_pitch * (180.0/3.14);
  g_roll = g_roll * (180.0/3.14);
}
