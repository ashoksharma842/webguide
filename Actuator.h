enum direction_t{BACKWARD = -1, STOP = 0, FORWARD = 1};

class Actuator
{
	public :
		Actuator(int Apin = 26, int Bpin = 27, int pwmPin = 14, bool fb = false, int crnt = 100);
		void setDirection(direction_t dir);
		direction_t getDirection();
		void setFbType(bool type);
		int getFbData();
		void actuatorSetSpeed(unsigned int spd);
		void actuatorMove(int correction);
    void actuatorInit();
    void setCurrent(int current);
    int getCurrent();
		
	private :
		direction_t m_direction;
		unsigned int m_speed;//pwm
		bool m_feedback;
		int m_feedbackData;
		int m_prevDir;
    int m_Apin;
    int m_Bpin;
    int m_pwmPin;
    int m_freq;
    int m_pwmChannel;
    int m_resolution;
    int m_current;
};
