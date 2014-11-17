#ifndef CARD_H
#define CARD_H
//#include "Card.h"
#include <vector>
#include <string>
#endif
class Card;
using namespace std;
class Player{
    public:
        Player(bool, string);
        Player(int, int, bool, string, Card*); //NEED TO INCLUDE HANDS
        vector<Card*> hand;
        vector<Card*> scoreHand;
        Card* lastCard;
        string name;
        int points;
        bool isHuman;
        int position;

        void reset(string);
        string serialize();
        int getPosition();
        void setLastCard(Card* card);
        void setHuman(bool val);
        Card* discard(int index);
        void addPoints(int morePoints);
        Card* getLastCard();
        int getPoints();
        std::vector<Card*> getHand();
        std::vector<Card*> getScoreHand();
};
