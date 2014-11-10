#include <vector>
//#include "Card.h"
using namespace std;
class Player;
class Deck;
class ShowCribbage;
class Card;
class Cribbage{
    public:
        Cribbage();
    private:
        void playGame();
        void playRound(Player* ps[], ShowCribbage*);
        void goPhase(Player* ps[], Deck*, ShowCribbage*, Card*);
        int AIGoChoice(Player*, int);
        int AIHandChoice(Player* p);
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
        int goPoints(vector<Card*> v);
        //void scorePhase(Players* p[], vector<Card*> crib);
        int dealerPos;
        int pTurn;
};
