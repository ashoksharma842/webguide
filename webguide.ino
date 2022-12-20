/*
 * Web guide controller
 */

#include <FS.h>
#include "Free_Fonts.h" // Include the header file attached to this sketch
#include <TFT_eSPI.h>
#include <TFT_eWidget.h>
#include "Controller.h"
#include "Sensor.h"
#include "Actuator.h"
#include "calibration.h"
TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sensorBar  = TFT_eSprite(&tft);
TFT_eSprite currentBar = TFT_eSprite(&tft);
TFT_eSprite gainBar    = TFT_eSprite(&tft);
TFT_eSprite mainDisp   = TFT_eSprite(&tft);
Controller controller = Controller();
Sensor sensor1 = Sensor(2048,32); // 50 gp, adc pin 32
Sensor sensor2 = Sensor(2048,33); // 50 gp, adc pin 33
Sensor current = Sensor(50,25); // current sensor.
Sensor limitSwitch = Sensor(50,35); // for servo centre NAMUR sensor
Sensor *activeSensor = &sensor1; // used for guiding mode
Actuator actuator = Actuator();

enum {bLeft, bAuto, bManual, bCenter, bSetup, bRight, bS1, bS2};
#define TEXT_SIZE 3

TaskHandle_t TaskLCD;
TaskHandle_t TaskCtrl;
ButtonWidget btnL  = ButtonWidget(&tft);//LEFT
ButtonWidget btnA  = ButtonWidget(&tft);//AUTO
ButtonWidget btnM  = ButtonWidget(&tft);//MANUAL
ButtonWidget btnC  = ButtonWidget(&tft);//CENTER
ButtonWidget btnS  = ButtonWidget(&tft);//SETTING
ButtonWidget btnR  = ButtonWidget(&tft);//RIGHT
ButtonWidget btnS1 = ButtonWidget(&tft);//SENSOR1
ButtonWidget btnS2 = ButtonWidget(&tft);//SENSOR2

#define BUTTON_W 50
#define BUTTON_H 50
//static uint32_t setupTime = millis();
bool autoSetup = false, currentSetup = false;
ButtonWidget* btn[] = {&btnL, &btnA, &btnM, &btnC, &btnS, &btnR, &btnS1, &btnS2};
uint8_t buttonCount = sizeof(btn) / sizeof(btn[0]);
uint8_t b = 2;//for counting buttons
int prevDispData = 1, dispData = 0, countForDataDisp = 0;
static uint32_t waitTime = 1000;

//call back functions 
void incGP(void){
	activeSensor->setGuidePoint(activeSensor->getGuidePoint()+30);
	Serial.print("GP: ");
	Serial.println(activeSensor->getGuidePoint());
}
void decGP(void){
	activeSensor->setGuidePoint(activeSensor->getGuidePoint()-30);
	Serial.print("GP: ");
	Serial.println(activeSensor->getGuidePoint());
}
void incGain(void){
	activeSensor->setGain(activeSensor->getGain()+1);
	Serial.print("Gain: ");
	Serial.println(activeSensor->getGain());
}
void decGain(void){
	activeSensor->setGain(activeSensor->getGain()-1);
	Serial.print("Gain: ");
	Serial.println(activeSensor->getGain());
}
void incCurrent(void){
	actuator.setCurrent(actuator.getCurrent()+5);
	Serial.print("Current: ");
	Serial.println(actuator.getCurrent());
}
void decCurrent(void){
	actuator.setCurrent(actuator.getCurrent()-5);
	Serial.print("Current: ");
	Serial.println(actuator.getCurrent());
}
void moveActuatorLeft(void){
	actuator.actuatorMove(80);
}
void moveActuatorRight(void){
	actuator.actuatorMove(-80);
}
void AutoPressed(void){
	this->drawSmoothButton(false);
	controller.setOperatingMode(AUTO);
	controller.getGuidingMode() == S1 ? activeSensor = &sensor1 : activeSensor = &sensor2;
	//set right left arrow to gain 
	if(autoSetup){
		btn[bLeft]->setPressAction(incGain);
		btn[bRight]->setPressAction(decGain);
	} else {
		btn[bLeft]->setPressAction(incGP);
		btn[bRight]->setPressAction(decGP);		
	}
	
}
void ManualPressed(void){
	this->drawSmoothButton(false);
	controller.setOperatingMode(MANUAL);
	//set right left arrow to move actuator 
	btn[bLeft]->setPressAction(moveActuatorLeft);
	btn[bRight]->setPressAction(moveActuatorRight);
}
void SCPressed(void){
	this->drawSmoothButton(false);
	controller.setOperatingMode(SC);
	activeSensor = &limitSwitch;
	//set right left arrow to gain
}
void SetupPressed(void){
	this->drawSmoothButton(false);
	if(controller.getOperatingMode() == AUTO){
		if(autoSetup == false) {
			autoSetup = true;
		} else {
			autoSetup = false;
		}
	} else if(controller.getOperatingMode() == MANUAL){
		if(currentSetup == false) {
			currentSetup = true;
		} else {
			currentSetup = false;
		}
	}
	//set right left arrow to gain
}
void S1Pressed(void){
	this->drawSmoothButton(false);
	controller.setGuidingMode(S1);
	if(controller.getOperatingMode()==AUTO)
	{
		activeSensor = &sensor1;
	}
}
void S2Pressed(void){
	this->drawSmoothButton(false);
	controller.setGuidingMode(S2);
	if(controller.getOperatingMode()==AUTO)
	{
		activeSensor = &sensor2;
	}
}
	
