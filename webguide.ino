/**
* Module : webGuideController.ino
* Project : Web Guide Controller
* Originated : 2021.11.12
*
* Overview : uses esp32 and L298N for web controlling
*
*/

#include "controller.h"
#include "lcd.h"
#include "FS.h"

#include <SPI.h>

#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

#define NUM_KEYS 8

TFT_eSPI_Button key[NUM_KEYS];

TaskHandle_t Control;
TaskHandle_t Display;

const int AUTObuttonPin = 0;
const int MANUALbuttonPin = 16;
const int SCbuttonPin = 17;
const int Sensor1buttonPin = 12;
const int Sensor2buttonPin = 22;
const int feedbackTypePin = 13;

const int sensor1Pin = 32;
const int sensor2Pin = 33;
const int feedbackPin = 34;
const int namurPin = 35;
const int currentSensePin = 25;

const int motor1Pin1 = 27; 
const int motor1Pin2 = 26; 
const int enable1Pin = 14;

volatile uint8_t operatingMode = MANUAL;
volatile uint8_t guidingMode = SENSOR1;
volatile uint8_t feedBackType = NAMUR;
volatile uint8_t gain = 1;

// Setting PWM properties
const int freq = 20000;
const int pwmChannel = 0;
const int resolution = 8;

//#define ADC_CHANNELS  5
int adcChannelNumbers[ADC_CHANNELS] = {currentSensePin, sensor1Pin, sensor2Pin, namurPin, feedbackPin};
volatile int adcData[ADC_CHANNELS] = {00, 00, 00, 00, 00};
int refData[ADC_CHANNELS] = {ADC_MIDVAL, ADC_MIDVAL, ADC_MIDVAL, ADC_MIDVAL, ADC_MIDVAL};

volatile int* edgeData;
volatile int* feedbackData;
volatile int* refEdgeData;
volatile int* refFeedbackData;
volatile int** sensorData;
volatile int** referenceData;

int prevEdgeData, prevFeedbackData, prevCurrentData;

int requiredCorrection = 0;
int actuatorStatus = STOP;

int32_t height, width;



void setup() {
  Serial.begin(115200);
  
    tft.init();

  // Set the rotation before we calibrate
  tft.setRotation(1);

  // call screen calibration
  touch_calibrate();

  // Clear screen
  tft.fillScreen(TFT_BLACK);

  tft.setFreeFont(&FreeMono9pt7b);
  height = tft.getViewportHeight();
  width = tft.getViewportWidth();
  drawButtons(key);
  tft.fillRect(width_per100(4), height_per75(5), width_per100(4), height_per75(50), TFT_RED);//left
  tft.fillRect(width_per100(94), height_per75(5), width_per100(4), height_per75(50), TFT_RED);//right
//  tft.fillRect(width_per100(21), height_per75(36), width_per100(60), height_per75(2), TFT_RED);//center
  tft.fillRect(width_per100(26), height_per75(61), width_per100(50), height_per75(4), TFT_RED);//bottom
  displaySensor(SENSOR1, TFT_CYAN);
  displaySensor(SENSOR2, TFT_SILVER);
  
  // Setting button properties
  pinMode(AUTObuttonPin, INPUT_PULLUP);
  pinMode(MANUALbuttonPin, INPUT_PULLUP);
  pinMode(SCbuttonPin, INPUT_PULLUP);
  pinMode(Sensor1buttonPin, INPUT_PULLUP);
  pinMode(Sensor2buttonPin, INPUT_PULLUP);
  pinMode(feedbackTypePin, INPUT_PULLUP);

  DisplayInfo();
  //set default guiding mode and operating mode values
  sensorData = &edgeData;
  referenceData = &refEdgeData;
  edgeData = &adcData[1];//sensor1
  refEdgeData = &refData[1];//sensor1
  feedbackData = &adcData[4];//feedback
  refFeedbackData = &refData[4];//feedback
  // sets the pins as outputs:
  pinMode(motor1Pin1, OUTPUT);
  pinMode(motor1Pin2, OUTPUT);
  pinMode(enable1Pin, OUTPUT);
  // configure LED PWM functionalitites
  ledcSetup(pwmChannel, freq, resolution);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(enable1Pin, pwmChannel);

  tft.setCursor(125, 235);
  tft.setTextColor(TFT_WHITE, TFT_RED);
  tft.println("NO ERROR");
  
  //create a task that will be executed in the ControlTask() function, with priority 1 and executed on core 0
  xTaskCreatePinnedToCore(ControlTask, "Control", 10000, NULL, 1, &Control, 0);
  delay(500);

  //create a task that will be executed in the DisplayTask() function, with priority 1 and executed on core 1
  xTaskCreatePinnedToCore(DisplayTask, "Display", 10000, NULL, 1, &Display, 1);
  delay(500);
}

