#include "Actuator.h"
#include <iostream>
/***************************************************************************************
** Function name:           Actuator
** Description:             Constructor , we must stop actuator in begning
***************************************************************************************/
Actuator::Actuator(direction direction, int speed, bool fb)	
{
	m_direction = direction;
	m_speed = speed;
	m_feedback = fb;
}

/***************************************************************************************
** Function name:           setDirection
** Description:             set direction of actuator
***************************************************************************************/
void Actuator::setDirection(direction dir)
{
	m_direction = dir;
}

/***************************************************************************************
** Function name:           setSpeed
** Description:             set speed of actuator
***************************************************************************************/
void Actuator::setSpeed(unsigned int speed)
{
	m_speed = speed;
}

/***************************************************************************************
** Function name:           getDirection
** Description:             get direction of actuator
***************************************************************************************/
direction Actuator::getDirection()
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
void Actuator::move(int correction)
{
    setSpeed((correction < 0)?(correction * (-1)):correction);
    if(correction > 0){
        setDirection(FORWARD);
        std::cout<<"->"<<std::endl;
    } else if(correction < 0){
        setDirection(BACKWARD);
        std::cout<<"<-"<<std::endl;
    } else {
        setSpeed(0);
        setDirection(STOP);
        std::cout<<"||"<<std::endl;
    }
}