void btn_releaseAction(void){
	waitTime = 1000;
	if (this->justReleased()) {
		this->drawSmoothButton(true);
		this->setReleaseTime(millis());
		waitTime = 10000;
	}else{
		if (millis() - this->getReleaseTime() >= waitTime) {
			waitTime = 1000;
			this->setReleaseTime(millis());
			this->drawSmoothButton(!this->getState());
		}
    }
}
void Auto_releaseAction(void){
	waitTime = 1000;
	if (this->justReleased()) {
		this->drawSmoothButton(true);
		btn[bManual]->drawSmoothButton(true);
        btn[bCenter]->drawSmoothButton(true);
		this->setReleaseTime(millis());
		waitTime = 10000;
	}else{
		if (millis() - this->getReleaseTime() >= waitTime) {
			waitTime = 1000;
			this->setReleaseTime(millis());
			this->drawSmoothButton(!this->getState());
		}
    }
}
void Manual_releaseAction(void){
	waitTime = 1000;
	if (this->justReleased()) {
		this->drawSmoothButton(true);
		btn[bAuto]->drawSmoothButton(true);
        btn[bCenter]->drawSmoothButton(true);
		this->setReleaseTime(millis());
		waitTime = 10000;
	}else{
		if (millis() - this->getReleaseTime() >= waitTime) {
			waitTime = 1000;
			this->setReleaseTime(millis());
			this->drawSmoothButton(!this->getState());
		}
    }
}
void Center_releaseAction(void){
	waitTime = 1000;
	if (this->justReleased()) {
		this->drawSmoothButton(true);
		btn[bAuto]->drawSmoothButton(true);
        btn[bManual]->drawSmoothButton(true);
		this->setReleaseTime(millis());
		waitTime = 10000;
	}else{
		if (millis() - this->getReleaseTime() >= waitTime) {
			waitTime = 1000;
			this->setReleaseTime(millis());
			this->drawSmoothButton(!this->getState());
		}
    }
}
void Setup_releaseAction(void){
	waitTime = 1000;
	if (this->justReleased()) {
		this->drawSmoothButton(true);
		if((autoSetup == true)||(currentSetup == true)) {
			btn[bSetup]->drawSmoothButton(false);
			tft.setTextColor(TFT_WHITE);
			if(controller.getOperatingMode() == AUTO){
				tft.drawString("set gain",90,50);
			} else if(controller.getOperatingMode() == MANUAL){
				tft.drawString("set crnt",90,50);
			}
			Serial.println("*");
        } else {
			btn[bSetup]->drawSmoothButton(true);
			tft.setTextColor(TFT_BLACK);
			if(controller.getOperatingMode() == AUTO){
				tft.drawString("set gain",90,50);
			} else if(controller.getOperatingMode() == MANUAL){
				tft.drawString("set crnt",90,50);
			}
			Serial.println("!");
        }
		this->setReleaseTime(millis());
		waitTime = 10000;
	}else{
		if (millis() - this->getReleaseTime() >= waitTime) {
			waitTime = 1000;
			this->setReleaseTime(millis());
			this->drawSmoothButton(!this->getState());
		}
    }
}

