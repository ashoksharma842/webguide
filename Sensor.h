class Sensor
{
	public :
	  Sensor(int gp = 21, int adcPin = 34, int gain = 1);
		void setGuidePoint(int gp);
		int getGuidePoint();
		int getData();
		void setData(int data);
    int getGain();
    void setGain(int gain);
		
	private :
		int m_data;
		int m_guidePoint;
    int m_adcPin;
    int m_gain;
};
