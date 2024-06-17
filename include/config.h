/*
********************************************************************************
Author: JuanD Valenciano, jvalenciano@unal.edu.co
Date of creation: 15th Dec 2022
Project: TFT_display_for_oven-screen
Supported Hardware: Arduino Mega 2560
Arch: AVR
MCU: AtMega 2560
File: config.h
Version: 1.0
Date Version: NA
Compatibility: NA
Control Version:

    1.0.0   Initial version.

Comments:

    File to define operation and settings

Website: None!=NULL
********************************************************************************
*/

#ifndef _CONFIG_H_
#define _CONFIG_H_

//########## Info
#define HWVERSION                       "0.0.1"
#define FWVERSION                       "0.0.1-ALFA"
#define DATECOMPILATION                 __DATE__
#define TIMECOMPILATION                 __TIME__

//########## Config HW
#define SERIAL_DEBUG                    Serial
#define SERIAL_DEBUG_BAUDRATE           115200
#define DEBUG_HW                        1       // 1 -> Available, 0 -> Disable

// Device
#define TIME_2_START                    100     // mSec

// Temp
#define RREF                            430.0   // The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
                                                // The 'nominal' 0-degrees-C resistance of the sensor
                                                // 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL                        100.0
#define TEMP_MIN                        20
#define TEMP_MAX                        200

//Time
#define TIME_MIN                        5
#define TIME_MAX                        360

// Encoder
#define PIN_ENCODER_CLK                 47
#define PIN_ENCODER_DT                  43
#define PIN_ENCODER_SW                  41

// ArduinoMega
#define KEEP_ALIVE_LED                  13//49

// Display
#define MY_DISPLAY_WIDTH                    480
#define MY_DISPLAY_HEIGHT                   272
#define OFFSET_MY_DISPLAY_WIDTH(x)          ((x>=0&&x<=MY_DISPLAY_WIDTH) ? x : 0)
#define OFFSET_MY_DISPLAY_HEIGHT(y)         ((y>=0&&y<=MY_DISPLAY_HEIGHT) ? (24+(y)) : 24)

#endif/* _CONFIG_H_ */