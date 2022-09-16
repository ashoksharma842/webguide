// Button widget demo, requires SPI display with touch screen

// Requires widget library here:
// https://github.com/Bodmer/TFT_eWidget

#include <FS.h>
#include "Free_Fonts.h" // Include the header file attached to this sketch

#include <TFT_eSPI.h>              // Hardware-specific library
#include <TFT_eWidget.h>           // Widget library

TFT_eSPI tft = TFT_eSPI();         // Invoke custom library

#define CALIBRATION_FILE "/TouchCalData1"
#define REPEAT_CAL false

TaskHandle_t TaskLCD;
TaskHandle_t TaskCtrl;
ButtonWidget btnL = ButtonWidget(&tft);//LEFT
ButtonWidget btnR = ButtonWidget(&tft);//RIGHT
ButtonWidget btnA = ButtonWidget(&tft);//AUTO
ButtonWidget btnM = ButtonWidget(&tft);//MANUAL
ButtonWidget btnC = ButtonWidget(&tft);//CENTER
ButtonWidget btnS = ButtonWidget(&tft);//SETTING
ButtonWidget btnS1 = ButtonWidget(&tft);//SENSOR1
ButtonWidget btnS2 = ButtonWidget(&tft);//SENSOR2

#define BUTTON_W 50
#define BUTTON_H 50

// Create an array of button instances to use in for() loops
// This is more useful where large numbers of buttons are employed
ButtonWidget* btn[] = {&btnL , &btnR, &btnA, &btnM, &btnC, &btnS, &btnS1, &btnS2};
uint8_t buttonCount = sizeof(btn) / sizeof(btn[0]);
uint8_t i = 1;uint8_t b = 2;
void btn_pressAction(void)
{
  if (btn[b]->justPressed()) {
    Serial.print(b);
    Serial.println(" button just pressed");
    btn[b]->drawSmoothButton(true);
  }
}

void btn_releaseAction(void)
{
  static uint32_t waitTime = 1000;
  if (btn[b]->justReleased()) {
    Serial.println("Left button just released");
    btn[b]->drawSmoothButton(false);
    btn[b]->setReleaseTime(millis());
    waitTime = 10000;
  }
  else {
    if (millis() - btn[b]->getReleaseTime() >= waitTime) {
      waitTime = 1000;
      btn[b]->setReleaseTime(millis());
//      btn[b]->drawSmoothButton(!btnL.getState());
    }
  }
}

void initButtons() {
  uint16_t x = 10;
  uint16_t y = 190;
  btn[0]->initButtonUL(x, y, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_YELLOW, TFT_BLACK, "<" , 1);
  btn[1]->initButtonUL(x+50, y, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_RED, TFT_BLACK, "A" , 1);
  btn[2]->initButtonUL(x+100, y, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_RED, TFT_BLACK, "M" , 1);
  btn[3]->initButtonUL(x+150, y, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_RED, TFT_BLACK, "C" , 1);
  btn[4]->initButtonUL(x+200, y, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_BLUE, TFT_BLACK, "S" , 1);
  btn[5]->initButtonUL(x+250, y, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_YELLOW, TFT_BLACK, ">" , 1);
  btn[6]->initButtonUL(x+40, y-100, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_BLUE, TFT_BLACK, "S1" , 1);
  btn[7]->initButtonUL(x+210, y-100, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_BLUE, TFT_BLACK, "S2" , 1);

  for(i = 0; i < 8; i++){
    btn[i]->setPressAction(btn_pressAction);
    btn[i]->setReleaseAction(btn_releaseAction);
    btn[i]->drawSmoothButton(false, 3, TFT_BLACK);
  }
  tft.setTextSize(3);
  tft.setTextColor(TFT_YELLOW,TFT_BLACK,true);
  tft.setFreeFont(FF32);
  tft.drawNumber(50,110,80);
  tft.fillRect(10,170,300,10,TFT_YELLOW);
}

void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(FF18);
  // Calibrate the touch screen and retrieve the scaling factors
  touch_calibrate();
  initButtons();

  xTaskCreatePinnedToCore(TaskLCDcode, "TaskLCD", 10000, NULL, 1, &TaskLCD, 0);
  delay(500);
  xTaskCreatePinnedToCore(TaskCtrlcode, "TaskCtrl", 10000, NULL, 1, &TaskCtrl, 0);
  delay(500);
}
void TaskCtrlcode( void * pvParameters ){
  Serial.print("TaskCtrl running on core ");
  Serial.println(xPortGetCoreID());
  while(1){
    Serial.print(".");
    delay(500);
  }
}
void TaskLCDcode( void * pvParameters ){
  Serial.print("TaskLCD running on core ");
  Serial.println(xPortGetCoreID());
  while(1){
    static uint32_t scanTime = millis();
    uint16_t t_x = 9999, t_y = 9999; // To store the touch coordinates

    // Scan keys every 50ms at most
    if (millis() - scanTime >= 50) {
      // Pressed will be set true if there is a valid touch on the screen
      bool pressed = tft.getTouch(&t_x, &t_y);
      scanTime = millis();
      for (b = 0; b < buttonCount; b++) {
        if (pressed) {
          if (btn[b]->contains(t_x, t_y)) {
            Serial.print(b);
            Serial.println(" button pressed");
            btn[b]->press(true);
            btn[b]->pressAction();
          }
        }
        else {
          btn[b]->press(false);
          btn[b]->releaseAction();
        }
      }
    }
  }
}

void touch_calibrate()
{
  uint16_t calData[5];
  uint8_t calDataOK = 0;

  // check file system exists
  if (!LittleFS.begin()) {
    Serial.println("Formating file system");
    LittleFS.format();
    LittleFS.begin();
  }

  // check if calibration file exists and size is correct
  if (LittleFS.exists(CALIBRATION_FILE)) {
    if (REPEAT_CAL)
    {
      // Delete if we want to re-calibrate
      LittleFS.remove(CALIBRATION_FILE);
    }
    else
    {
      File f = LittleFS.open(CALIBRATION_FILE, "r");
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
    File f = LittleFS.open(CALIBRATION_FILE, "w");
    if (f) {
      f.write((const unsigned char *)calData, 14);
      f.close();
    }
  }
}
void loop() {
//all code is run in tasks
}
