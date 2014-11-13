#include "Cribbage.h"
#include "Deck.h"
#include "Player.h"
#include "ShowCribbage.h"
#include <iostream>
#include <string>
#include <time.h>
using namespace std;
#define WINNING 61  //Technically games go to 121, but I've halfed it for brevity

vector<Card*> crib;

Cribbage::Cribbage(){
    playGame();
}

void Cribbage::playGame(){
    //Ask player to input name?
    Player * human = new Player(true, "Jesus");
    Player * AI = new Player(false, "ROBOT");
    //ShowCribbage * display = new ShowCribbage();
    /*
    Player* players[2];
    players[0] = human;
    players[1] = AI;
    dealerPos = 0; //should randomly generate
    pTurn = 1;
    while(players[0]->getPoints() < WINNING && players[1]->getPoints() < WINNING){
        playRound(players, display);
        dealerPos = (dealerPos +1) % 2;
    }
    int winPos;
    if(players[0]->getPoints() > WINNING){winPos = 0; }
    else { winPos = 1;}
    display->displayWinner(players, winPos);
    delete human;
    delete AI;
    delete display;
    */
}

void Cribbage::playRound(Player* players[] , ShowCribbage* display) {
    srand(time(NULL));
    Deck * deck = new Deck();
    players[0]->hand = deck->dealCards();
    players[1]->hand = deck->dealCards();
    Card* cut = new Card();
    //Call
    display->drawCards(players, cut, 1, 0, false, crib, dealerPos, 0, 0);
    int gophase = 0;
    int selected;

    /*Discard 1st human card */
    selected = display->getCard(players[0], 1, gophase); //getCard only allows a valid card to be played (play game to see)
    crib.push_back(players[0]->hand.at(selected)); //add discarded cards to crib
    players[0]->hand.erase(players[0]->hand.begin()+selected);
    display->drawCards(players, cut, 1, 0, true, crib, dealerPos, 0, 0);

    /*Discard 2nd human card */
    selected = display->getCard(players[0], 1, gophase);
    crib.push_back(players[0]->hand.at(selected));
    players[0]->hand.erase(players[0]->hand.begin()+selected);
    display->drawCards(players, cut, 1, 0, true, crib, dealerPos,  0, 0);
    players[0]->scoreHand = players[0]->hand; //update scoring hand with card chosen

    /*Discard AI cards */
    crib.push_back(players[1]->hand.back());
    players[1]->hand.pop_back(); //remove card from hand
    crib.push_back(players[1]->hand.back());
    players[1]->hand.pop_back(); //remove card from hand
    display->drawCards(players, cut, 1, 0, true, crib, dealerPos,  0, 0);
    players[1]->scoreHand = players[1]->hand;

    //test that cards were placed in crib:
    //display->testCards(crib);

    /*Now cut deck */
    cut = deck->cutDeck();
    if(cut->getPriority() == 11){ players[dealerPos]->addPoints(2);}
    players[0]->scoreHand.push_back(cut); //
    players[1]->scoreHand.push_back(cut);
    crib.push_back(cut);
    display->drawCards(players, cut, 1, 0, true, crib, dealerPos,  0, 0);

    /*Now play the go phase */
    goPhase(players, deck, display, cut);

    /* Go phase over. Now score hand */
    int pScore = 0;
    int AIscore = 0;
    int cribScore = 0;
    pScore = scorePhase(players[0]->scoreHand, cut);
    players[0]->addPoints(pScore);
    AIscore = scorePhase(players[1]->scoreHand, cut);
    players[1]->addPoints(AIscore);
    cribScore = scorePhase(crib, cut);
    if(dealerPos == players[0]->getPosition()){
        players[0]->addPoints(cribScore);
        pScore += cribScore;
    }
    else {
        players[1]->addPoints(cribScore);
        AIscore += cribScore;
    }
    display->drawCards(players, cut, 3, 0, true, crib, dealerPos,  AIscore, pScore);
    display->drawCards(players, cut, 3, 0, true, crib, dealerPos,  AIscore, pScore);
    display->spinWait();

    /* Reset for next round */
    crib.erase(crib.begin(), crib.begin() + crib.size());
    Card* blank;
    players[0]->lastCard = blank;
    players[1]->lastCard = blank;
    delete cut;
    delete deck;
}