void initButtons(){
  uint16_t x = 10;
  uint16_t y = 190;

  btn[bLeft		]->initButtonUL(x, 		y, 		BUTTON_W, BUTTON_H, TFT_WHITE, TFT_YELLOW, 	TFT_BLACK, "<" ,  TEXT_SIZE);
  btn[bAuto		]->initButtonUL(x+50, 	y, 		BUTTON_W, BUTTON_H, TFT_WHITE, TFT_RED, 	TFT_BLACK, "A" ,  TEXT_SIZE);
  btn[bManual	]->initButtonUL(x+100, 	y, 		BUTTON_W, BUTTON_H, TFT_WHITE, TFT_RED, 	TFT_BLACK, "M" ,  TEXT_SIZE);
  btn[bCenter	]->initButtonUL(x+150, 	y, 		BUTTON_W, BUTTON_H, TFT_WHITE, TFT_RED, 	TFT_BLACK, "C" ,  TEXT_SIZE);
  btn[bSetup	]->initButtonUL(x+200, 	y, 		BUTTON_W, BUTTON_H, TFT_WHITE, TFT_RED, 	TFT_BLACK, "S" ,  TEXT_SIZE);
  btn[bRight	]->initButtonUL(x+250, 	y, 		BUTTON_W, BUTTON_H, TFT_WHITE, TFT_YELLOW, 	TFT_BLACK, ">" ,  TEXT_SIZE);
  btn[bS1		]->initButtonUL(x+20, 	y-100, 	BUTTON_W, BUTTON_H, TFT_WHITE, TFT_BLUE, 	TFT_BLACK, "S1" , TEXT_SIZE);
  btn[bS2		]->initButtonUL(x+230, 	y-100, 	BUTTON_W, BUTTON_H, TFT_WHITE, TFT_BLUE, 	TFT_BLACK, "S2" , TEXT_SIZE);

  btn[bLeft		]->setReleaseAction(decGP);
  btn[bAuto		]->setReleaseAction(AutoPressed);
  btn[bManual	]->setReleaseAction(ManualPressed);
  btn[bCenter	]->setReleaseAction(SCPressed);
  btn[bSetup	]->setReleaseAction(SetupPressed);
  btn[bRight	]->setReleaseAction(incGP);
  btn[bS1		]->setReleaseAction(S1Pressed);
  btn[bS2		]->setReleaseAction(S2Pressed);

  btn[bLeft		]->setPressAction(btn_releaseAction);
  btn[bAuto		]->setPressAction(Auto_releaseAction);
  btn[bManual	]->setPressAction(Manual_releaseAction);
  btn[bCenter	]->setPressAction(Center_releaseAction);
  btn[bSetup	]->setPressAction(SetupPressed);
  btn[bRight	]->setPressAction(btn_releaseAction);
  btn[bS1		]->setPressAction(btn_releaseAction);
  btn[bS2		]->setPressAction(btn_releaseAction);

 for(int i = 0; i < 8; i++){
    btn[i]->drawSmoothButton(true, 3, TFT_BLACK);
  }
  
  btn[bManual]->drawSmoothButton(false);
  btn[bS1]->drawSmoothButton(false);
}
void setup(){
  Serial.begin(115200);
  actuator.actuatorInit();
  tft.begin();
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);
  // Calibrate the touch screen and retrieve the scaling factors
  touch_calibrate();
  initButtons();
  sensorBar.createSprite(300, 10);
  currentBar.createSprite(10,150);
  gainBar.createSprite(10,150);
  mainDisp.createSprite(120,60);
  mainDisp.setTextColor(TFT_ORANGE,TFT_BLACK,true);
  mainDisp.setTextSize(5);
  mainDisp.drawNumber(9999,0,0);
  tft.drawRect(9,171,302,12,TFT_WHITE);

  xTaskCreatePinnedToCore(TaskLCDcode, "TaskLCD", 10000, NULL, 1, &TaskLCD, 0);
  delay(500);
  xTaskCreatePinnedToCore(TaskCtrlcode, "TaskCtrl", 10000, NULL, 1, &TaskCtrl, 1);
  delay(500);
}


