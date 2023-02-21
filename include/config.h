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
#define HWVERSION                       "0.0.0"
#define FWVERSION                       "0.0.0"
#define DATECOMPILATION                 __DATE__
#define TIMECOMPILATION                 __TIME__

//########## Config
#define SERIAL_DEBUG                    Serial
#define SERIAL_DEBUG_BAUDRATE           115200


#define RREF                            430.0   // The value of the Rref resistor. Use 430.0 for PT100 and 4300.0 for PT1000
                                                // The 'nominal' 0-degrees-C resistance of the sensor
                                                // 100.0 for PT100, 1000.0 for PT1000
#define RNOMINAL                        100.0

#define PIN_ENCODER_CLK                 47
#define PIN_ENCODER_DT                  43
#define PIN_ENCODER_SW                  41

//ArduinoMega
#define KEEP_ALIVE_LED                  13//49

#endif/* _CONFIG_H_ */