void Cribbage::goPhase(Player* players[], Deck * deck, ShowCribbage* display, Card* cut){
    pTurn = (dealerPos +1) % 2;
    vector<Card*> roundCards;
    int goNum = 0;
    int index;
    int played = 0;
    int lastPlayed = 0;
    while(!players[0]->hand.empty() || !players[1]->hand.empty()){
        display->drawCards(players, cut, 2, goNum, true, crib, dealerPos,  0, 0);
        lastPlayed = played;
        if(players[pTurn]->isHuman){
            index = display->getCard(players[pTurn], 2, goNum);
            if(index > -1){
                players[pTurn]->lastCard = players[pTurn]->hand.at(index); //
                roundCards.push_back(players[pTurn]->hand.at(index)); //
                goNum += players[pTurn]->lastCard->getValue();
                players[pTurn]->hand.erase(players[pTurn]->hand.begin()+index);
                //cout << "player just selected: " << index;
                //Prove that cards are here:
                //display->testCards(players[0]->scoreHand);
                played = 0;
            }
            else {played = -1;}
        }
        else{
            index = AIGoChoice(players[pTurn], goNum);
            if(index > -1){
                players[pTurn]->lastCard = players[pTurn]->hand.at(index); //
                roundCards.push_back(players[pTurn]->hand.at(index)); //
                goNum += players[pTurn]->lastCard->getValue();
                players[pTurn]->hand.erase(players[pTurn]->hand.begin()+index);
                played = 0;
            }
            else {played = -1;}
        }
        /* Check if recently played cards should yield additional points */
        players[pTurn]->addPoints(goPoints(roundCards)); //check for pairs, runs and add to player's score
        if(goNum == 15) {players[pTurn]->addPoints(2);}
        if(goNum == 31) {
            players[pTurn]->addPoints(2);
            goNum = 0;
            roundCards.erase(roundCards.begin(), roundCards.begin() + roundCards.size());
        }
        /* If both players said "go," reset goPhaseNumber */
        if(lastPlayed == -1 && played == -1){
            goNum = 0;
            players[pTurn]->addPoints(1);
            roundCards.erase(roundCards.begin(), roundCards.begin() + roundCards.size());
        }
        pTurn = (pTurn + 1) % 2;
    }
    /* Assign last go point after both players are out of cards */
    players[(pTurn +1) %2]->addPoints(1); //Tested that this always assigns 1 pt to last player who played card
    display->drawCards(players, cut, 2, goNum, true, crib, dealerPos, 0, 0);
}
int Cribbage::goPoints(vector<Card*> v){
    int pts = 0;
    pts += PairsGo(v);
    pts += RunScoreGo(v);
    return pts;
}

int Cribbage::AIGoChoice(Player* p, int goNum){
    for(int i = 0; i < p->hand.size(); i++){
        if(p->hand.at(i)->getValue() + goNum < 31) {return i;}
    }
    return -1; //return "go" if can't play valid card
}

int Cribbage::scorePhase(vector<Card*> hand, Card* cut){
    int points = 0;
    points += Pairs(hand);
    points += JackSameSuit(hand, cut);
    points += Fifteen(hand);
    points += RunScore(hand);
    points += FlushScore(hand);
    //cout << "IT FOUND: " << points;
    return points;
}

vector<Card*> Cribbage::sortHand(vector<Card*> v){
    int i, j;
    Card*  t;
    vector<Card*> temp;

    for (i = 0; i < (v.size()-1); i++){
        for (j = 0; j < (v.size()-1); j++){
            if ((v.at(j+1)->getPriority()) < (v.at(j)->getPriority())){
                t = v[j];
                v[j] = v[j+1];
                v[j+1] = t;
            }
        }
    }
    return v;
}


int Cribbage::JackSameSuit(vector<Card*> v, Card* cut){ // 1 pt if player's card is Jack and same suit as starter card
    int i;

    for(i = 0; i < v.size(); i++){
        if(v.at(i)->getFace() == 'J' && v.at(i)->getSuit() == cut->getSuit()){
            return (1);
        }
    }
    return(0);
}


int Cribbage::Pairs(vector<Card*> v){ // 2 pts per pair (3 of a kind - 3 pairs; 4 of a kind - 6 pairs)
    int n = 0;
    int i, b;

    for(i = 0; i < v.size(); i++){
        for(b = (i+1); b < v.size(); b++){
            if(v.at(i)->getFace() == v.at(b)->getFace()){
                n++;
            }
        }
    }
    return (n*2);
}


int Cribbage::ThirtyOne(vector<Card*> v){ // 2 pts for reaching 31 exactly
    int n = 0;
    int i;

    for(i = 0; i < v.size(); i++){
        n = n + (v.at(i)->getValue());
    }

    if(n == 31)
        return(2);
    else
        return(0);
}

