#include <vector>
//#include "Card.h"
using namespace std;
class Player;
class Deck;
class ShowCribbage;
class Card;
class testCribbage{
    public:
        testCribbage();
    private:
        void playGame();
        vector<Card*> sortHand(vector<Card*> v);
        int scorePhase(vector<Card*>, Card*);
        int Pairs(vector<Card*> v);
        int ThirtyOne(vector<Card*> v);
        int JackSameSuit(vector<Card*> v, Card* cut);
        int Fifteen(vector<Card*> v);
        int RunScore(vector<Card*> v);
        int FlushScore(vector<Card*> v);
        int PairsGo(vector<Card*> v);
        int RunScoreGo(vector<Card*> v);
        int FifteenGo(vector<Card*> v);
        void checkJackSameSuit(vector<Card*>, Card*);
        void checkFifteen(vector<Card*>);
        void checkRunScore(vector<Card*>);
        void checkFlushScore(vector<Card*>);
        void checkPairs(vector<Card*>);
        void checkThirtyOne(vector<Card*>);
        void checkPairsGo(vector<Card*>);
        void checkFifteenGo(vector<Card*>);
        void checkRunScoreGo(vector<Card*>);
        void checkDeck(vector<Card*>);
        void checkDeckwithHand(vector<Card*>, vector<Card*>);
        void testRemoveCard(vector<Card*>, Card*);
};
