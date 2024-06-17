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
#include <math.h>
#include "RTClib.h"
#include "AT24CX.h"
#include "Adafruit_MAX31865.h"
#include "RotaryEncoder.h"
//#include "Adafruit_GFX.h" // Hardware-specific library
#include "MCUFRIEND_kbv.h" //#include "TFT_PRINTGLUE.h"
#include "config.h"
#include "global_variables.h"

//Img on Flash
#include "static_images_display.h"

//Struc
RTC_DS1307                  RTC;
AT24C32                     eepromRTC;
Adafruit_MAX31865           thermo =  Adafruit_MAX31865(53);// use hardware SPI, just pass in the CS pin
RotaryEncoder               encoder(PIN_ENCODER_CLK, PIN_ENCODER_DT, PIN_ENCODER_SW, RotaryEncoder::LatchMode::TWO03);// Setup a RotaryEncoder with 2 steps per latch for the 2 signal input pins:
MCUFRIEND_kbv               tft; //TFT_PRINTGLUE
//###############################################################


//Global variables
//RTC
DateTime    now;
bool        statusLed = false;
char        dateTimeBuf[7] = {0};

//Temp
uint16_t    rtd;
uint16_t    setTemp = TEMP_MAX; //20<->200*C

//Time
uint16_t    setMinutes = TIME_MAX; //5<->360 Minutes

//Encoder

//Display
int a = 1000, b = 3500;
int n, f;
int j, j2 , lj;
int ii, i2 , li;
int pct = 0;
int d[5] = {20, 20, 20, 20, 20};
uint16_t col[5] = {0x7006, 0xF986, 0x6905, 0x7FF7, 0x024D};

//main
state_machine_mode_t stateValue = STATE_MACHINE_MODE_EDIT;
//###############################################################

void  drawRectMod(  int16_t x, int16_t y,
                    int16_t w, int16_t h,
                    uint16_t nLine,
                    uint16_t color)
{
  int16_t tmp_x = x, tmp_y = y, tmp_w = w, tmp_h = h;
  for (uint16_t i = 0; i < nLine; i++)
  {
    tft.drawRect( tmp_x/*X*/, tmp_y/*Y*/,
                  tmp_w /*Axis X*/, tmp_h /*Axis Y*/,
                  color/*Color*/);
    tmp_x++;   
    tmp_y++;
    tmp_w-=2;
    tmp_h-=2;
  }
}

void displayInit(void) {
  tft.reset();
  tft.begin(tft.readID());
  tft.setRotation(1);
}

void displayLoadLayout(void){
  tft.fillScreen(TFT_WHITE);
  //tft.drawRGBBitmap(  OFFSET_MY_DISPLAY_WIDTH(140),
  //                    OFFSET_MY_DISPLAY_HEIGHT(55), 
  //                    gauseMeter, 93, 169);
  tft.fillRect( OFFSET_MY_DISPLAY_WIDTH(120) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(60) /*Y*/ /*P1*/, 
                93 /*Axis X*/, 169 /*Axis Y*/,
                TFT_BLUE/*Color*/); //blackLabel on Top Display.
}

