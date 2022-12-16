/*
********************************************************************************
Author: JuanD Valenciano, jvalenciano@unal.edu.co
Date of creation: 15th Dec 2022
Project: TFT_display_for_oven-screen
Supported Hardware: Arduino Mega 2560
Arch: AVR
MCU: AtMega 2560
File: main.cpp
Version: 1.0
Date Version: NA
Compatibility: NA
Control Version:
    1.0.0   Initial version.

Comments:

Website: None!=NULL
********************************************************************************
*/

#include <Arduino.h>
#include <Wire.h>
#include "RTClib.h"
#include "AT24CX.h"
#include "Adafruit_MAX31865.h"
#include "RotaryEncoder.h"
#include "config.h"

//Struc
RTC_DS1307  RTC;
AT24C32     eepromRTC;
Adafruit_MAX31865 thermo = Adafruit_MAX31865(53);// use hardware SPI, just pass in the CS pin
RotaryEncoder encoder(PIN_ENCODER_CLK, PIN_ENCODER_DT, RotaryEncoder::LatchMode::TWO03);// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
//Global variables
DateTime    now;
bool        statusLed = false;
uint16_t    rtd;
int         pos;
int         newPos;

void setup() {
  delay(200);//200mSegToStart!
  //ConfigBoard..
  pinMode(KEEP_ALIVE_LED, OUTPUT);
  pinMode(PIN_ENCODER_SW, INPUT);
  SERIAL_DEBUG.begin(SERIAL_DEBUG_BAUDRATE, SERIAL_8N1);
  Wire.begin();
  RTC.begin();
  thermo.begin(MAX31865_3WIRE);  // set to 2WIRE or 4WIRE as necessary
  if (! RTC.isrunning()) {
    SERIAL_DEBUG.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }
  SERIAL_DEBUG.println(">>>Start<<<");
}

void loop() {
  /*
  //RTC
  now = RTC.now();
  SERIAL_DEBUG.print(now.year(), DEC);
  SERIAL_DEBUG.print('/');
  SERIAL_DEBUG.print(now.month(), DEC);
  SERIAL_DEBUG.print('/');
  SERIAL_DEBUG.print(now.day(), DEC);
  SERIAL_DEBUG.print(' ');
  SERIAL_DEBUG.print(now.hour(), DEC);
  SERIAL_DEBUG.print(':');
  SERIAL_DEBUG.print(now.minute(), DEC);
  SERIAL_DEBUG.print(':');
  SERIAL_DEBUG.print(now.second(), DEC);
  SERIAL_DEBUG.println();
  //MAX331865
  rtd = thermo.readRTD();

  SERIAL_DEBUG.print("RTD value: "); SERIAL_DEBUG.println(rtd);
  float ratio = rtd;
  ratio /= 32768;
  SERIAL_DEBUG.print("Ratio = "); SERIAL_DEBUG.println(ratio,8);
  SERIAL_DEBUG.print("Resistance = "); SERIAL_DEBUG.println(RREF*ratio,8);
  SERIAL_DEBUG.print("Temperature = "); SERIAL_DEBUG.println(thermo.temperature(RNOMINAL, RREF));
        */
  //Encoder
  /*
  encoder.tick();

  newPos = encoder.getPosition();
  if (pos != newPos) {
    SERIAL_DEBUG.print("pos:");
    SERIAL_DEBUG.print(newPos);
    SERIAL_DEBUG.print(" dir:");
    SERIAL_DEBUG.println((int)(encoder.getDirection()));
    pos = newPos;
  }
  */
  //SERIAL_DEBUG.print("SW: ");
  //SERIAL_DEBUG.println(digitalRead(PIN_ENCODER_SW));
  //delay(150);
  /*
  //KeepAlive.......
  digitalWrite(KEEP_ALIVE_LED, statusLed=!statusLed);
  SERIAL_DEBUG.println("HolaMundo..........");
  SERIAL_DEBUG.flush();
  delay(1000);
  */
}