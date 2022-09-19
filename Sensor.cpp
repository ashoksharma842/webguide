#include "Sensor.h"
#include <Arduino.h>
Sensor::Sensor(int gp, int adcPin, int gain)
{
    m_guidePoint = gp;
    m_adcPin = adcPin;
    m_gain = gain;
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
** Function name:           setGain
** Description:             Set gain of the sensor
***************************************************************************************/
void Sensor :: setGain(int gain)
{
  m_gain = gain;
}

/***************************************************************************************
** Function name:           getGain
** Description:             Get gain of the sensor
***************************************************************************************/
int Sensor :: getGain()
{
  return m_gain;
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
