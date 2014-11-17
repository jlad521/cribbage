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
        static bool instanceFlag;
        static clientConnection* an_instance;
        clientConnection(){};
    public:
        void phaseI();
        void tellDiscard(int, int, int);
        void writeHardState();
        vector<int> getGameInfo();
        vector<int> getPlayerInfo(int);
        vector<int> readCards(int, int);
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
void clientConnection::phaseI(){
    try{
        xmlrpc_c::clientSimple myClient;// = new(xmlrpc_c::clientSimple);
        xmlrpc_c::value result;
        string const serverUrl("http://localhost:8080/RPC2");
        string const phaseI("server.phaseI");
        myClient.call(serverUrl, phaseI, &result);
    } catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
        sleep(6000);
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
        sleep(6000);
    }
}
void clientConnection::tellDiscard(int pIndex, int cIndex, int cardNum){
    try{
        xmlrpc_c::clientSimple myClient;// = new(xmlrpc_c::clientSimple);
        xmlrpc_c::value nothing;
        string const serverUrl("http://localhost:8080/RPC2");
        string const tellDiscard("tellDiscard");
        myClient.call(serverUrl, tellDiscard, "iii", &nothing, pIndex, cIndex, cardNum);
    } catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
        sleep(6000);
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
        sleep(6000);
    }
}

void clientConnection::writeHardState(/* DOES IT NEED ANY INPUTS?? */){
    try{
        xmlrpc_c::clientSimple myClient;// = new(xmlrpc_c::clientSimple);
        xmlrpc_c::value value;
        string const serverUrl("http://localhost:8080/RPC2");
        string const writeHardState("writeHardState");
        myClient.call(serverUrl, writeHardState, &value);
        //You could make the rpc return something if you want, and then manipulate the return here

    } catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
        sleep(6000);
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
        sleep(6000);
    }
}

vector<int> clientConnection::getGameInfo(/* DOES IT NEED ANY INPUTS?? */){
    vector<int> info;
    try{
        xmlrpc_c::clientSimple myClient;// = new(xmlrpc_c::clientSimple);
        xmlrpc_c::value value;
        string const serverUrl("http://localhost:8080/RPC2");
        string const getGameInfo("getGameInfo");
        myClient.call(serverUrl, getGameInfo, &value);

        xmlrpc_c::value_array arrr(value);
        vector<xmlrpc_c::value> const gameInfo(arrr.vectorValueValue());
        info.push_back(xmlrpc_c::value_int(gameInfo.at(0)));
        info.push_back(xmlrpc_c::value_int(gameInfo.at(1)));
        info.push_back(xmlrpc_c::value_int(gameInfo.at(2)));

    } catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
        sleep(6000);
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
        sleep(6000);
    }
    return info;
}

vector<int> clientConnection::getPlayerInfo(int pID){
    vector<int> info;
    try{
        xmlrpc_c::clientSimple myClient;// = new(xmlrpc_c::clientSimple);
        xmlrpc_c::value value;
        string const serverUrl("http://localhost:8080/RPC2");
        string const getPlayerInfo("getPlayerInfo");
        myClient.call(serverUrl, getPlayerInfo, "i", &value, pID);

        xmlrpc_c::value_array arrr(value);
        vector<xmlrpc_c::value> const gameInfo(arrr.vectorValueValue());
        info.push_back(xmlrpc_c::value_int(gameInfo.at(0)));
        info.push_back(xmlrpc_c::value_int(gameInfo.at(1)));
        info.push_back(xmlrpc_c::value_int(gameInfo.at(2)));

    } catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
        sleep(6000);
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
        sleep(6000);
    }
    return info;
}

vector<int> clientConnection::readCards(int pIndex, int context){
    vector<int> info;
    try{
        xmlrpc_c::clientSimple myClient;// = new(xmlrpc_c::clientSimple);
        xmlrpc_c::value value;
        string const serverUrl("http://localhost:8080/RPC2");
        string const readCards("readCards");
        myClient.call(serverUrl, readCards, "ii", &value, pIndex, context);

        xmlrpc_c::value_array arrr(value);
        vector<xmlrpc_c::value> const gameInfo(arrr.vectorValueValue());
        //if(xmlrpc_c::value_int(gameInfo.at(0)) == -1) {return info;}
        if(gameInfo.empty()) {return info;}
        for(int i = 0; i < gameInfo.size(); i++){
        info.push_back(xmlrpc_c::value_int(gameInfo.at(i)));
        }
    } catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
        sleep(6000);
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
        sleep(6000);
    }
    return info;
}
/**********************************************************************************************/

vector<Card*> crib;
int dealerPos, pTurn, phase; // I'm sure there'll be more
clientConnection *myClient;
Card* cut = new Card();
Card* storeCut = new Card();
Player* players[2];
vector<int> cardsToInts(vector<Card*> hand){
    vector<int> convertedHand;
    for(int i = 0; i < hand.size(); i++){
        convertedHand.push_back((hand.at(i)->getSuit()-1)*13 + hand.at(i)->getPriority());
    }
    return convertedHand;
}

