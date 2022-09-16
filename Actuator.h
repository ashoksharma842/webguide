enum direction{BACKWARD = -1, STOP = 0, FORWARD = 1};

class Actuator
{
	public :
		Actuator(direction direction = STOP, int sd = 0, bool fb = false);
		void setDirection(direction dir);
		direction getDirection();
		void setFbType(bool type);
		int getFbData();
		void setSpeed(unsigned int speed);
		void move(int correction);
		
	private :
		direction m_direction;
		unsigned int m_speed;
		bool m_feedback;
		int m_feedbackData;
		int m_prevDir;
};
