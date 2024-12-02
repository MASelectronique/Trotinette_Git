#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <math.h>
#include <WiFi.h>
#include <TFT_eSPI.h>      //Librairie pour l'écran tactile
#include <MQTT.h>

//Paramètres de connexion au réseau et au broker MQTT
const char *ssid = "BC_iPhone";
const char *pass = "pipette1";
const char broker_address[] = "cgs.altecoop.ca";
WiFiClient net;
MQTTClient client;

//Définition des paramètres pour l'écran
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define FONT_SIZE 2
uint16_t calData[5] = { 610, 2638, 592, 3103, 4 };  //Valeurs de calibration de l'écran

//Définition des pattes pour communication I2C
#define SDA_PIN 18  
#define SCL_PIN 19 

//Définition des paramètres pour l'accéléromètre
const uint8_t MPU=0x68; //Adresse de l'accéléromètre
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
double pitch,roll;

TFT_eSPI tft = TFT_eSPI(); //Initialise l'écran tactile

void getAngle(int Vx,int Vy,int Vz);  //Fonction qui calcule le roulis et le tangage de l'accéléromètre
void connect(); //Fonction pour se connecter au WiFi

void setup() {
  Serial.begin(115200);

  tft.init();
  tft.setRotation(0); //Écran en mode vertical
  //Vide l'écran et met la couleur de la police à noir
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setTouch(calData);  //Calibre l'écran avec les valeurs de calibration

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  
  Wire.write(0);    
  Wire.endTransmission(true);

  //WiFi.begin(ssid, pass);
  //client.begin(broker_address, net);
  //connect();
}

void loop(){
  uint16_t t_x = 0, t_y = 0; // To store the touch coordinates

  //client.loop();
  //delay(10);

  /*if (!client.connected()) {
    connect();
  }*/

  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,(size_t)14,(bool)true);

  int AcXoff,AcYoff,AcZoff;

  //Acceleration data correction
  AcXoff = -950;
  AcYoff = -300;
  AcZoff = 0;

  //read accel data
  AcX=(Wire.read()<<8|Wire.read()) + AcXoff;
  AcY=(Wire.read()<<8|Wire.read()) + AcYoff;
  AcZ=(Wire.read()<<8|Wire.read()) + AcZoff;

  //get pitch/roll
  getAngle(AcX,AcY,AcZ);

  //send the data out the serial port
  Serial.print("Angle: ");
  Serial.print("Pitch = "); Serial.print(pitch);
  Serial.print(" | Roll = "); Serial.println(roll);
  tft.setTextColor(TFT_BLACK);
  tft.setCursor(0,80);
  tft.println("Angle: ");
  tft.print("Pitch = "); tft.print(pitch);
  tft.print(" | Roll = "); tft.println(roll);

  bool pressed = tft.getTouch(&t_x, &t_y);

  if (pressed == true) {
    tft.print("SCREEN TOUCHED");
  }

  delay(500);
  tft.fillScreen(TFT_WHITE);
}


/*Fonction pour convertir les valeurs x, y et z 
  provenant de l'accéléromètre en valeurs de roulis et 
  de tangage.
  Param: int Vx, Vy, Vz*/
void getAngle(int Vx,int Vy,int Vz) {
  double x = Vx;
  double y = Vy;
  double z = Vz;

  pitch = atan(x/sqrt((y*y) + (z*z)));
  roll = atan(y/sqrt((x*x) + (z*z)));
  //convert radians into degrees
  pitch = pitch * (180.0/3.14);
  roll = roll * (180.0/3.14);
}

void connect() {
  Serial.print("checking wifi...");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  /*Serial.print("\nconnecting...");
  while (!client.connect("arduino", "public", "public")) {
    Serial.print(".");
    delay(1000);
  }*/

  Serial.println("\nconnected to WiFi!");

  //client.subscribe("/hello");
  // client.unsubscribe("/hello");
}