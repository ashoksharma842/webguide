/**
* Module : controller.cpp
* Project : Web Guide Controller
* Originated : 2021.11.12
*
* Overview : definations required for controlling of web guide.
*
*/

#include "controller.h"
#include <Arduino.h>

void DisplayInfo(void) {
  uint32_t Freq = 0;
  Serial.println("version : 0.0.1");
  Freq = getCpuFrequencyMhz();
  Serial.print("CPU Freq = ");
  Serial.print(Freq);
  Serial.println(" MHz");
  Freq = getXtalFrequencyMhz();
  Freq = getApbFrequency();
  Serial.print("APB Freq = ");
  Serial.print(Freq);
  Serial.println(" Hz");  
}

void read_adc_data(int *channels, volatile int *data)
{
  for(int i = 0; i< ADC_CHANNELS; i++){
    data[i] = analogRead(channels[i]);
  }
}

int CalculateCorrection (void)
{
  volatile int required_correction;
  /*Serial.print("referenceData = ");
  Serial.print(**referenceData);
  Serial.print(" - ");
  Serial.print(**sensorData);
  Serial.print(" = ");
 */
  required_correction = (**referenceData) - (**sensorData);
//  Serial.print(required_correction);
  required_correction *= (gain * 2);
  required_correction = map(required_correction,0,4095,0,100);//return value from 0-100
//  Serial.print(" required_correction = ");Serial.println(required_correction);
  return required_correction;
}

void MoveActuator(int direction, int dutyCycle)
{
  dutyCycle = abs((int8_t)dutyCycle);
  uint8_t u8dutyCycle = (uint8_t)dutyCycle;
  if(u8dutyCycle > 100){
  u8dutyCycle = 100;
  }
  /* PINS and PWM are inverted, because they're driven via BJT
     Hence by default it'll be HIGH and on making pin HIGH, output goes LOW*/
  if(direction == STOP){
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, HIGH);
  } else if (direction == FORWARD){
    digitalWrite(motor1Pin1, HIGH);
    digitalWrite(motor1Pin2, LOW);
  } else if (direction == BACKWARD){
    digitalWrite(motor1Pin1, LOW);
    digitalWrite(motor1Pin2, HIGH);
  }
  u8dutyCycle = map(u8dutyCycle,0,100,0,255);//100% to 255 counts
  ledcWrite(pwmChannel, (255 - u8dutyCycle));
}

int ApplyCorrection(int requiredCorrection)
{
  if(requiredCorrection == 0){
    MoveActuator(STOP,0);
    return STOP;
  } else if(requiredCorrection > 0){
    MoveActuator(FORWARD, requiredCorrection);
    return FORWARD;
  } else if(requiredCorrection < 0){
    MoveActuator(BACKWARD, requiredCorrection);
    return BACKWARD;
  } else {
    return STOP;//invalid condition, to remove warning
  }
}
