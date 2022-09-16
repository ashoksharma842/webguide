#include <iostream>

enum OperatingMode{MANUAL, AUTO, SC};
enum GuidingMode{S1, S2};

class Controller 
{
	public:
		Controller(OperatingMode oMode = MANUAL, GuidingMode gMode = S1);
		OperatingMode getOperatingMode();
		GuidingMode getGuidingMode();
		void setOperatingMode(OperatingMode oMode);
		void setGuidingMode(GuidingMode gMode);
		void dispCtrlStatus();
		
	private:
		OperatingMode m_operatingMode;
		GuidingMode m_guidingMode;
};
