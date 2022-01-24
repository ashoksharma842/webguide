/**
* Module : lcd.h
* Project : Web Guide Controller
* Originated : 2021.12.17
*
* Overview : definations required for displaying of web guide.
*
*/
#ifndef LCD_H
#define LCD_H

#include <Arduino.h>
#include "controller.h"
#include "FS.h"
#include <SPI.h>
#include <TFT_eSPI.h>

#define KEY_TEXTSIZE 1   // Font size multiplier

#define  SENSOR1_BUTTON   0
#define  SENSOR2_BUTTON   1
#define  AUTO_BUTTON      2
#define  MANUAL_BUTTON    3
#define  SC_BUTTON        4
#define  FB_BUTTON        5
#define  LEFT_BUTTON      6
#define  RIGHT_BUTTON     7

#define SAMPLE_COUNT      10

// This is the file name used to store the calibration data
// You can change this to create new calibration files.
// The SPIFFS file name must start with "/".
#define CALIBRATION_FILE "/TouchCalData1"

// Set REPEAT_CAL to true instead of false to run calibration
// again, otherwise it will only be done once.
// Repeat calibration if you change the screen rotation.
#define REPEAT_CAL false
void touch_calibrate();
void drawButtons(TFT_eSPI_Button key[]);
uint16_t width_per100(uint8_t widthPer100);
uint16_t height_per75(uint8_t heightPer75);
void displaySensor(uint8_t guidingMode, int16_t color);
void displaySensorData(uint8_t guidingMode, volatile int sensorData);
void displayFeedbackData(uint8_t feedBackType, volatile int feedbackData);
void displayCurrentData(volatile int currentData);
void displayGain(volatile int argGain);
int getDispEdgeData(volatile int* edgeData);
int getDispFeedbackData(volatile int* feedbackData);

#endif //LCD_H
