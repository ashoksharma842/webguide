/**
* Module : lcd.cpp
* Project : Web Guide Controller
* Originated : 2021.11.12
*
* Overview : definations required for displaying of web guide.
*
*/
#include "lcd.h"
#include <Arduino.h>
#include "FS.h"
#include <SPI.h>
#include <TFT_eSPI.h>

extern TFT_eSPI tft;
extern int32_t height, width;
uint16_t width_per100(uint8_t widthPer100)
{
  uint16_t widthInPixel = (widthPer100 * width) / 100;
  return widthInPixel;
}
uint16_t height_per75(uint8_t heightPer75)
{
  uint16_t heightInPixel = (heightPer75 * height) / 75;
  return heightInPixel;
}

void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // check file system exists
  if (!SPIFFS.begin()) {
    Serial.println("Formating file system");
    SPIFFS.format();
    SPIFFS.begin();
  }

  // check if calibration file exists and size is correct
  if (SPIFFS.exists(CALIBRATION_FILE)) {
    if (REPEAT_CAL)
    {
      // Delete if we want to re-calibrate
      SPIFFS.remove(CALIBRATION_FILE);
    }
    else
    {
      File f = SPIFFS.open(CALIBRATION_FILE, "r");
      if (f) {
        if (f.readBytes((char *)calData, 14) == 14)
          calDataOK = 1;
        f.close();
      }
    }
  }

  if (calDataOK && !REPEAT_CAL) {
    // calibration data valid
    tft.setTouch(calData);
  } else {
    // data not valid so recalibrate
    tft.fillScreen(TFT_BLACK);
    tft.setCursor(20, 0);
    tft.setTextFont(2);
    tft.setTextSize(1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);

    tft.println("Touch corners as indicated");

    tft.setTextFont(1);
    tft.println();

    if (REPEAT_CAL) {
      tft.setTextColor(TFT_RED, TFT_BLACK);
      tft.println("Set REPEAT_CAL to false to stop this running again!");
    }

    tft.calibrateTouch(calData, TFT_MAGENTA, TFT_BLACK, 15);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.println("Calibration complete!");

    // store data
    File f = SPIFFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}

void drawButtons(TFT_eSPI_Button key[])
{

  key[0].initButtonUL(&tft, width_per100(11), height_per75(20), width_per100(30), height_per75(35), TFT_BLACK, TFT_BLACK, TFT_BLACK, "", KEY_TEXTSIZE);
  key[1].initButtonUL(&tft, width_per100(61), height_per75(20), width_per100(30), height_per75(35), TFT_BLACK, TFT_BLACK, TFT_BLACK, "", KEY_TEXTSIZE);
  key[2].initButtonUL(&tft, width_per100(29), height_per75(8),  width_per100(12), height_per75(10), TFT_BLACK, TFT_CYAN, TFT_BLACK, "", KEY_TEXTSIZE);
  key[3].initButtonUL(&tft, width_per100(45), height_per75(8),  width_per100(12), height_per75(10), TFT_BLACK, TFT_YELLOW, TFT_BLACK, "", KEY_TEXTSIZE);
  key[4].initButtonUL(&tft, width_per100(61), height_per75(8),  width_per100(12), height_per75(10), TFT_BLACK, TFT_CYAN, TFT_BLACK, "", KEY_TEXTSIZE);
  key[5].initButtonUL(&tft, width_per100(45), height_per75(49), width_per100(12), height_per75(10), TFT_BLACK, TFT_GREEN, TFT_BLACK, "", KEY_TEXTSIZE);
  key[6].initButtonUL(&tft, width_per100(4), height_per75(61),  width_per100(19), height_per75(10), TFT_BLACK, TFT_CYAN, TFT_BLACK, "", KEY_TEXTSIZE);
  key[7].initButtonUL(&tft, width_per100(79), height_per75(61),  width_per100(19), height_per75(10), TFT_BLACK, TFT_CYAN, TFT_BLACK, "", KEY_TEXTSIZE);

  key[0].setLabelDatum(-10, 0, ML_DATUM);
  key[1].setLabelDatum(-10, 0, ML_DATUM);
  key[2].setLabelDatum(-5, 0, ML_DATUM);
  key[3].setLabelDatum(-5, 0, ML_DATUM);
  key[4].setLabelDatum(-5, 0, ML_DATUM);
  key[5].setLabelDatum(-15, 0, ML_DATUM);
  key[6].setLabelDatum(-5, 0, ML_DATUM);
  key[7].setLabelDatum(-5, 0, ML_DATUM);
  
  key[0].drawButton(false, "S1");
  key[1].drawButton(false, "S2");
  key[2].drawButton(false, "A");
  key[3].drawButton(false, "M");
  key[4].drawButton(false, "C");
  key[5].drawButton(false, "F/N");
  key[6].drawButton(false, "<");
  key[7].drawButton(false, ">");
}
void displaySensor(uint8_t guidingMode, int16_t color){
  if(guidingMode == SENSOR1){
    tft.fillRect(width_per100(11), height_per75(20), width_per100(30), height_per75(6), color);
    tft.fillRect(width_per100(11), height_per75(26), width_per100(7), height_per75(23), color);
    tft.fillRect(width_per100(11), height_per75(49), width_per100(30), height_per75(6), color);
  }else{
    tft.fillRect(width_per100(60), height_per75(20), width_per100(30), height_per75(6), color);
    // tft.fillRect(width_per100(84), height_per75(26), width_per100(7), height_per75(23), color);
    tft.fillRect(width_per100(84), height_per75(26), width_per100(6)+1, height_per75(23), color);
    tft.fillRect(width_per100(60), height_per75(49), width_per100(30), height_per75(6), color);
  }
}

