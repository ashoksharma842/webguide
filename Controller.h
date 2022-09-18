#include <iostream>

enum eOperatingMode{MANUAL, AUTO, SC};
enum eGuidingMode{S1, S2};

class Controller 
{
	public:
		Controller(eOperatingMode oMode = MANUAL, eGuidingMode gMode = S1);
		eOperatingMode getOperatingMode();
		eGuidingMode getGuidingMode();
		void setOperatingMode(eOperatingMode oMode);
		void setGuidingMode(eGuidingMode gMode);
		void dispCtrlStatus();
		
	private:
		eOperatingMode m_operatingMode;
		eGuidingMode m_guidingMode;
};
