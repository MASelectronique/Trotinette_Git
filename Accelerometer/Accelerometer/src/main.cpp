#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <math.h>
#include <Adafruit_GFX.h>       // include Adafruit graphics library
#include <Adafruit_ILI9341.h>   // include Adafruit ILI9341 TFT library

const int MPU=0x68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
double pitch,roll;

//Définition des pattes pour communication I2C
#define SDA_PIN 18  
#define SCL_PIN 19 

//Définition des pattes pour communication SPI
#define CS_PIN 0  
#define MOSI_PIN 1  
#define MISO_PIN 2   
#define SCK_PIN 3   

void getAngle(int Vx,int Vy,int Vz);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Wire.begin(SDA_PIN, SCL_PIN);
  SPI.begin(SCK_PIN, MISO_PIN, MOSI_PIN, CS_PIN);
  digitalWrite(CS_PIN,HIGH);
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  
  Wire.write(0);    
  Wire.endTransmission(true);
  Serial.begin(9600);
}

void loop(){
  Wire.beginTransmission(MPU);
  Wire.write(0x3B);
  Wire.endTransmission(false);
  Wire.requestFrom(MPU,14,true);

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
  delay(1000);
}

//convert the accel data to pitch/roll
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