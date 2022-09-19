#include "Actuator.h"
#include <Arduino.h>
/***************************************************************************************
** Function name:           Actuator
** Description:             Constructor , we must stop actuator in begning
***************************************************************************************/
Actuator::Actuator(int A_pin, int B_pin, int pwm_pin, bool fb)	
{
  m_Apin = A_pin;
  m_Bpin = B_pin;
	m_pwmPin = pwm_pin;
	m_feedback = fb;
}
void Actuator::actuatorInit()
{
  pinMode(m_Apin, OUTPUT);
  pinMode(m_Bpin, OUTPUT);
  pinMode(m_pwmPin, OUTPUT);
  ledcSetup(m_pwmChannel, m_freq, m_resolution);
  ledcAttachPin(m_pwmPin, m_pwmChannel);
}
/***************************************************************************************
** Function name:           setDirection
** Description:             set direction of actuator
***************************************************************************************/
void Actuator::setDirection(direction_t dir)
{
  if(dir > 0){
    digitalWrite(m_Apin, HIGH);
    digitalWrite(m_Bpin, LOW);
  } else if(dir < 0){
    digitalWrite(m_Apin, LOW);
    digitalWrite(m_Bpin, HIGH);
  } else {
    digitalWrite(m_Apin, LOW);
    digitalWrite(m_Bpin, LOW);
  }
	m_direction = dir;
}

/***************************************************************************************
** Function name:           setSpeed
** Description:             set speed of actuator, convert 100% to 255
***************************************************************************************/
void Actuator::actuatorSetSpeed(unsigned int actuatorSpeed)
{
	m_speed = map(actuatorSpeed,0,100,0,255);
  ledcWrite(m_pwmChannel, m_speed);
}

/***************************************************************************************
** Function name:           getDirection
** Description:             get direction of actuator
***************************************************************************************/
direction_t Actuator::getDirection()
{
	return m_direction;
}

/***************************************************************************************
** Function name:           setFbType
** Description:             with feedback or without feedback
***************************************************************************************/
void Actuator::setFbType(bool type)
{
	m_feedback = type;
}

/***************************************************************************************
** Function name:           getFbData
** Description:             adc data of actuator feedback
***************************************************************************************/
int Actuator::getFbData()
{
	return m_feedbackData;
}

/***************************************************************************************
** Function name:           move
** Description:             move actuator
***************************************************************************************/
void Actuator::actuatorMove(int correction)
{
    actuatorSetSpeed((correction < 0)?(correction * (-1)):correction);
    
    if(correction > 0){
        setDirection(FORWARD);
    } else if(correction < 0){
        setDirection(BACKWARD);
    } else {
        actuatorSetSpeed(0);
        setDirection(STOP);
    }
}
