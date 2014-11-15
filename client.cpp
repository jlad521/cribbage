#include <cstdlib>
#include <string>
#include <iostream>
#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client_simple.hpp>
#include <signal.h>
#include <ncurses.h>
#include <sstream>
//#include "display.h"
#include <unistd.h>
#include "Cribbage.h"
#include "clientCribbage.h"
//#include "Card.h"
#include "Player.h"
#include "Deck.h"
#include "ShowCribbage.h"
#define WINNING 61
//#include "client.h"
class clientConnection{
    private:
        xmlrpc_c::clientSimple myClient;// = new(xmlrpc_c::clientSimple);
        static bool instanceFlag;
        static clientConnection* an_instance;
        clientConnection(){};
    public:
        vector<int> phaseI();
        static clientConnection* createInstance();
        ~clientConnection() { instanceFlag = false;}
};

bool clientConnection::instanceFlag = false;
clientConnection* clientConnection::an_instance = NULL;

clientConnection* clientConnection::createInstance(){
    if(!instanceFlag){
        an_instance = new clientConnection();
        instanceFlag = true;
        return an_instance;
    }
}
//Have it return ints for now, but will eventually return vector<Card*>
vector<int> clientConnection::phaseI(){
    xmlrpc_c::value result;
        vector<int> cards;
    try{
        string const serverUrl("http://localhost:8080/RPC2");
        string const phaseI("server.phaseI");
        myClient.call(serverUrl, phaseI, &result);
        xmlrpc_c::value_array dealtHand(result);
        vector<xmlrpc_c::value> const hand(dealtHand.vectorValueValue());
        for(int i = 0; i < 6; i++){
//MIGHT HAVE TO STORE AS CONST INT FIRST:   int const val1 = xmlrpc_c::value_int(hand.at(i));
        cards.push_back(xmlrpc_c::value_int(hand.at(i)));
        }
        return cards;
    } catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
        sleep(6000);
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
        sleep(6000);
    }
}


//string const serverUrl("http://localhost:8080/RPC2");
//string const phaseI("server.phaseI");
//string const definePlayers("define_players");
using namespace std;

vector<Card*> crib;
int dealerPos, pTurn; // I'm sure there'll be more
    clientConnection *myClient;

void goPhase(Player* players[], Deck * deck, ShowCribbage* display, Card* cut){
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
            //RPC CALL
            //index = AIGoChoice(players[pTurn], goNum);
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
        //RPC CALL
        //players[pTurn]->addPoints(goPoints(roundCards)); //check for pairs, runs and add to player's score
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

void playRound(Player* players[] , ShowCribbage* display) {
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
    //RPC CALLS!
    //pScore = scorePhase(players[0]->scoreHand, cut);
    players[0]->addPoints(pScore);
    //AIscore = scorePhase(players[1]->scoreHand, cut);
    players[1]->addPoints(AIscore);
    //cribScore = scorePhase(crib, cut);
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

void playGame(){
    //Ask player to input name?
    Player * human = new Player(/*getPlayerIno*/true, "Jesus");
    Player * AI = new Player(/*getPlayerInfo*/false, "ROBOT");
    //ShowCribbage * display = new ShowCribbage();
    Player* players[2];
    players[0] = human;
    players[1] = AI;
    dealerPos = 0; //should randomly generate
    pTurn = 1;
    myClient = clientConnection::createInstance();
    xmlrpc_c::value resultHand;
    vector<int> testReturned;
    testReturned = myClient->phaseI();
    for(int k =0; k < testReturned.size(); k++){
        cout << "Card Index returned: " << testReturned.at(k);
    }

    //myClient->call(serverUrl,phaseI,&resultHand);
    //xmlrpc_c::value_array handFromServer(resultHand);
    //vector<xmlrpc_c::value> const dealtHand(handFromServer);
    /*
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


int main(int argc, char* argv[]){


    playGame();
    return 0;
}

