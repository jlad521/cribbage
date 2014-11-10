//#include "Cribbage.h"
#include "testCribbage.h"
#include "Deck.h"
#include "Player.h"
#include "ShowCribbage.h"
//#include "Deck.h"
#include <iostream>
#include <string>
#include <time.h>
using namespace std;
#define WINNING 10
//Global Declarations:
vector<Card*> crib;
testCribbage::testCribbage(){
    playGame();
}

void testCribbage::checkDeck(vector<Card*> deck) {
    cout << "The deck: " << endl;
    for (int i = 0; i < deck.size(); i++) {
        cout << deck.at(i)->getValue() << deck.at(i)->getSuit() << endl;
    }
}

void testCribbage::checkDeckwithHand(vector<Card*> deck, vector<Card*> crib) {
    for ( int i = 0; i < deck.size(); i++) {
        for(int h = 0; h < 6; h++) {
            if (crib.at(h)->getValue() == deck.at(i)->getValue() && crib.at(h)->getSuit() == deck.at(i)->getSuit()) {
                printf("This card is still in the deck: %c-%d\n", crib.at(h)->getValue(), crib.at(h)->getSuit());
            }
        }
    }
}
// Looks through the deck to see if cutDeck() properly removed the card from the deck
// // Run after using the removeCard or removeRandomCard method
void testCribbage::testRemoveCard(vector<Card*> deck, Card* removedCard) {
    for(int i = 0; i < deck.size(); i++) {
        if(removedCard->getValue() == deck.at(i)->getValue() && removedCard->getSuit() == deck.at(i)->getValue()){
            cout << "the card is still in the deck!" << deck.at(i)->getValue() << deck.at(i)->getSuit() << endl;
        }
    }
}

void testCribbage::checkJackSameSuit(vector<Card*> crib, Card* cutCard){
    cout << "Cut Card is: " << cutCard << endl;
    cout << "Player's hand is: " << endl;
    for (int i = 0; i < crib.size(); i++){
        cout << crib.at(i)->getValue() << crib.at(i)->getSuit() << endl;
    }

    if (JackSameSuit(crib, cutCard) != 1)
        printf("Jack from hand is NOT the same suit as starter card's.");
    else
        printf("Jack from hand IS the same suit as the starter card's.");

}

void testCribbage::checkFifteen(vector<Card*> crib){
    cout << "Player's hand: " << endl;
    for (int i = 0; i < crib.size(); i++){
        cout << crib.at(i)->getValue() << crib.at(i)->getSuit() << endl;
    }
    int x = Fifteen(crib);
    cout << "Player's hand has " << x << " combinations that equal 15." << endl;

}

void testCribbage::checkRunScore(vector<Card*> crib){
    cout << "Player's hand: " << endl;
    for (int i = 0; i < crib.size(); i++){
        cout << crib.at(i)->getValue() << crib.at(i)->getSuit() << endl;
    }
    int x = RunScore(crib);
    cout << "Player's score from runs is " << x << " ." << endl;
}

void testCribbage::checkFlushScore(vector<Card*> crib){
    cout << "Player's hand: " << endl;
    for (int i = 0; i < crib.size(); i++){
        cout << crib.at(i)->getValue() << crib.at(i)->getSuit() << endl;
    }
    int x = FlushScore(crib);
    cout << "Player's hand has a " << x << " Card flush." << endl;
}

void testCribbage::checkPairs(vector<Card*> crib){
    cout << "Player's hand: " << endl;
    for (int i = 0; i < crib.size(); i++){
        cout << crib.at(i)->getValue() << crib.at(i)->getSuit() << endl;
    }
    int x = Pairs(crib);
    cout << "Player's hand has " << x << " number of pair combinations." << endl;
}

void testCribbage::checkThirtyOne(vector<Card*> crib){
    int total = 0;

    for (int i = 0; i < crib.size(); i++){
        total = total + crib.at(i)->getValue();
    }
    cout << "Player's hand totals to: " << total << endl;

    if (ThirtyOne(crib) != 2) {cout << "ThirtyOne function shows hand does NOT equal 31." << endl; }
    else {cout << "ThirtyOne function shows hand equals 31." << endl; }
}