void displaySensorData(uint8_t guidingMode, volatile int sensorData){
  volatile int dispData = ((sensorData) * 100) / 4095;
  static int prevDispData = 0;
  static int prevGuidingMode = 9;//not s1 or s2
  if(dispData == prevDispData) return;
  static int dispDataXpos = 75;
  int dispDataYpos = 77;
  if(prevGuidingMode == guidingMode){
    tft.setCursor(dispDataXpos, dispDataYpos);
    tft.setTextColor(TFT_CYAN, TFT_BLUE);
    tft.print(prevDispData);
    tft.setCursor(dispDataXpos, dispDataYpos);
    tft.setTextColor(TFT_BLACK, TFT_BLUE);
    tft.print(dispData);
  } else{
    prevGuidingMode = guidingMode;    
  }
  prevDispData = dispData;
//  Serial.println(dispData);
  int dispDataProcessed = map(dispData,0,100,0,25);
  tft.fillRect(width_per100(21), height_per75(36), width_per100(60), height_per75(2), TFT_BLACK);//clear all
  if(guidingMode == SENSOR1)
  {
    dispDataXpos = 75;
    tft.fillRect(width_per100(21+25-dispDataProcessed), height_per75(36), width_per100(dispDataProcessed), height_per75(2), TFT_RED);//center
  }
  else if(guidingMode == SENSOR2) 
  {
    dispDataXpos = 230;
    tft.fillRect(width_per100(21+25+10), height_per75(36), width_per100(dispDataProcessed), height_per75(2), TFT_RED);//center    
  }
}
void displayFeedbackData(uint8_t guidingMode, volatile int feedbackData){
  volatile int dispData = ((feedbackData) * 100) / 4095;
  static int prevDispData = 0;
  if(dispData == prevDispData) return;
//  Serial.println(dispData);
  int dispDataProcessed = map(dispData,0,100,0,50);
  tft.fillRect(width_per100(26), height_per75(61), width_per100(50), height_per75(4), TFT_BLACK);//bottom
  tft.fillRect(width_per100(26), height_per75(61), width_per100(dispDataProcessed), height_per75(4), TFT_RED);//bottom

  tft.setCursor(150, 206);
  tft.setTextColor(TFT_WHITE, TFT_RED);
  tft.print(dispData);
  prevDispData = dispData;
}
void displayCurrentData(volatile int currentData){
  volatile int dispData = ((currentData) * 100) / 4095;
  static int prevDispData = 0;
  if(dispData == prevDispData) return;
//  Serial.println(dispData);
  int dispDataProcessed = map(dispData,0,100,0,50);
  tft.fillRect(width_per100(4), height_per75(5), width_per100(4), height_per75(50), TFT_BLACK);//clear
  tft.fillRect(width_per100(4), height_per75(5+50-dispDataProcessed), width_per100(4), height_per75(dispDataProcessed), TFT_RED);//fill
  tft.setCursor(5, 190);
  tft.setTextColor(TFT_BLACK, TFT_RED);
  tft.print(prevDispData);
  tft.setCursor(5, 190);
  tft.setTextColor(TFT_WHITE, TFT_RED);
  tft.print(dispData);
  prevDispData = dispData;
}
void displayGain(volatile int argGain){
  volatile int dispGain = argGain;//((argGain) * 100) / 4095;
  static int prevDispGain = 0;
  if(dispGain == prevDispGain) return;
  int dispDataProcessed = map(dispGain,0,255,0,50);
  tft.fillRect(width_per100(94), height_per75(5), width_per100(4), height_per75(50), TFT_BLACK);//clear
  tft.fillRect(width_per100(94), height_per75(5+50-dispDataProcessed), width_per100(4), height_per75(dispDataProcessed), TFT_RED);//fill
//  tft.setCursor(width_per100(90), 190);
//  tft.setTextColor(TFT_BLACK, TFT_RED);
//  tft.print(prevDispGain);
//  tft.drawFloat(prevDispGain,2,width_per100(90), 190);
  tft.setCursor(width_per100(90), 190);
  tft.setTextColor(TFT_WHITE, TFT_RED);
//  tft.print(dispGain);
  tft.drawFloat((dispGain/100.0),1,width_per100(87), 177);
  prevDispGain = dispGain;
}

int getDispEdgeData(volatile int* edgeData){
  static int dispDataBuffer[SAMPLE_COUNT];
  int sum = 0, i = 0;
  for(i = 0; i < SAMPLE_COUNT-1; i++){
    dispDataBuffer[i] = dispDataBuffer[i+1];
    sum += dispDataBuffer[i];
  }
  dispDataBuffer[i] = (*edgeData);
  sum += dispDataBuffer[i];
  int moving_avg = sum /SAMPLE_COUNT;
  return moving_avg;
}
int getDispFeedbackData(volatile int* feedbackData){
  static int dispDataBuffer[SAMPLE_COUNT];
  int sum = 0, i = 0;
  for(i = 0; i < SAMPLE_COUNT-1; i++){
    dispDataBuffer[i] = dispDataBuffer[i+1];
    sum += dispDataBuffer[i];
  }
  dispDataBuffer[i] = (*feedbackData);
  sum += dispDataBuffer[i];
  int moving_avg = sum /SAMPLE_COUNT;
  return moving_avg;
}
