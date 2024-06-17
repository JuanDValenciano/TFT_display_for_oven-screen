/*
********************************************************************************
Author: JuanD Valenciano, jvalenciano@unal.edu.co
Date of creation: 31th March 2023
Project: TFT_display_for_oven-screen
Supported Hardware: Arduino Mega 2560
Arch: AVR
MCU: AtMega 2560
File: global_variables.h
Version: 1.0
Date Version: NA
Compatibility: NA
Control Version:

    1.0.0   Initial version.

Comments:

    File to define global variables.

Website: None!=NULL
********************************************************************************
*/

#ifndef _GLOBAL_VARIABLES_H_
#define _GLOBAL_VARIABLES_H_

//Encoder
typedef enum{
    DIR_COUNTERCLOCKWISE = -1,
    DIR_NOROTATION = 0,
    DIR_CLOCKWISE = 1,
}directionEnum;

//StateMachine
typedef enum{
    STATE_MACHINE_MODE_EDIT=0,
    STATE_MACHINE_MODE_EDIT_USING_ENCODER,
    STATE_MACHINE_MODE_EDIT_USING_SW,
    STATE_MACHINE_MODE_IDLE,
}state_machine_mode_t;

//displayLayoutEditSelector
enum{
    DISPLAY_SELECTOR_SET_TIME=0,
    DISPLAY_SELECTOR_SAVE_AND_START,
    DISPLAY_SELECTOR_SAVE,
    DISPLAY_SELECTOR_SET_TEMP,
    DISPLAY_SELECTOR_CLEAR,
};

int8_t     positionSelection = 0;
bool       positionSelectionFlag = false;

bool       editSelectionFlag = false;

#endif /* _GLOBAL_VARIABLES_H_ */