int Cribbage::Fifteen(vector<Card*> v){  // 2 pts per combination that equals fifteen
    int n = 0;
    int i = 0;
    int b, d, e;
    vector<Card*> temp;

    v = sortHand(v);

    if((v.at(i)->getValue()) + (v.at(i+1)->getValue()) + (v.at(i+2)->getValue()) + (v.at(i+3)->getValue()) + (v.at(i+4)->getValue()) == 15){n++;} // 5-card combo

    for(i = 0; i < v.size(); i++){ // 4 card combination
        for(b = (i+1); b < v.size(); b++){
            for(d = (i+2); d < v.size(); d++){
                for(e = (i+3); e < v.size(); e++){
                    if (v.at(i)->getValue() + v.at(b)->getValue() + v.at(d)->getValue() + v.at(e)->getValue() == 15){
                        n++;
                    }
                }
            }
        }
    }

    for(i = 0; i < v.size(); i++){ // 3 card combination
        for(b = (i+1); b < v.size(); b++){
            for(d =(i+2); d < v.size(); d++){
                if((v.at(i)->getValue()) + (v.at(b)->getValue()) + (v.at(d)->getValue()) == 15){
                    n++;
                }
            }
        }
    }

    for(i = 0; i < v.size(); i++){ // 2 card combination
        for(b = (i+1); b < v.size(); b++){
            if((v.at(i)->getValue()) + (v.at(b)->getValue()) == 15){
                n++;
            }
        }
    }

    return (n*2);
}


int Cribbage::RunScore(vector<Card*> v){ // n points per card in run
    v = sortHand(v);

    int i;
    int n = 0;

    for(i = 0; i < (v.size()-2); i++){
        if (i == 0 && (v[i+4]->getPriority()) == (v[i]->getPriority() + 4) &&
                (v[i+3]->getPriority()) == (v[i]->getPriority() + 3) &&
                (v[i+2]->getPriority()) == (v[i]->getPriority() + 2) &&
                (v[i+1]->getPriority()) == (v[i]->getPriority() + 1)){return(5);} // check for 5-card run
        else if ((i == 1 || i == 0) && (v.at(i+3)->getPriority()) == (v.at(i)->getPriority() + 3) &&
                (v.at(i+2)->getPriority()) == (v.at(i)->getPriority() + 2) &&
                (v.at(i+1)->getPriority()) == (v.at(i)->getPriority() + 1)){return(4);} // check for 4-card run
        else if ((i == 2 || i == 1 || i == 0) && (v.at(i+2)->getPriority()) == (v.at(i)->getPriority() + 2) &&
                (v.at(i+1)->getPriority()) == (v.at(i)->getPriority() + 1)){return(3);} // check for 3-card run
    }
    return(0);
}



int Cribbage::FlushScore(vector<Card*> v){ // 4 points for 4 cards in player's hand all of same suit
    int i = 0;
    int clubs = 0;
    int diamonds = 0;
    int hearts = 0;
    int spades = 0;

    for(i = 0; i < v.size(); i++){ // checking for flush from 4 cards
        switch(v.at(i)->getSuit()){
            case (1):
                spades++;
                break;
            case (2):
                hearts++;
                break;
            case (3):
                clubs++;
                break;
            case (4):
                diamonds++;
                break;
            default:
                printf("This ain't no officiated suit, son..");
                break;
        }
    }

    if(clubs >= 4)
        return(clubs);
    else if(diamonds >= 4)
        return(diamonds);
    else if(hearts >= 4)
        return(hearts);
    else if(spades >= 4)
        return(spades);
    else
        return(0);

}


int Cribbage::PairsGo(vector<Card*> v){
    int SIZE = (v.size()-1);

    if(SIZE == 2){
        if ((v.at(SIZE)->getPriority() == v.at(SIZE-1)->getPriority())) { // one pair
            return(2);
        }
    }

    if(SIZE == 3){
        if ((v.at(SIZE)->getPriority() == v.at(SIZE-1)->getPriority()) && (v.at(SIZE)->getPriority() == v.at(SIZE-2)->getPriority()) ){ // 3 pair (3-of-a-kind)
            return(6);
        }

        if ((v.at(SIZE)->getPriority() == v.at(SIZE-1)->getPriority())) { // one pair
            return(2);
        }
    }

    if(SIZE >= 4){
        if ((v.at(SIZE)->getPriority() == v.at(SIZE-1)->getPriority()) && (v.at(SIZE)->getPriority() == v.at(SIZE-2)->getPriority()) && (v.at(SIZE)->getPriority() == v.at(SIZE-3)->getPriority()) ){ // 6 pair (4-of-a-kind)
            return(12);
        }

        if ((v.at(SIZE)->getPriority() == v.at(SIZE-1)->getPriority()) && (v.at(SIZE)->getPriority() == v.at(SIZE-2)->getPriority()) ){ // 3 pair (3-of-a-kind)
            return(6);
        }

        if ((v.at(SIZE)->getPriority() == v.at(SIZE-1)->getPriority())) { // one pair
            return(2);
        }
    }

    return(0);
}


