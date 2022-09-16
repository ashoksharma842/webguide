#include "Controller.h"

/***************************************************************************************
** Function name:           getOperatingMode
** Description:             Get operating mode of the controller
***************************************************************************************/
OperatingMode Controller::getOperatingMode()
{
	return m_operatingMode;
}

/***************************************************************************************
** Function name:           getGuidingMode
** Description:             Get guiding mode of the controller
***************************************************************************************/
GuidingMode Controller::getGuidingMode()
{
	return m_guidingMode;
}

/***************************************************************************************
** Function name:           setOperatingMode
** Description:             Set operating mode of the controller
***************************************************************************************/
void Controller::setOperatingMode(OperatingMode oMode)
{
	m_operatingMode = oMode;
}

/***************************************************************************************
** Function name:           setGuidingMode
** Description:             Set guiding mode of the controller
***************************************************************************************/
void Controller::setGuidingMode(GuidingMode gMode)
{
	m_guidingMode = gMode;
}

/***************************************************************************************
** Function name:           Controller
** Description:             Constructor , default : manual mode and sensor1
***************************************************************************************/
Controller :: Controller(OperatingMode oMode, GuidingMode gMode)
{
	m_guidingMode = gMode;
	m_operatingMode = oMode;
}

/***************************************************************************************
** Function name:           Controller
** Description:             Constructor , default : manual mode and sensor1
***************************************************************************************/
void Controller :: dispCtrlStatus()
{
	std::cout<<"oMode : " << m_operatingMode << std::endl;
	std::cout<<"gMode : " << m_guidingMode << std::endl;
}
