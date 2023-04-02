//360 degree lidar. Just for fun. (c)Airrr(r).

#include <Wire.h>
#include "FastTrig.h"

#include <VL53L0X.h>
VL53L0X sensor;

#include <TFT_ILI9341.h>
#include <SPI.h>
TFT_ILI9341 tft = TFT_ILI9341();

bool FC = true;                      //first circle
byte mSpeed = 245;  //188
int range;

const byte readPerTurn = 24;         //desired readings per turn!!! minimum 20 => max speed! more readings => slow speed
byte c = 0;
volatile bool loopTog = false;
byte tot = readPerTurn;

int wx[readPerTurn + 2];             //three more
int wy[readPerTurn + 2];             //three more
int wxd[readPerTurn + 2];            //three more
int wyd[readPerTurn + 2];            //three more
int k = 180;



void setup() {
  tft.init();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setCursor(0, 0);
  tft.println("360LIDAR");

  pinMode(2, INPUT_PULLUP);    //360 trigger input
  pinMode(3, OUTPUT);          //MOTOR PWM output (via transistor)
  analogWrite(3, mSpeed);
  pinMode (9, OUTPUT);         //LCD backlight.
  digitalWrite(9, HIGH);       //on

  attachInterrupt(digitalPinToInterrupt(2), turn, FALLING);
  Serial.begin(115200);
  Wire.begin();
  TWBR = 10;                   //i2c 10=242Hz,18=200Hz;

  sensor.setTimeout(500);
  if (!sensor.init())  {
    Serial.println("Failed to detect and initialize sensor!");
    while (1) {}
  }
  sensor.startContinuous();
  sensor.setMeasurementTimingBudget(20000);  //faster!
}


void loop() {

  range = sensor.readRangeContinuousMillimeters();
  if (range > 1200) range = 1200 - random(30);      //for visual

  wx[c] = float(isin(k) * range / 10);
  wy[c] = float(icos(k) * range / 10);
  k = k - float(360 / readPerTurn);
  c++;
  tot++;

  if (c > 1) {
    tft.drawLine(wx[c - 2] + 200, wy[c - 2] + 120, wx[c - 1] + 200, wy[c - 1] + 120, TFT_WHITE);
    tft.drawLine(wxd[c - 2] + 200, wyd[c - 2] + 120, wxd[c - 1] + 200, wyd[c - 1] + 120, TFT_BLACK);
  }


  if (c == 25) {
    tft.drawLine(wx[c - 1] + 200, wy[c - 1] + 120, wx[0] + 200, wy[0] + 120, TFT_WHITE);
    tft.drawLine(wxd[c - 1] + 200, wyd[c - 1] + 120, wxd[0] + 200, wyd[0] + 120, TFT_BLACK);
  }

  if (FC == true) tft.fillScreen(TFT_BLACK);

  if ((loopTog == true) || (c > readPerTurn)) {

    if ((tot > 5) && (loopTog == true)) {
      if (tot > readPerTurn) mSpeed++;
      if (tot < readPerTurn) mSpeed--;
      if (mSpeed > 255) mSpeed = 255;
      if (mSpeed < 70) mSpeed = 70;
      analogWrite(3, mSpeed);

      tft.setCursor(0, 0);
      tft.println("         ");
      tft.print("         ");
      tft.setCursor(0, 0);
      tft.print("SPT: ");
      tft.println(tot);
      tft.print("PWM: ");
      tft.println(mSpeed);  //mSpeed //pwm


    }

    loopTog = false;

    tot = 0;
    c = 0;
    k = 180;
    FC = false;

    for (byte e = 0; e < 25; e++) {
      wxd[e] = wx[e];
      wyd[e] = wy[e];
    }
  }

}



void turn() {
  loopTog = true;
}