int cardToInt(Card* card){
    return (card->getSuit() - 1) * 13 + card->getPriority();
}
vector<Card*> intsToCards(vector<int> hand){
    vector<Card*> convertedHand;
    int temp, priority, suit, value;
    for(int i = 0; i < hand.size(); i++){
        temp = hand.at(i);
        priority = temp % 13;
        value = temp % 13;
        if(value > 10) {value = 10;}
        if(temp > 0 && temp < 14) {suit = 1;}
        if(temp > 13 && temp < 27){suit = 2;}
        if(temp > 26 && temp < 40){suit = 3;}
        if(temp > 39 && temp < 53){suit = 4;}
        convertedHand.push_back(new Card(priority, suit, value));
    }
    return convertedHand;
}
Card* intToCard(int card){
    if(card == 0) {
        Card* c = new Card();
        return c;
    }
    Card* convertedCard;
    int priority, suit, value;
    priority = card % 13;
    value = card % 13;
    if(value > 10) {value = 10;}
    if(card > 0 && card < 14) {suit = 1;}
    if(card > 13 && card < 27){suit = 2;}
    if(card > 26 && card < 40){suit = 3;}
    if(card > 39 && card < 53){suit = 4;}
    convertedCard = new Card(priority, suit, value);
    return convertedCard;
}
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
void phaseIII(){
}
void phaseII(){
}
void phaseI(/*ShowCribbage * display*/){
    myClient = clientConnection::createInstance();
    myClient->phaseI(); //deals cards to players
    players[0]->hand = intsToCards(myClient->readCards(0,0));
    players[1]->hand = intsToCards(myClient->readCards(1,0));
    //display->drawCards(players, cut, 1, 0, false, crib, dealerPos, 0, 0);
    int gophase = 0;
    int selected;


    /*Discard 1st human card */
    while(players[0]->hand.size() > 4){
    //selected = display->getCard(players[0], 1, gophase); //getCard only allows a valid card to be played (play game to see)
    selected = 1;
    crib.push_back(players[0]->hand.at(selected)); //add discarded cards to crib
    myClient->tellDiscard(0, 0, cardToInt(players[0]->hand.at(selected))); //add discarded cards to crib);
    players[0]->hand.erase(players[0]->hand.begin()+selected);
    //display->drawCards(players, cut, 1, 0, true, crib, dealerPos, 0, 0);
    }
    /*Discard 2nd human card */
    //selected = display->getCard(players[0], 1, gophase);
    /*
    selected = 3;
    crib.push_back(players[0]->hand.at(selected));
    myClient->tellDiscard(0, selected,cardToInt(players[0]->hand.at(selected))); //add discarded cards to crib);
    players[0]->hand.erase(players[0]->hand.begin()+selected);
    //display->drawCards(players, cut, 1, 0, true, crib, dealerPos,  0, 0);
    */
    //WRITE A METHOD THAT DOES THIS ON SERVER
    //prepP2
    players[0]->scoreHand = players[0]->hand; //update scoring hand with card chosen

    //myClient->AICards();//

    /*Discard AI cards on client; can get away with this because client and server will always discard last card in array */
    crib.push_back(players[1]->hand.back());
    players[1]->hand.pop_back(); //remove card from hand
    crib.push_back(players[1]->hand.back());
    players[1]->hand.pop_back(); //remove card from hand
    players[1]->scoreHand = players[1]->hand; //update scoring hand with card chosen
    //display->drawCards(players, cut, 1, 0, true, crib, dealerPos,  0, 0);
    phase = 2;
}
//INITIALIZE GAME WILL CALL: READHARDSTATE, WHICH GETS ALL INFO FROM SERVER
void initializeGame(){
    myClient = clientConnection::createInstance();
    vector<int> gameInfo; //dealerPos, pTurn, phase
    vector<int> p0Info;
    vector<int> p1Info;
    gameInfo = myClient->getGameInfo();
    p0Info = myClient->getPlayerInfo(0);
    p1Info = myClient->getPlayerInfo(1);
    cout << "Wait.. I did it??" << gameInfo.at(0) << gameInfo.at(1) << gameInfo.at(2) << endl;
    cout << "p0 info: " << p0Info.at(0) << p0Info.at(1) << p0Info.at(2) << endl;
    cout << "p1 info: " << p1Info.at(0) << p1Info.at(1) << p1Info.at(2) << endl;
    bool isHmn = (p0Info.at(1) != 0);
    bool isAHmn = (p1Info.at(1) != 0);
    Player * human = new Player(0, p0Info.at(0),  isHmn, intToCard(p0Info.at(2)));
    Player * AI = new Player(1, p1Info.at(0),  isAHmn, intToCard(p1Info.at(2)));
    //Player* players[2];
    players[0] = human;
    players[1] = AI;
    dealerPos = gameInfo.at(0);
    pTurn = gameInfo.at(1);
    phase = gameInfo.at(2);
    /* FOR RECOVERING HANDS: NEED TO FIX VECTOR ERROR, PROBABLY FROM RETURNING EMPTY VECTOR
    vector<Card*> t;
    players[0]->scoreHand = t;
    players[0]->hand = intsToCards(myClient->readCards(0,0));
    players[1]->hand = intsToCards(myClient->readCards(1,0));
    players[0]->scoreHand = intsToCards(myClient->readCards(0,1));
    players[1]->scoreHand = intsToCards(myClient->readCards(1,1));
    crib = intsToCards(myClient->readCards(2,2));
    for(int i = 0; i < players[0]->hand.size(); i++){
        cout << players[0]->hand.at(i)->getPriority() << endl;
    }
    */

    //HAVE TO GETCARDS FOR CRIB, PLAYER HANDS, ETC

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
    initializeGame();
    //ShowCribbage * display = new ShowCribbage();
    //phase = 0;
    phase = 1;
    while(players[0]->getPoints() < WINNING && players[1]->getPoints() < WINNING){
        if(phase == 1) phaseI();
        if(phase == 2) phaseII();
        if(phase == 3) phaseIII();
    }

    return 0;
}