void displayLayoutEdit(void){
  tft.setTextColor(TFT_BLACK);
  tft.setTextSize(3);
  tft.setCursor(  OFFSET_MY_DISPLAY_WIDTH(7)  /*X*/, 
                  OFFSET_MY_DISPLAY_HEIGHT(7) /*Y*/);
  tft.print("HORNO: EDITAR");

  tft.setCursor(  OFFSET_MY_DISPLAY_WIDTH(40)  /*X*/, 
                  OFFSET_MY_DISPLAY_HEIGHT(120) /*Y*/);
  tft.print(setTemp);

  tft.setCursor(  OFFSET_MY_DISPLAY_WIDTH(320)  /*X*/, 
                  OFFSET_MY_DISPLAY_HEIGHT(40) /*Y*/);
  tft.print(setMinutes);

  tft.setCursor(  OFFSET_MY_DISPLAY_WIDTH(300)  /*X*/, 
                  OFFSET_MY_DISPLAY_HEIGHT(72) /*Y*/);
  tft.print("TIEMPO");

  drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(122) /*Y*/,
                185 /*Axis X*/, 70 /*Axis Y*/,
                3 /*nLine*/,
                TFT_BLACK/*Color*/);
  tft.setCursor(  OFFSET_MY_DISPLAY_WIDTH(280)  /*X*/, 
                  OFFSET_MY_DISPLAY_HEIGHT(132) /*Y*/);
  tft.print("GUARDAR E");
  tft.setCursor(  OFFSET_MY_DISPLAY_WIDTH(280)  /*X*/, 
                  OFFSET_MY_DISPLAY_HEIGHT(162) /*Y*/);
  tft.print("  INICAR");
  
  drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(212) /*Y*/,
                185 /*Axis X*/, 42 /*Axis Y*/,
                3 /*nLine*/,
                TFT_BLACK/*Color*/);
  tft.setCursor(  OFFSET_MY_DISPLAY_WIDTH(280)  /*X*/, 
                  OFFSET_MY_DISPLAY_HEIGHT(222) /*Y*/);
  tft.print("GUARDAR");
}

void displayLayoutEditSelector(int8_t _selector){
  switch (_selector)
  {
    case DISPLAY_SELECTOR_SET_TIME: //Time Selection
      {
        //Time selection
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(30) /*Y*/,
                185 /*Axis X*/, 70 /*Axis Y*/,
                3 /*nLine*/,
                TFT_BLUE/*Color*/);
        //Clear temperature selection
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(34) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(109) /*Y*/,
                65 /*Axis X*/, 42 /*Axis Y*/,
                3 /*nLine*/,
                TFT_WHITE/*Color*/);
        //Clear default blackLine button.
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(122) /*Y*/,
                185 /*Axis X*/, 70 /*Axis Y*/,
                3 /*nLine*/,
                TFT_BLACK/*Color*/);
        //Clear default blackLine button.
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(212) /*Y*/,
                185 /*Axis X*/, 42 /*Axis Y*/,
                3 /*nLine*/,
                TFT_BLACK/*Color*/);
        break;
      }
    case DISPLAY_SELECTOR_SAVE_AND_START:
      {
        //Clear Time selection
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(30) /*Y*/,
                185 /*Axis X*/, 70 /*Axis Y*/,
                3 /*nLine*/,
                TFT_WHITE/*Color*/);
        //Clear temperature selection
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(34) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(109) /*Y*/,
                65 /*Axis X*/, 42 /*Axis Y*/,
                3 /*nLine*/,
                TFT_WHITE/*Color*/);
        //Set default blackLine button.
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(122) /*Y*/,
                185 /*Axis X*/, 70 /*Axis Y*/,
                3 /*nLine*/,
                TFT_BLUE/*Color*/);
        //Clear default blackLine button.
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(212) /*Y*/,
                185 /*Axis X*/, 42 /*Axis Y*/,
                3 /*nLine*/,
                TFT_BLACK/*Color*/);
        break;
      }
    case DISPLAY_SELECTOR_SAVE:
      {
        //Clear Time selection
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(30) /*Y*/,
                185 /*Axis X*/, 70 /*Axis Y*/,
                3 /*nLine*/,
                TFT_WHITE/*Color*/);
        //Clear temperature selection
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(34) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(109) /*Y*/,
                65 /*Axis X*/, 42 /*Axis Y*/,
                3 /*nLine*/,
                TFT_WHITE/*Color*/);
        //Clear default blackLine button.
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(122) /*Y*/,
                185 /*Axis X*/, 70 /*Axis Y*/,
                3 /*nLine*/,
                TFT_BLACK/*Color*/);
        //Set default blackLine button.
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(212) /*Y*/,
                185 /*Axis X*/, 42 /*Axis Y*/,
                3 /*nLine*/,
                TFT_BLUE/*Color*/);
        break;
      }
    case DISPLAY_SELECTOR_SET_TEMP:
      {
        //Clear Time selection
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(30) /*Y*/,
                185 /*Axis X*/, 70 /*Axis Y*/,
                3 /*nLine*/,
                TFT_WHITE/*Color*/);
        //Set temperature selection
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(34) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(109) /*Y*/,
                65 /*Axis X*/, 42 /*Axis Y*/,
                3 /*nLine*/,
                TFT_BLUE/*Color*/);
        //Clear default blackLine button.
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(122) /*Y*/,
                185 /*Axis X*/, 70 /*Axis Y*/,
                3 /*nLine*/,
                TFT_BLACK/*Color*/);
        //Clear default blackLine button.
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(212) /*Y*/,
                185 /*Axis X*/, 42 /*Axis Y*/,
                3 /*nLine*/,
                TFT_BLACK/*Color*/);
        break;
      }
    case DISPLAY_SELECTOR_CLEAR:
      {
        //Clear Time selection
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(30) /*Y*/,
                185 /*Axis X*/, 70 /*Axis Y*/,
                3 /*nLine*/,
                TFT_WHITE/*Color*/);
        //Clear temperature selection
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(34) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(109) /*Y*/,
                65 /*Axis X*/, 42 /*Axis Y*/,
                3 /*nLine*/,
                TFT_WHITE/*Color*/);
        //Clear default blackLine button.
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(122) /*Y*/,
                185 /*Axis X*/, 70 /*Axis Y*/,
                3 /*nLine*/,
                TFT_BLACK/*Color*/);
        //Clear default blackLine button.
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(212) /*Y*/,
                185 /*Axis X*/, 42 /*Axis Y*/,
                3 /*nLine*/,
                TFT_BLACK/*Color*/);
        break;
      }
    default:
      break;
  }
}

