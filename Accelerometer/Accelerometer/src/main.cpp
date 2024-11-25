/*
  Sketch to generate the setup() calibration values, these are reported
  to the Serial Monitor.

  The sketch has been tested on the ESP8266 and screen with XPT2046 driver.
*/

#include <SPI.h>
#include <TFT_eSPI.h>      // Hardware-specific library

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

void touch_calibrate();

//------------------------------------------------------------------------------------------

void setup() {
  // Use serial port
  Serial.begin(115200);

  // Initialise the TFT screen
  tft.init();

  // Set the rotation to the orientation you wish to use in your project before calibration
  // (the touch coordinates returned then correspond to that rotation only)
  tft.setRotation(0);

  // Calibrate the touch screen and retrieve the scaling factors
  touch_calibrate();

/*
  // Replace above line with the code sent to Serial Monitor
  // once calibration is complete, e.g.:
  uint16_t calData[5] = { 286, 3534, 283, 3600, 6 };
  tft.setTouch(calData);
*/

  // Clear the screen
  tft.fillScreen(TFT_BLACK);
  tft.drawCentreString("Touch screen to test!",tft.width()/2, tft.height()/2, 2);
}

//------------------------------------------------------------------------------------------

void loop(void) {
  uint16_t x = 0, y = 0; // To store the touch coordinates

  // Pressed will be set true is there is a valid touch on the screen
  bool pressed = tft.getTouch(&x, &y);

  // Draw a white spot at the detected coordinates
  if (pressed) {
    tft.fillCircle(x, y, 2, TFT_WHITE);
    //Serial.print("x,y = ");
    //Serial.print(x);
    //Serial.print(",");
    //Serial.println(y);
  }
}

//------------------------------------------------------------------------------------------

// Code to run a screen calibration, not needed when calibration values set in setup()
void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // Calibrate
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(20, 0);
  tft.setTextFont(2);
  tft.setTextSize(1);
  tft.setTextColor(TFT_WHITE, TFT_BLACK);

  tft.println("Touch corners as indicated");

  tft.setTextFont(1);
  tft.println();

  tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

  Serial.println(); Serial.println();
  Serial.println("// Use this calibration code in setup():");
  Serial.print("  uint16_t calData[5] = ");
  Serial.print("{ ");

  for (uint8_t i = 0; i < 5; i++)
  {
    Serial.print(calData[i]);
    if (i < 4) Serial.print(", ");
  }

  Serial.println(" };");
  Serial.print("  tft.setTouch(calData);");
  Serial.println(); Serial.println();

  tft.fillScreen(TFT_BLACK);
  
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.println("Calibration complete!");
  tft.println("Calibration code sent to Serial port.");

  delay(4000);
}
/*#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <math.h>
#include <WiFi.h>
#include <TFT_eSPI.h>      // Hardware-specific library
#include <FS.h>
#include <SPIFFS.h>

const uint8_t MPU=0x68;
int16_t AcX,AcY,AcZ,Tmp,GyX,GyY,GyZ;
double pitch,roll;

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define FONT_SIZE 2

// This is the file name used to store the calibration data
// You can change this to create new calibration files.
// The SPIFFS file name must start with "/".
#define CALIBRATION_FILE "/TouchCalData1"

// Set REPEAT_CAL to true instead of false to run calibration
// again, otherwise it will only be done once.
// Repeat calibration if you change the screen rotation.
#define REPEAT_CAL false

//Définition des pattes pour communication I2C
#define SDA_PIN 18  
#define SCL_PIN 19 

int x, y, z;

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

void getAngle(int Vx,int Vy,int Vz);

void setup() {
  Serial.begin(115200);

  tft.init();
  tft.setRotation(0);
  // Clear the screen before writing to it
  tft.fillScreen(TFT_WHITE);
  tft.setTextColor(TFT_BLACK, TFT_WHITE);
  tft.setTouch(calData);

  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.begin();
  Wire.beginTransmission(MPU);
  Wire.write(0x6B);  
  Wire.write(0);    
  Wire.endTransmission(true);
}

void loop(){
  uint16_t t_x = 0, t_y = 0; // To store the touch coordinates

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

  delay(1000);
  tft.fillScreen(TFT_WHITE);
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
}*/