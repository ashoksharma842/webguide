#include "Sensor.h"

Sensor::Sensor(int gp)
{
    m_guidePoint = gp;
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
	return m_data;
}

/***************************************************************************************
** Function name:           setData
** Description:             Set ADC data of the sensor
***************************************************************************************/
void Sensor :: setData(int data)
{
	m_data = data;
}