void displayLayoutEditSelectorSW(int8_t _selector){
  switch (_selector)
  {
    case DISPLAY_SELECTOR_SET_TIME: //Time Selection
      {
        //Time selection
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(30) /*Y*/,
                185 /*Axis X*/, 70 /*Axis Y*/,
                3 /*nLine*/,
                TFT_YELLOW/*Color*/);
        //Clear temperature selection
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(34) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(109) /*Y*/,
                65 /*Axis X*/, 42 /*Axis Y*/,
                3 /*nLine*/,
                TFT_WHITE/*Color*/);
        //Clear default blackLine button.
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(122) /*Y*/,
                185 /*Axis X*/, 70 /*Axis Y*/,
                3 /*nLine*/,
                TFT_BLACK/*Color*/);
        //Clear default blackLine button.
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(212) /*Y*/,
                185 /*Axis X*/, 42 /*Axis Y*/,
                3 /*nLine*/,
                TFT_BLACK/*Color*/);
        break;
      }
    case DISPLAY_SELECTOR_SAVE_AND_START:
      {
        //Clear Time selection
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(30) /*Y*/,
                185 /*Axis X*/, 70 /*Axis Y*/,
                3 /*nLine*/,
                TFT_WHITE/*Color*/);
        //Clear temperature selection
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(34) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(109) /*Y*/,
                65 /*Axis X*/, 42 /*Axis Y*/,
                3 /*nLine*/,
                TFT_WHITE/*Color*/);
        //Set default blackLine button.
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(122) /*Y*/,
                185 /*Axis X*/, 70 /*Axis Y*/,
                3 /*nLine*/,
                TFT_YELLOW/*Color*/);
        //Clear default blackLine button.
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(212) /*Y*/,
                185 /*Axis X*/, 42 /*Axis Y*/,
                3 /*nLine*/,
                TFT_BLACK/*Color*/);
        break;
      }
    case DISPLAY_SELECTOR_SAVE:
      {
        //Clear Time selection
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(30) /*Y*/,
                185 /*Axis X*/, 70 /*Axis Y*/,
                3 /*nLine*/,
                TFT_WHITE/*Color*/);
        //Clear temperature selection
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(34) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(109) /*Y*/,
                65 /*Axis X*/, 42 /*Axis Y*/,
                3 /*nLine*/,
                TFT_WHITE/*Color*/);
        //Clear default blackLine button.
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(122) /*Y*/,
                185 /*Axis X*/, 70 /*Axis Y*/,
                3 /*nLine*/,
                TFT_BLACK/*Color*/);
        //Set default blackLine button.
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(212) /*Y*/,
                185 /*Axis X*/, 42 /*Axis Y*/,
                3 /*nLine*/,
                TFT_YELLOW/*Color*/);
        break;
      }
    case DISPLAY_SELECTOR_SET_TEMP:
      {
        //Clear Time selection
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(30) /*Y*/,
                185 /*Axis X*/, 70 /*Axis Y*/,
                3 /*nLine*/,
                TFT_WHITE/*Color*/);
        //Set temperature selection
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(34) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(109) /*Y*/,
                65 /*Axis X*/, 42 /*Axis Y*/,
                3 /*nLine*/,
                TFT_YELLOW/*Color*/);
        //Clear default blackLine button.
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(122) /*Y*/,
                185 /*Axis X*/, 70 /*Axis Y*/,
                3 /*nLine*/,
                TFT_BLACK/*Color*/);
        //Clear default blackLine button.
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(212) /*Y*/,
                185 /*Axis X*/, 42 /*Axis Y*/,
                3 /*nLine*/,
                TFT_BLACK/*Color*/);
        break;
      }
    case DISPLAY_SELECTOR_CLEAR:
      {
        //Clear Time selection
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(30) /*Y*/,
                185 /*Axis X*/, 70 /*Axis Y*/,
                3 /*nLine*/,
                TFT_WHITE/*Color*/);
        //Clear temperature selection
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(34) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(109) /*Y*/,
                65 /*Axis X*/, 42 /*Axis Y*/,
                3 /*nLine*/,
                TFT_WHITE/*Color*/);
        //Clear default blackLine button.
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(122) /*Y*/,
                185 /*Axis X*/, 70 /*Axis Y*/,
                3 /*nLine*/,
                TFT_BLACK/*Color*/);
        //Clear default blackLine button.
        drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(212) /*Y*/,
                185 /*Axis X*/, 42 /*Axis Y*/,
                3 /*nLine*/,
                TFT_BLACK/*Color*/);
        break;
      }
    default:
      break;
  }
}

