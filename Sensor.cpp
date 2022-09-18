#include "Sensor.h"
#include <Arduino.h>
Sensor::Sensor(int gp, int adcPin)
{
    m_guidePoint = gp;
    m_adcPin = adcPin;
}
/***************************************************************************************
** Function name:           setGuidePoint
** Description:             Set guide point of the sensor
***************************************************************************************/
void Sensor :: setGuidePoint(int gp)
{
	m_guidePoint = gp;
}

/***************************************************************************************
** Function name:           getGuidePoint
** Description:             Get guide point of the sensor
***************************************************************************************/
int Sensor :: getGuidePoint()
{
	return m_guidePoint;
}

/***************************************************************************************
** Function name:           getData
** Description:             Get ADC data of the sensor
***************************************************************************************/
int Sensor :: getData()
{
	return analogRead(m_adcPin);
}

/***************************************************************************************
** Function name:           setData
** Description:             Set ADC data of the sensor
***************************************************************************************/
void Sensor :: setData(int data)
{
	m_data = data;
}