void TaskCtrlcode( void * pvParameters ){
  Serial.print("TaskCtrl running on core ");
  Serial.println(xPortGetCoreID());
  eGuidingMode currentGuidingMode, prevGuidingMode;
  eOperatingMode currentOperatingMode, prevOperatingMode;
  while(1){
    currentGuidingMode = controller.getGuidingMode();
    currentOperatingMode = controller.getOperatingMode();
    if(prevGuidingMode != currentGuidingMode){
      Serial.print("Guiding mode:");
      Serial.println(currentGuidingMode);
      prevGuidingMode = currentGuidingMode;
    }
    if(prevOperatingMode != currentOperatingMode){
      Serial.print("Operting mode:");
      Serial.println(controller.getOperatingMode());
      prevOperatingMode = currentOperatingMode;
    }
    if(currentOperatingMode){
      int sensorData = activeSensor->getData();
      int guidePoint = activeSensor->getGuidePoint();
      int gain = activeSensor->getGain();
      int correction = 2 * gain * abs(sensorData - guidePoint);
      if(correction){
          actuator.actuatorMove(correction);
      }
    }
    delay(1);
  }
}


void TaskLCDcode( void * pvParameters ){
  Serial.print("TaskLCD running on core ");
  Serial.println(xPortGetCoreID());
  uint16_t sensorDispData = 0, currentDispData = 0, gainDispData = 0, gp = 0;
  while(1){
    static uint32_t scanTime = millis();
    uint16_t t_x = 9999, t_y = 9999; // To store the touch coordinates
    // Scan keys every 50ms at most
    if (millis() - scanTime >= 50) {
      countForDataDisp++;
      // Pressed will be set true if there is a valid touch on the screen
      bool pressed = tft.getTouch(&t_x, &t_y);
      scanTime = millis();
      for (b = 0; b < buttonCount; b++) {
        if (pressed) {
          if (btn[b]->contains(t_x, t_y)) {
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
	//clear main screen
    sensorBar.fillRect(0,0,(sensorDispData*300)/4095,10,TFT_BLACK);
    currentBar.fillRect(0,150-(currentDispData),10,current.getData()/2,TFT_BLACK);
    gainBar.fillRect(0,150-(gainDispData),10,gainDispData,TFT_BLACK);
    sensorBar.fillRect((gp*300)/4095,0,3,10,TFT_BLACK);
	//update variables
    sensorDispData = activeSensor->getData();
    currentDispData = (current.getData()*150)/4095;
    gainDispData = activeSensor->getGain();
    gp = activeSensor->getGuidePoint();
    //rewrite main screen
    sensorBar.fillRect(0,0,(sensorDispData*300)/4095,10,TFT_RED);
    currentBar.fillRect(0,150-(currentDispData),10,current.getData()/2,TFT_RED);
    gainBar.fillRect(0,150-(gainDispData),10,gainDispData,TFT_RED);
    sensorBar.fillRect((gp*300)/4095,0,3,10,TFT_BLUE);
	//push sprites
    sensorBar.pushSprite(10, 172);
    currentBar.pushSprite(10,0);
    gainBar.pushSprite(300,0);
	
	//display main text
    if(countForDataDisp++ > 100){
      mainDisp.fillScreen(TFT_BLACK);
//      mainDisp.setTextDatum(TR_DATUM); //TODO: right allign
      mainDisp.setTextColor(TFT_BLUE,TFT_BLACK,true);
      mainDisp.drawNumber(sensorDispData,0,0);
      countForDataDisp = 0;
    }

    mainDisp.pushSprite(100,100);
    tft.setTextSize(2);
    tft.setTextColor(TFT_YELLOW,TFT_BLACK,true);
    tft.drawNumber((current.getData()*150)/4095,5,155);
    tft.drawNumber(gainDispData,285,155);
//    if((millis() - setupTime > 3000) && (autoSetup)){
//      btn[bSetup]->drawSmoothButton(true);
//      autoSetup = false;
//      Serial.println("!");      
//    }
	delay(1);
  }
}
void loop(){
//all code is run in tasks
}
