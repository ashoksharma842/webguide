class Sensor
{
	public :
	    Sensor(int gp = 21);
		void setGuidePoint(int gp);
		int getGuidePoint();
		int getData();
		void setData(int data);
		
	private :
		int m_data;
		int m_guidePoint;
};