//ControlTask: move actuator as per adc
void ControlTask( void * pvParameters ){
  Serial.print("Control running on core ");
  Serial.println(xPortGetCoreID());
//  int requiredCorrection = 0;
  for(;;){
    read_adc_data(adcChannelNumbers, adcData);
    /*Serial.print("[");
    for(int i = 0; i < 5; i++){
      Serial.print(adcData[i]);Serial.print(" ");
    }
    Serial.println("]");*/
//    Serial.println(**sensorData);
    if((operatingMode != MANUAL) && (**sensorData != **referenceData)){
      requiredCorrection = CalculateCorrection();
    } else if (actuatorStatus != STOP){
//      MoveActuator(STOP,0);
        ApplyCorrection(0);
    }
    if(operatingMode != MANUAL){
      actuatorStatus = ApplyCorrection(requiredCorrection);
    }
    delay(10);
//    yield();
  }
}

void updateOperatingMode(volatile uint8_t operatingModeArg) {
  switch (operatingModeArg){
    case AUTO_BUTTON :
    key[AUTO_BUTTON].setFillcolor(TFT_YELLOW);
    key[AUTO_BUTTON].drawButton(false, "A");
    key[MANUAL_BUTTON].setFillcolor(TFT_CYAN);
    key[MANUAL_BUTTON].drawButton(false, "M");
    key[SC_BUTTON].setFillcolor(TFT_CYAN);
    key[SC_BUTTON].drawButton(false, "C");
    if(operatingMode != AUTO){
      operatingMode = AUTO;
      sensorData = &edgeData;
      referenceData = &refEdgeData;
      Serial.println("<AUTO>");
    }
  break;
    case MANUAL_BUTTON :
    key[AUTO_BUTTON].setFillcolor(TFT_CYAN);
    key[AUTO_BUTTON].drawButton(false, "A");
    key[MANUAL_BUTTON].setFillcolor(TFT_YELLOW);
    key[MANUAL_BUTTON].drawButton(false, "M");
    key[SC_BUTTON].setFillcolor(TFT_CYAN);
    key[SC_BUTTON].drawButton(false, "C");
    if(operatingMode != MANUAL){
      operatingMode = MANUAL;
      Serial.println("<MANUAL>");
    }
  break;
    case SC_BUTTON :
    key[AUTO_BUTTON].setFillcolor(TFT_CYAN);
    key[AUTO_BUTTON].drawButton(false, "A");
    key[MANUAL_BUTTON].setFillcolor(TFT_CYAN);
    key[MANUAL_BUTTON].drawButton(false, "M");
    key[SC_BUTTON].setFillcolor(TFT_YELLOW);
    key[SC_BUTTON].drawButton(false, "C");
    if(operatingMode != SC){
      operatingMode = SC;
      sensorData = &feedbackData;
      referenceData = &refFeedbackData;
      Serial.println("<SC>");
    }
  break;
  }
}
//DisplayTask: display data and get input
void DisplayTask( void * pvParameters ){
  Serial.print("Display running on core ");
  Serial.println(xPortGetCoreID());
  int prevRequiredCorrection = 88;
  int i = 0;
  for(;;){
    if(!digitalRead(AUTObuttonPin)){
      Serial.println("AUTO BUTTON PRESSED");
      updateOperatingMode(AUTO_BUTTON);
    }
    if(!digitalRead(MANUALbuttonPin)){
      Serial.println("MANUAL BUTTON PRESSED");
      updateOperatingMode(MANUAL_BUTTON);
    }
    if(!digitalRead(SCbuttonPin)){
      Serial.println("SC BUTTON PRESSED");
      updateOperatingMode(SC_BUTTON);
    }
//-----------read LCD---------------
  uint16_t t_x = 0, t_y = 0; // To store the touch coordinates

  // Get current touch state and coordinates
  bool pressed = tft.getTouch(&t_x, &t_y);

  // Adjust press state of each key appropriately
  for (uint8_t b = 0; b < NUM_KEYS; b++) {
    if (pressed && key[b].contains(t_x, t_y)) 
      key[b].press(true);  // tell the button it is pressed
    else
      key[b].press(false);  // tell the button it is NOT pressed
  }

  // Check if any key has changed state
  for (uint8_t b = 0; b < NUM_KEYS; b++) {
    // If button was just pressed, redraw inverted button
    if (key[b].justPressed()) {
      // Serial.println("Button " + (String)b + " pressed");
      // key[b].drawButton(true, (String)(b));
      switch(b){
        case LEFT_BUTTON :
        key[LEFT_BUTTON].setFillcolor(TFT_YELLOW);
        key[LEFT_BUTTON].drawButton(false, "<");
        if(operatingMode == MANUAL){
          ApplyCorrection(-80);
        }
      break;
        case RIGHT_BUTTON :
        key[RIGHT_BUTTON].setFillcolor(TFT_YELLOW);
        key[RIGHT_BUTTON].drawButton(false, ">");
        if(operatingMode == MANUAL){
          ApplyCorrection(80);
        }
      break;
      }
    }

    // If button was just released, redraw normal color button
    if (key[b].justReleased()) {
      // Serial.println("Button " + (String)b + " released");
      // key[b].drawButton(false, (String)(b));
      switch(b){
        case SENSOR1_BUTTON :
        displaySensor(SENSOR1, TFT_CYAN);
        displaySensor(SENSOR2, TFT_SILVER);
        if((guidingMode != GM_SENSOR1) && (operatingMode != SC)){
          guidingMode = GM_SENSOR1;
          edgeData = &adcData[1];
          refEdgeData =&refData[1];
          Serial.print("guidingMode : ");Serial.println(guidingMode);
        }
      break;
        case SENSOR2_BUTTON :
        displaySensor(SENSOR2, TFT_CYAN);
        displaySensor(SENSOR1, TFT_SILVER);
        if((guidingMode != GM_SENSOR2) && (operatingMode != SC)){
          guidingMode = GM_SENSOR2;
          edgeData = &adcData[2];
          refEdgeData =&refData[2];
          Serial.print("guidingMode : ");Serial.println(guidingMode);
        }
      break;
        case AUTO_BUTTON :
        case MANUAL_BUTTON :
        case SC_BUTTON :
        updateOperatingMode(b);
      break;
        case FB_BUTTON :
        // key[FB_BUTTON].drawButton(false, "F/N");
        if((guidingMode != GM_NAMUR) && (operatingMode == SC)){
          guidingMode = GM_NAMUR;
          edgeData = &adcData[3];
          refEdgeData =&refData[3];
          Serial.println("GM_NAMUR");
          key[FB_BUTTON].setFillcolor(TFT_GREENYELLOW);
          key[FB_BUTTON].drawButton(false, "NMR");
        } else if ((guidingMode != GM_ACT_FB) && (operatingMode == SC)){
          guidingMode = GM_ACT_FB;
          edgeData = &adcData[4];
          refEdgeData =&refData[4];
          Serial.println("GM_ACT_FB");
          key[FB_BUTTON].setFillcolor(TFT_GREEN);
          key[FB_BUTTON].drawButton(false, "FB");
        }
      break;
        case LEFT_BUTTON :
        key[LEFT_BUTTON].setFillcolor(TFT_CYAN);
        key[LEFT_BUTTON].drawButton(false, "<");
        if(operatingMode == MANUAL){
          ApplyCorrection(0);
        }
      break;
        case RIGHT_BUTTON :
        key[RIGHT_BUTTON].setFillcolor(TFT_CYAN);
        key[RIGHT_BUTTON].drawButton(false, ">");
        if(operatingMode == MANUAL){
          ApplyCorrection(0);
        }
      break;
      }
    }
  }
  static int dispEdgeData;
  dispEdgeData = getDispEdgeData(edgeData);
  if(prevEdgeData != dispEdgeData){
    displaySensorData(guidingMode, dispEdgeData);
    prevEdgeData = dispEdgeData;
  }
  static int dispFeedbackData;
  dispFeedbackData = getDispFeedbackData(feedbackData);
  if(prevFeedbackData != dispFeedbackData){
    displayFeedbackData(feedBackType, dispFeedbackData);
    prevFeedbackData = dispFeedbackData;
  }
//  Serial.println(adcData[0]);
  if(prevCurrentData != adcData[0]){
    displayCurrentData(adcData[0]);
    prevCurrentData = adcData[0];
  }
  if((operatingMode == AUTO) && (prevRequiredCorrection != requiredCorrection)){
//    Serial.print(**sensorData);Serial.print(" -> ");Serial.println(requiredCorrection);
    prevRequiredCorrection = requiredCorrection;
  }
  delay(10);
//  yield();
  }
}

void loop() {

}
