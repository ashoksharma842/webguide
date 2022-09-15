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
ButtonWidget* btn[] = {&btnL , &btnR, &btnA, &btnM, &btnC, &btnS};
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

void btnR_pressAction(void)
{
  if (btnR.justPressed()) {
    btnR.drawSmoothButton(!btnR.getState(), 3, TFT_BLACK, btnR.getState() ? "OFF" : "ON");
    Serial.print("Button toggled: ");
    if (btnR.getState()) Serial.println("ON");
    else  Serial.println("OFF");
    btnR.setPressTime(millis());
  }

  // if button pressed for more than 1 sec...
  if (millis() - btnR.getPressTime() >= 1000) {
    Serial.println("Stop pressing my buttton.......");
  }
  else Serial.println("Right button is being pressed");
}

void btnR_releaseAction(void)
{
  // Not action
}
#if 1
void initButtons() {
  uint16_t x = 10;
  uint16_t y = 190;
  btn[0]->initButtonUL(x, y, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_YELLOW, TFT_BLACK, "<" , 1);
  btn[1]->initButtonUL(x+50, y, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_RED, TFT_BLACK, "A" , 1);
  btn[2]->initButtonUL(x+100, y, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_RED, TFT_BLACK, "M" , 1);
  btn[3]->initButtonUL(x+150, y, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_RED, TFT_BLACK, "C" , 1);
  btn[4]->initButtonUL(x+200, y, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_BLUE, TFT_BLACK, "S" , 1);
  btn[5]->initButtonUL(x+250, y, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_YELLOW, TFT_BLACK, ">" , 1);

  for(i = 0; i < 6; i++, x += 50){
    btn[i]->setPressAction(btn_pressAction);
    btn[i]->setReleaseAction(btn_releaseAction);
    btn[i]->drawSmoothButton(false, 3, TFT_BLACK);
  }
  tft.setTextSize(3);
  tft.setTextColor(TFT_YELLOW,TFT_BLACK,true);
  tft.drawFloat(12.3,1,60,80);
  tft.fillRect(10,170,300,10,TFT_YELLOW);
}
#else
void initButtons() {
  uint16_t x = (tft.width() - BUTTON_W) / 2;
  uint16_t y = tft.height() / 2 - BUTTON_H - 10;
  btnL.initButtonUL(x, y, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_RED, TFT_BLACK, "Button", 1);
  btnL.setPressAction(btnL_pressAction);
  btnL.setReleaseAction(btnL_releaseAction);
  btnL.drawSmoothButton(false, 3, TFT_BLACK); // 3 is outline width, TFT_BLACK is the surrounding background colour for anti-aliasing

  y = tft.height() / 2 + 10;
  btnR.initButtonUL(x, y, BUTTON_W, BUTTON_H, TFT_WHITE, TFT_BLACK, TFT_GREEN, "OFF", 1);
  btnR.setPressAction(btnR_pressAction);
  //btnR.setReleaseAction(btnR_releaseAction);
  btnR.drawSmoothButton(false, 3, TFT_BLACK); // 3 is outline width, TFT_BLACK is the surrounding background colour for anti-aliasing
}
#endif
void setup() {
  Serial.begin(115200);
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  tft.setFreeFont(FF18);

  // Calibrate the touch screen and retrieve the scaling factors
  touch_calibrate();
  initButtons();
}

void loop() {
  static uint32_t scanTime = millis();
  uint16_t t_x = 9999, t_y = 9999; // To store the touch coordinates

  // Scan keys every 50ms at most
  if (millis() - scanTime >= 50) {
    // Pressed will be set true if there is a valid touch on the screen
    bool pressed = tft.getTouch(&t_x, &t_y);
    scanTime = millis();
    for (b = 0; b < buttonCount; b++) {
      if (pressed) {
//        Serial.print("x = ");Serial.print(t_x);
//        Serial.print(" y = ");Serial.println(t_y);
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