void testCribbage::checkPairsGo(vector<Card*> crib){
    int i;
    cout << "Cards played so far: " << endl;
    for (i = 0; i < crib.size(); i++){
        cout << crib.at(i)->getValue() << crib.at(i)->getSuit() << endl;
    }
    int x = Pairs(crib);
    cout << "There are currently " << x << " number of pair combinations." << endl;
}

void testCribbage::checkFifteenGo(vector<Card*> crib){
    int i;
    cout << "Cards played so far: " << endl;
    for (i = 0; i < crib.size(); i++){
        cout << crib.at(i)->getValue() << crib.at(i)->getSuit() << endl;
    }
    int x = Fifteen(crib);
    cout << "There are currently " << x << " combinations equalling 15." << endl;
}

void testCribbage::checkRunScoreGo(vector<Card*> crib){
    int i;
    cout << "Cards played so far: " << endl;
    for (i = 0; i < crib.size(); i++){
        cout << crib.at(i)->getValue() << crib.at(i)->getSuit() << endl;
    }
    int x = Pairs(crib);
    cout << "There are currently a score of " << x << " for runs." << endl;
}
void testCribbage::playGame(){
    /* This code initializes every object. Write functions here to test your functions*/
    Player * human = new Player(true, "Jesus");
    Player * AI = new Player(false, "ROBOT");
    Deck * deck = new Deck();
    Player* players[2];
    players[0] = human;
    players[1] = AI;
    int dealerPos = 1; //should randomly generate
    int pTurn = 0;
    players[0]->hand = deck->dealCards(); //puts 6 random cards in player's hand. feel free to pop elements off if you want 5 or 4 card hand
    players[1]->hand = deck->dealCards();

    crib.push_back(players[0]->hand.back());
    players[0]->hand.pop_back(); //remove card from hand
    crib.push_back(players[0]->hand.back());
    players[0]->hand.pop_back(); //remove card from hand
    players[0]->scoreHand = players[0]->hand;

    crib.push_back(players[1]->hand.back());
    players[1]->hand.pop_back(); //remove card from hand
    crib.push_back(players[1]->hand.back());
    players[1]->hand.pop_back(); //remove card from hand
    players[1]->scoreHand = players[1]->hand;

    Card* cut;
    cut = deck->cutDeck();
    players[0]->scoreHand.push_back(cut); //
    players[1]->scoreHand.push_back(cut);
    /* Drive your functions and write if statements to check if the return value of your
     * function to make sure it's what it should be. I've left the entire code base below if
     * you need to see how something is implemented.  */

    /* TEST CODE HERE: */
    checkJackSameSuit(players[0]->hand, cut);
    checkFifteen(players[0]->hand);
    checkFifteen(players[0]->hand);
    checkRunScore(players[0]->hand);
    checkFlushScore(players[0]->hand);
    checkPairs(players[0]->hand);
    checkThirtyOne(players[0]->hand);
    checkPairsGo(players[0]->hand);
    checkFifteenGo(players[0]->hand);
    checkRunScoreGo(players[0]->hand);
    checkDeck(players[0]->hand);
    checkDeckwithHand(deck->deck, crib);
    testRemoveCard(players[0]->hand, cut);
}

vector<Card*> testCribbage::sortHand(vector<Card*> v){
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
    //printHand(v, l);
}


int testCribbage::JackSameSuit(vector<Card*> v, Card* cut){ // 1 pt if player's card is Jack and same suit as starter card
    int i;

    for(i = 0; i < v.size(); i++){
        if(v.at(i)->getFace() == 'J' && v.at(i)->getSuit() == cut->getSuit()){
            return (1);
        }
    }
    return(0);
}


int testCribbage::Pairs(vector<Card*> v){ // 2 pts per pair (3 of a kind - 3 pairs; 4 of a kind - 6 pairs)
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


int testCribbage::ThirtyOne(vector<Card*> v){ // 2 pts for reaching 31 exactly
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

int testCribbage::Fifteen(vector<Card*> v){  // 2 pts per combination that equals fifteen
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


int testCribbage::RunScore(vector<Card*> v){ // n points per card in run
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



int testCribbage::FlushScore(vector<Card*> v){ // 4 points for 4 cards in player's hand all of same suit
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


int testCribbage::PairsGo(vector<Card*> v){
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


int testCribbage::RunScoreGo(vector<Card*> v){
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