void setup() {
  
  delay(TIME_2_START);//200mSegToStart!
  
  //pinMode(KEEP_ALIVE_LED, OUTPUT);

  SERIAL_DEBUG.begin(SERIAL_DEBUG_BAUDRATE, SERIAL_8N1);

  Wire.begin();

  RTC.begin();

  thermo.begin(MAX31865_3WIRE);  // set to 2WIRE or 4WIRE as necessary

  if (! RTC.isrunning()) {
    #if DEBUG_HW
      SERIAL_DEBUG.println(F("RTC is NOT running!"));
    #endif
    // following line sets the RTC to the date & time this sketch was compiled
    RTC.adjust(DateTime(__DATE__, __TIME__));
  }

  //Display Setup
  displayInit();
  displayLoadLayout();

  #if DEBUG_HW
    SERIAL_DEBUG.println(F(">>>Start<<<"));
  #endif

  while (1)
  {
    /* code */
  }
  
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
  
  if(pos>20){
    encoderPosReset();
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
  /*
  int width = tft.width();
  int height = tft.height();
  tft.fillScreen(colormask[aspect]);
  tft.drawRect(0, 0, width, height, WHITE);
  tft.drawRect(32, 32, width - 64, height - 64, WHITE);
  tft.setTextSize(2);
  tft.setTextColor(BLACK);
  tft.setCursor(40, 40);
  tft.print("ID=0x");
  tft.print(ID, HEX);
  if (ID == 0xD3D3){ tft.print(" w/o"); }
  tft.setCursor(40, 70);
  tft.print(aspectname[aspect]);
  tft.setCursor(40, 100);
  tft.print(width);
  tft.print(" x ");
  tft.print(height);
  tft.setTextColor(WHITE);
  tft.setCursor(40, 130);
  tft.print(colorname[aspect]);
  tft.setCursor(40, 160);
  tft.setTextSize(1);
  tft.print("MCUFRIEND_KBV_H_ = ");
  tft.print(version);
  if (++aspect > 3) aspect = 0;
  delay(5000);
  while (1) { }
  */

  // //Area Temp
  // drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(0) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(0) /*Y*/,
  //               270 /*Axis X*/, MY_DISPLAY_HEIGHT /*Axis Y*/,
  //               5 /*nLine*/,
  //               TFT_BLUE/*Color*/);
  // //Area Time
  // drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(0) /*Y*/,
  //               210 /*Axis X*/, MY_DISPLAY_HEIGHT /*Axis Y*/,
  //               5 /*nLine*/,
  //               TFT_GREENYELLOW/*Color*/);
  // /////////////////////////////////////////////////////////////                
  // //p1
  // drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(20) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(40) /*Y*/,
  //               40 /*Axis X*/, 40 /*Axis Y*/,
  //               5 /*nLine*/,
  //               TFT_BLUE/*Color*/);
  // //p2
  // drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(20) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(90) /*Y*/,
  //               40 /*Axis X*/, 40 /*Axis Y*/,
  //               5 /*nLine*/,
  //               TFT_CYAN/*Color*/);
  // //p3
  // drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(290) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(40) /*Y*/,
  //               40 /*Axis X*/, 40 /*Axis Y*/,
  //               5 /*nLine*/,
  //               TFT_DARKCYAN/*Color*/);
  // //p4
  // drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(290) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(90) /*Y*/,
  //               40 /*Axis X*/, 40 /*Axis Y*/,
  //               5 /*nLine*/,
  //               TFT_DARKGREY/*Color*/);
  // /////////////////////////////////////////////////////////////



  // //Area Temp
  // drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(0) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(0) /*Y*/,
  //               270 /*Axis X*/, MY_DISPLAY_HEIGHT /*Axis Y*/,
  //               5 /*nLine*/,
  //               TFT_BLUE/*Color*/);

  // tft.setTextColor(TFT_BLACK);
  // tft.setTextSize(3);
  // tft.setCursor(OFFSET_MY_DISPLAY_WIDTH(100) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(45) /*Y*/);
  // tft.print("Temp:");
  // tft.print(setTemp);
  // ///////////////////////////////////////////////////////////////////////////////////////
  // //Area Time
  // drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(270) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(0) /*Y*/,
  //               210 /*Axis X*/, MY_DISPLAY_HEIGHT /*Axis Y*/,
  //               5 /*nLine*/,
  //               TFT_GREENYELLOW/*Color*/);
  // tft.setCursor(OFFSET_MY_DISPLAY_WIDTH(280) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(45) /*Y*/);
  // tft.print("T:");
  // snprintf(dateTimeBuf, sizeof(dateTimeBuf), "%d%s%d", timeMinutes, ":", timeSecond);
  // tft.print(dateTimeBuf);
  // ///////////////////////////////////////////////////////////////////////////////////////
  // //Button
  // tft.setCursor(OFFSET_MY_DISPLAY_WIDTH(280) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(75) /*Y*/);
  // tft.print("Boton1");

  // tft.setCursor(OFFSET_MY_DISPLAY_WIDTH(280) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(105) /*Y*/);
  // tft.print("Boton2");


  // tft.fillRect( OFFSET_MY_DISPLAY_WIDTH(100) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(45) /*Y*/ /*P1*/,
  //               15 /*Axis X*/, 15 /*Axis Y*/,
  //               TFT_DARKGREEN/*Color*/); //blackLabel on Top Display.
  
  // tft.drawRect( OFFSET_MY_DISPLAY_WIDTH(100) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(45) /*Y*/ /*P1*/,
  //               17 /*Axis X*/, 23 /*Axis Y*/,
  //               TFT_RED);
  
  // tft.setTextSize(4);
  // tft.setCursor(OFFSET_MY_DISPLAY_WIDTH(100) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(80) /*Y*/);
  // tft.print(setTemp);

  
  // tft.drawRect( OFFSET_MY_DISPLAY_WIDTH(100) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(80) /*Y*/ /*P1*/,
  //               23 /*Axis X*/, 28 /*Axis Y*/,
  //               TFT_RED);

  // tft.setTextSize(2);
  // tft.setCursor(OFFSET_MY_DISPLAY_WIDTH(100) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(140) /*Y*/);
  // tft.print(setTemp);


  // tft.drawRect( OFFSET_MY_DISPLAY_WIDTH(100) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(140) /*Y*/ /*P1*/,
  //               13 /*Axis X*/, 17 /*Axis Y*/,
  //               TFT_RED);
  
  // tft.setTextSize(5);
  // tft.setCursor(OFFSET_MY_DISPLAY_WIDTH(100) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(180) /*Y*/);
  // tft.print(setTemp);


  // tft.drawRect( OFFSET_MY_DISPLAY_WIDTH(100) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(180) /*Y*/ /*P1*/,
  //               28 /*Axis X*/, 35 /*Axis Y*/,
  //               TFT_RED);
  
  // while (1){
  //   //Encoder
  //   encoder.tick();
  //   newPos = encoder.getPosition();
  //   if (pos != newPos) {
  //     direction = (int)encoder.getDirection();
      
  //     #if DEBUG_HW
  //       SERIAL_DEBUG.print("newPos:");
  //       SERIAL_DEBUG.print(newPos);
  //       SERIAL_DEBUG.print("pos:");
  //       SERIAL_DEBUG.print(pos);
  //       SERIAL_DEBUG.print(" dir:");
  //       SERIAL_DEBUG.println(direction);
  //     #endif
      
  //     switch (direction)
  //     {
  //       case DIR_COUNTERCLOCKWISE/* COUNTERCLOCKWISE */:
  //         {
  //           //setTemp--;
  //           positionSelection--;
  //           if(positionSelection<0){
  //             positionSelection = 0;
  //           }
  //           break;
  //         }
  //       case DIR_CLOCKWISE/* CLOCKWISE */:
  //         {
  //           //setTemp++;
  //           positionSelection++;
  //           if(positionSelection>3){
  //             positionSelection = 3;
  //           }
  //           break;
  //         }
  //       case DIR_NOROTATION/* NOROTATION */:
  //       default:
  //         break;
  //     }

  //     switch (positionSelection)
  //     {
  //       case 0/*TFT_BLUE*/:
  //         {
  //           //p1
  //           drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(20) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(40) /*Y*/,
  //                         40 /*Axis X*/, 40 /*Axis Y*/,
  //                         5 /*nLine*/,
  //                         TFT_RED/*Color*/);
  //           //p2
  //           drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(20) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(90) /*Y*/,
  //                         40 /*Axis X*/, 40 /*Axis Y*/,
  //                         5 /*nLine*/,
  //                         TFT_CYAN/*Color*/);
  //           //p3
  //           drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(290) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(40) /*Y*/,
  //                         40 /*Axis X*/, 40 /*Axis Y*/,
  //                         5 /*nLine*/,
  //                         TFT_DARKCYAN/*Color*/);
  //           //p4
  //           drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(290) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(90) /*Y*/,
  //                         40 /*Axis X*/, 40 /*Axis Y*/,
  //                         5 /*nLine*/,
  //                         TFT_DARKGREY/*Color*/);
  //           break; 
  //         }
  //       case 1/*TFT_CYAN*/:
  //         {
  //           //p1
  //           drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(20) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(40) /*Y*/,
  //                         40 /*Axis X*/, 40 /*Axis Y*/,
  //                         5 /*nLine*/,
  //                         TFT_BLUE/*Color*/);
  //           //p2
  //           drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(20) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(90) /*Y*/,
  //                         40 /*Axis X*/, 40 /*Axis Y*/,
  //                         5 /*nLine*/,
  //                         TFT_RED/*Color*/);
  //           //p3
  //           drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(290) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(40) /*Y*/,
  //                         40 /*Axis X*/, 40 /*Axis Y*/,
  //                         5 /*nLine*/,
  //                         TFT_DARKCYAN/*Color*/);
  //           //p4
  //           drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(290) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(90) /*Y*/,
  //                         40 /*Axis X*/, 40 /*Axis Y*/,
  //                         5 /*nLine*/,
  //                         TFT_DARKGREY/*Color*/);
  //           break; 
  //         }
  //       case 2/*TFT_DARKCYAN*/:
  //         {
  //           //p1
  //           drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(20) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(40) /*Y*/,
  //                         40 /*Axis X*/, 40 /*Axis Y*/,
  //                         5 /*nLine*/,
  //                         TFT_BLUE/*Color*/);
  //           //p2
  //           drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(20) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(90) /*Y*/,
  //                         40 /*Axis X*/, 40 /*Axis Y*/,
  //                         5 /*nLine*/,
  //                         TFT_CYAN/*Color*/);
  //           //p3
  //           drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(290) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(40) /*Y*/,
  //                         40 /*Axis X*/, 40 /*Axis Y*/,
  //                         5 /*nLine*/,
  //                         TFT_RED/*Color*/);
  //           //p4
  //           drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(290) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(90) /*Y*/,
  //                         40 /*Axis X*/, 40 /*Axis Y*/,
  //                         5 /*nLine*/,
  //                         TFT_DARKGREY/*Color*/);
  //           break; 
  //         }
  //       case 3/*TFT_DARKGREY*/:
  //         {
  //           //p1
  //           drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(20) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(40) /*Y*/,
  //                         40 /*Axis X*/, 40 /*Axis Y*/,
  //                         5 /*nLine*/,
  //                         TFT_BLUE/*Color*/);
  //           //p2
  //           drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(20) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(90) /*Y*/,
  //                         40 /*Axis X*/, 40 /*Axis Y*/,
  //                         5 /*nLine*/,
  //                         TFT_CYAN/*Color*/);
  //           //p3
  //           drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(290) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(40) /*Y*/,
  //                         40 /*Axis X*/, 40 /*Axis Y*/,
  //                         5 /*nLine*/,
  //                         TFT_DARKCYAN/*Color*/);
  //           //p4
  //           drawRectMod(  OFFSET_MY_DISPLAY_WIDTH(290) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(90) /*Y*/,
  //                         40 /*Axis X*/, 40 /*Axis Y*/,
  //                         5 /*nLine*/,
  //                         TFT_RED/*Color*/);
  //           break; 
  //         }
  //       default:
  //         break;
  //     }

  //     // tft.fillRect( OFFSET_MY_DISPLAY_WIDTH(100) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(45) /*Y*/ /*P1*/, 
  //     //               60 /*Axis X*/, 33 /*Axis Y*/,
  //     //               TFT_WHITE/*Color*/); //blackLabel on Top Display.
  //     // tft.setCursor(OFFSET_MY_DISPLAY_WIDTH(100) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(45) /*Y*/);
  //     // tft.print(setTemp);
  //     #if DEBUG_HW
  //       SERIAL_DEBUG.print("positionSelection:");
  //       SERIAL_DEBUG.println(positionSelection);
  //     #endif

  //     pos = newPos;
  //   }

  // }

  // tft.setTextColor(TFT_BLACK);
  // tft.setCursor(OFFSET_MY_DISPLAY_WIDTH(330) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(45) /*Y*/);
  // tft.setTextSize(3);
  // tft.print("MIN:SEC");
  // tft.setCursor(OFFSET_MY_DISPLAY_WIDTH(335) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(75) /*Y*/);
  // tft.print(999);
  // tft.print(":");
  // tft.println(59);


  /*
  tft.drawRGBBitmap(  OFFSET_MY_DISPLAY_WIDTH(10),
                      OFFSET_MY_DISPLAY_HEIGHT(60), 
                      gauseMeter, 315, 39);
  tft.drawRGBBitmap(  OFFSET_MY_DISPLAY_WIDTH(10),
                      OFFSET_MY_DISPLAY_HEIGHT(60)+39, 
                      gauseMeter2, 315, 39);
  */

  
  // tft.fillRect( OFFSET_MY_DISPLAY_WIDTH(10) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(10) /*Y*/ /*P1*/, 
  //               260 /*Axis X*/, 260 /*Axis Y*/,
  //               TFT_BLUE/*Color*/); //blackLabel on Top Display.
  
  // tft.drawRGBBitmap(  OFFSET_MY_DISPLAY_WIDTH(167),
  //                     OFFSET_MY_DISPLAY_HEIGHT(40), 
  //                     gauseMeter, 93, 170);
  // delayMicroseconds(1000);  

  // tft.drawRGBBitmap(  OFFSET_MY_DISPLAY_WIDTH(10),
  //                     OFFSET_MY_DISPLAY_HEIGHT(10)+40, 
  //                     gauseMeter2, 278, 40);
  // delayMicroseconds(1000);

  // tft.drawRGBBitmap(  OFFSET_MY_DISPLAY_WIDTH(10),
  //                     OFFSET_MY_DISPLAY_HEIGHT(10)+80, 
  //                     gauseMeter3, 278, 40);
  // delayMicroseconds(1000);
   
  //tft.drawRect( OFFSET_MY_DISPLAY_WIDTH(13) /*X*/, OFFSET_MY_DISPLAY_HEIGHT(14) /*Y*/,
  //              278 /*Axis X*/, 260 /*Axis Y*/,
  //              TFT_BLUE/*Color*/);
  switch (stateValue)
  {
    case STATE_MACHINE_MODE_EDIT:{
        #if DEBUG_HW
          SERIAL_DEBUG.println("STATE_MACHINE_MODE_EDIT");
        #endif
        displayLayoutEdit();
        displayLayoutEditSelector(positionSelection);
        stateValue = STATE_MACHINE_MODE_EDIT_USING_SW;
        break;
      }
    case STATE_MACHINE_MODE_EDIT_USING_ENCODER:{
      #if DEBUG_HW
        SERIAL_DEBUG.println("STATE_MACHINE_MODE_EDIT_USING_ENCODER");
      #endif
      positionSelectionFlag = false;
      switch (encoder.getDirection())
      {
        case RotaryEncoder::Direction::CLOCKWISE:{
            positionSelection++;
            if(positionSelection>DISPLAY_SELECTOR_SET_TEMP){
              positionSelection=DISPLAY_SELECTOR_SET_TIME;
            }
            positionSelectionFlag = true;
            break;
          }
        case RotaryEncoder::Direction::COUNTERCLOCKWISE:{
            positionSelection--;
            if(positionSelection<DISPLAY_SELECTOR_SET_TIME){
              positionSelection=DISPLAY_SELECTOR_SET_TEMP;
            }
            positionSelectionFlag = true;
            break;
          }          
        default:{
          break;
        }
      }
      if(positionSelectionFlag!=true){
        stateValue = STATE_MACHINE_MODE_EDIT_USING_SW;
      }else{
        stateValue = STATE_MACHINE_MODE_EDIT;
      }
      break;
      }
    case STATE_MACHINE_MODE_EDIT_USING_SW:{
      #if DEBUG_HW
        SERIAL_DEBUG.println("STATE_MACHINE_MODE_EDIT_USING_SW");
      #endif
      editSelectionFlag = false;
      if(encoder.getSW()!=false){
        displayLayoutEditSelectorSW(positionSelection);
        editSelectionFlag = true;
        stateValue = STATE_MACHINE_MODE_IDLE;
      }else{
        stateValue = STATE_MACHINE_MODE_EDIT_USING_ENCODER;
      }
      break;
      }
    case STATE_MACHINE_MODE_IDLE:{
        #if DEBUG_HW
          SERIAL_DEBUG.println("STATE_MACHINE_MODE_IDLE");
        #endif
        //stateValue = STATE_MACHINE_MODE_EDIT;
        break;
      }
    default:
      break;
  }
  encoder.tick();
}