int Cribbage::RunScoreGo(vector<Card*> v){
    int SIZE = (v.size()-1);
    /*
       if(SIZE == 3){
       if ((v.at(SIZE-2)->getPriority() == v.at(SIZE)->getPriority()-2 ) && (v.at(SIZE-1)->getPriority() == v.at(SIZE)->getPriority()-1 ) ||
       (v.at(SIZE-2)->getPriority() == v.at(SIZE)->getPriority()+2 ) && (v.at(SIZE-1)->getPriority() == v.at(SIZE)->getPriority()+1 )){ // 3-card run combo
       return(3);
       }

       }*/

    if(SIZE == 3){
        if ((v.at(SIZE-3)->getPriority() == v.at(SIZE)->getPriority()-3 ) && (v.at(SIZE-2)->getPriority() == v.at(SIZE)->getPriority()-2 ) && (v.at(SIZE-1)->getPriority() == v.at(SIZE)->getPriority()-1 ) ||
                (v.at(SIZE-3)->getPriority() == v.at(SIZE)->getPriority()+3 ) && (v.at(SIZE-2)->getPriority() == v.at(SIZE)->getPriority()+2 ) && (v.at(SIZE-1)->getPriority() == v.at(SIZE)->getPriority()+1 )){ // 4-card run combo
            return(4);
        }

        if ((v.at(SIZE-2)->getPriority() == v.at(SIZE)->getPriority()-2 ) && (v.at(SIZE-1)->getPriority() == v.at(SIZE)->getPriority()-1 ) ||
                (v.at(SIZE-2)->getPriority() == v.at(SIZE)->getPriority()+2 ) && (v.at(SIZE-1)->getPriority() == v.at(SIZE)->getPriority()+1 )){ // 3-card run combo
            return(3);
        }
    }

    if(SIZE >= 4){
        if ((v.at(SIZE-4)->getPriority() == v.at(SIZE)->getPriority()-4 ) && (v.at(SIZE-3)->getPriority() == v.at(SIZE)->getPriority()-3 ) && (v.at(SIZE-2)->getPriority() == v.at(SIZE)->getPriority()-2 ) && (v.at(SIZE-1)->getPriority() == v.at(SIZE)->getPriority()-1 )||
                (v.at(SIZE-4)->getPriority() == v.at(SIZE)->getPriority()+4 ) && (v.at(SIZE-3)->getPriority() == v.at(SIZE)->getPriority()+3 ) && (v.at(SIZE-2)->getPriority() == v.at(SIZE)->getPriority()+2 ) && (v.at(SIZE-1)->getPriority() == v.at(SIZE)->getPriority()+1 )){
            return(5); // 5-card combo
        }

        if ((v.at(SIZE-3)->getPriority() == v.at(SIZE)->getPriority()-3 ) && (v.at(SIZE-2)->getPriority() == v.at(SIZE)->getPriority()-2 ) && (v.at(SIZE-1)->getPriority() == v.at(SIZE)->getPriority()-1 ) ||
                (v.at(SIZE-3)->getPriority() == v.at(SIZE)->getPriority()+3 ) && (v.at(SIZE-2)->getPriority() == v.at(SIZE)->getPriority()+2 ) && (v.at(SIZE-1)->getPriority() == v.at(SIZE)->getPriority()+1 )){ // 4-card run combo
            return(4);
        }

        if ((v.at(SIZE-2)->getPriority() == v.at(SIZE)->getPriority()-2 ) && (v.at(SIZE-1)->getPriority() == v.at(SIZE)->getPriority()-1 ) ||
                (v.at(SIZE-2)->getPriority() == v.at(SIZE)->getPriority()+2 ) && (v.at(SIZE-1)->getPriority() == v.at(SIZE)->getPriority()+1 )){ // 3-card run combo
            return(3);
        }
    }

    return(0);
}







