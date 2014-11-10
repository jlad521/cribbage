#include <iostream>
#include "display.h"
#include <cstdlib>
#include <ncurses.h>
#include <signal.h>
#include <sstream>
#include <string>
#include <math.h>
#include <vector>

class Player;
class Card;
class ShowCribbage: public display {
public:
    ShowCribbage();
    void drawCards(Player* p[], Card*, int, int, bool, vector<Card*>, int, int, int);
    int getCard(Player*, int phase, int goPhaseNum); //ultimately getCard
    void spinWait();
    void displayWinner(Player* p[], int);
    void testCards(vector<Card*>);
private:
    display gameDisplay;
    char key;
    int xStart;
    int AIY;
    int humanY;
    int hlpy; //human last played card y position
    int AIlpy; //AI ""
    int yMsg;
    int inc;
    int cribx;
    int xlp;
    int mouseX;
    int mouseY;
    int scoreCribx;
    stringstream messageString;
    stringstream messageStringTop;
    stringstream messageStringBottom;
};
