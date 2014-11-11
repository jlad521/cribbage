class Card{
public:
	Card(char a, int i, int t);
    Card();
    ~Card();
	void setFace(char a);
	void setValue(int x);
	void setSuit(int y);
	void setPriority(int d);
	int setSuit();
	int setPriority();
	char getFace();
	int getValue();
    int getPriority();
    int getSuit();
private:
	char faceValue;
	int value;
	int priority;
	int suit;
};
