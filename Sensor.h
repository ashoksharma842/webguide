class Sensor
{
	public :
	  Sensor(int gp = 21, int adcPin = 34);
		void setGuidePoint(int gp);
		int getGuidePoint();
		int getData();
		void setData(int data);
		
	private :
		int m_data;
		int m_guidePoint;
    int m_adcPin;
};
