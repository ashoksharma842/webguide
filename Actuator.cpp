#include "Actuator.h"
#include <iostream>
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

/***************************************************************************************
** Function name:           setDirection
** Description:             set direction of actuator
***************************************************************************************/
void Actuator::setDirection(direction_t dir)
{
  if(dir > 0){
    m_Apin = 0;
    m_Bpin = 1;
  } else if(dir < 0){
    m_Apin = 1;
    m_Bpin = 0;
  } else {
    m_Apin = 0;
    m_Bpin = 0;
  }
	m_direction = dir;
}

/***************************************************************************************
** Function name:           setSpeed
** Description:             set speed of actuator
***************************************************************************************/
void Actuator::actuatorSetSpeed(unsigned int actuatorSpeed)
{
	m_speed = actuatorSpeed;
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
        std::cout<<"->"<<std::endl;
    } else if(correction < 0){
        setDirection(BACKWARD);
        std::cout<<"<-"<<std::endl;
    } else {
        actuatorSetSpeed(0);
        setDirection(STOP);
        std::cout<<"||"<<std::endl;
    }
}
