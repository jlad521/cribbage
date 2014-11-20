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
//#include "Cribbage.h"
//#include "Card.h"
#include "Player.h"
#include "Deck.h"
#include "ShowCribbage.h"
#define WINNING 25
//#include "client.h"
class clientConnection{
    private:
        static bool instanceFlag;
        static clientConnection* an_instance;
        clientConnection(){};
    public:
        void phaseI();
        void tellDiscard(int, int, int, int);
        vector<int> getGameInfo();
        vector<int> getPlayerInfo(int);
        vector<int> readCards(int, int);
        vector<int> phaseIII();
        int prepPII();
        int getCutCard();
        void prepPIII();
        void AIDiscards();
        void nextRound();
        void phaseIITurn(int, int);
        int AITurn();
        vector<int> pIIinfo();
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

void clientConnection::nextRound(){
    try{
        xmlrpc_c::clientSimple myClient;// = new(xmlrpc_c::clientSimple);
        xmlrpc_c::value result;
        string const serverUrl("http://localhost:8080/RPC2");
        string const nextRound("nextRound");
        myClient.call(serverUrl, nextRound, &result);
    } catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
        sleep(6000);
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
        sleep(6000);
    }
}

int clientConnection::getCutCard(){
    int card;
    try{
        xmlrpc_c::clientSimple myClient;// = new(xmlrpc_c::clientSimple);
        xmlrpc_c::value result;
        string const serverUrl("http://localhost:8080/RPC2");
        string const getCutCard("getCutCard");
        myClient.call(serverUrl, getCutCard, &result);
        card =xmlrpc_c::value_int(result);
    } catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
        sleep(6000);
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
        sleep(6000);
    }
    return card;
}

void clientConnection::prepPIII(){
    try{
        xmlrpc_c::clientSimple myClient;// = new(xmlrpc_c::clientSimple);
        xmlrpc_c::value result;
        string const serverUrl("http://localhost:8080/RPC2");
        string const prepPIII("prepPIII");
        myClient.call(serverUrl, prepPIII, &result);
    } catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
        sleep(6000);
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
        sleep(6000);
    }
}
void clientConnection::phaseIITurn(int pIndex, int cardNum){
    try{
        xmlrpc_c::clientSimple myClient;// = new(xmlrpc_c::clientSimple);
        xmlrpc_c::value result;
        string const serverUrl("http://localhost:8080/RPC2");
        string const phaseIITurn("phaseIITurn");
        myClient.call(serverUrl, phaseIITurn, "ii", &result, pIndex, cardNum);
    } catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
        sleep(6000);
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
        sleep(6000);
    }
}
void clientConnection::tellDiscard(int pIndex, int cIndex, int cardNum, int phase){
    try{
        xmlrpc_c::clientSimple myClient;// = new(xmlrpc_c::clientSimple);
        xmlrpc_c::value nothing;
        string const serverUrl("http://localhost:8080/RPC2");
        string const tellDiscard("tellDiscard");
        myClient.call(serverUrl, tellDiscard, "iiii", &nothing, pIndex, cIndex, cardNum, phase);
    } catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
        sleep(6000);
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
        sleep(6000);
    }
}

void clientConnection::AIDiscards(){
    try{
        xmlrpc_c::clientSimple myClient;// = new(xmlrpc_c::clientSimple);
        xmlrpc_c::value nothing;
        string const serverUrl("http://localhost:8080/RPC2");
        string const AIDiscards("AIDiscards");
        myClient.call(serverUrl, AIDiscards, &nothing);
    } catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
        sleep(6000);
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
        sleep(6000);
    }
}
int clientConnection::AITurn(){
    int index;
    try{
        xmlrpc_c::clientSimple myClient;// = new(xmlrpc_c::clientSimple);
        xmlrpc_c::value nothing;
        string const serverUrl("http://localhost:8080/RPC2");
        string const AITurn("AITurn");
        myClient.call(serverUrl, AITurn, &nothing);
        index = xmlrpc_c::value_int(nothing);
    } catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
        sleep(6000);
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
        sleep(6000);
    }
    return index;
}
vector<int> clientConnection::pIIinfo(){
    vector<int> inf;
    try{
        xmlrpc_c::clientSimple myClient;// = new(xmlrpc_c::clientSimple);
        xmlrpc_c::value number;
        string const serverUrl("http://localhost:8080/RPC2");
        string const pIIinfo("pIIinfo");
        myClient.call(serverUrl, pIIinfo, &number);
        xmlrpc_c::value_array arrr(number);
        vector<xmlrpc_c::value> const gameInfo(arrr.vectorValueValue());
        inf.push_back(xmlrpc_c::value_int(gameInfo.at(0)));
        inf.push_back(xmlrpc_c::value_int(gameInfo.at(1)));
        inf.push_back(xmlrpc_c::value_int(gameInfo.at(2)));
        inf.push_back(xmlrpc_c::value_int(gameInfo.at(3)));
    } catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
        sleep(6000);
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
        sleep(6000);
    }
    return inf;
}
int clientConnection::prepPII(){
    int card;
    try{
        xmlrpc_c::clientSimple myClient;// = new(xmlrpc_c::clientSimple);
        xmlrpc_c::value nothing;
        string const serverUrl("http://localhost:8080/RPC2");
        string const prepPII("prepPII");
        myClient.call(serverUrl, prepPII, &nothing);
        card = xmlrpc_c::value_int(nothing);
    } catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
        sleep(6000);
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
        sleep(6000);
    }
    return card;
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
        info.push_back(xmlrpc_c::value_int(gameInfo.at(3)));

    } catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
        sleep(6000);
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
        sleep(6000);
    }
    return info;
}

vector<int> clientConnection::phaseIII(){
    vector<int> info;
    try{
        xmlrpc_c::clientSimple myClient;// = new(xmlrpc_c::clientSimple);
        xmlrpc_c::value value;
        string const serverUrl("http://localhost:8080/RPC2");
        string const phaseIII("phaseIII");
        myClient.call(serverUrl, phaseIII, &value);
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
int dealerPos, pTurn, goPhaseNumber, phase; // I'm sure there'll be more
clientConnection *myClient;
Card* cut = new Card();
Card* blank = new Card();
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
        if(priority == 0) {
            priority = 13;
            value = 10;
        }
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
    if(priority == 0) {
        priority = 13;
        value = 10;
    }
    convertedCard = new Card(priority, suit, value);
    return convertedCard;
}

void phaseIII(ShowCribbage * display){
    vector<int> scoreInfo;
    scoreInfo = myClient->phaseIII();
    int pScore = scoreInfo.at(0);
    int AIscore = scoreInfo.at(1);
    int hasScored = scoreInfo.at(2);
    if(hasScored == 0){
    players[0]->addPoints(pScore);
    players[1]->addPoints(AIscore);
    }
    players[0]->scoreHand = intsToCards(myClient->readCards(0,1));
    players[1]->scoreHand = intsToCards(myClient->readCards(1,1));
    crib = intsToCards(myClient->readCards(2,2));
    display->drawPIII(players, crib, pScore, AIscore);
    display->spinWait();
    phase = 1;
    crib.erase(crib.begin(), crib.begin()+crib.size());
    players[0]->lastCard = blank;
    players[1]->lastCard = blank;
    dealerPos = (dealerPos +1) %2;
    cut = blank;
    myClient->nextRound();
}

void phaseII(ShowCribbage * display){
    vector<int> p2info;
    int aiIndex;
    int cardNum;
    vector<int> updatedP2info;
    vector<int> lastUpdate;
    vector<int> update;
    update = myClient->pIIinfo();
    goPhaseNumber = update.at(0);
    display->drawPII(players, cut, 2, goPhaseNumber, true, dealerPos);
    while(!players[0]->hand.empty() || !players[1]->hand.empty()){
        p2info = myClient->pIIinfo();
        goPhaseNumber = p2info.at(0);
        pTurn = p2info.at(1);
        if(pTurn == 0){
            int index = display->getCard(players[0], 2, goPhaseNumber);
            if(index > -1){
                myClient->phaseIITurn(pTurn, cardToInt(players[pTurn]->hand.at(index)));
                players[pTurn]->lastCard = players[pTurn]->hand.at(index); //
                players[pTurn]->hand.erase(players[pTurn]->hand.begin()+index);
            }
            if(index == -1){ myClient->phaseIITurn(pTurn, -1);}
        }
        else{
            aiIndex = myClient->AITurn();
            if(aiIndex > -1){
                myClient->phaseIITurn(pTurn, cardToInt(players[pTurn]->hand.at(aiIndex)));
                players[pTurn]->lastCard = players[1]->hand.at(aiIndex);
                players[pTurn]->hand.erase(players[pTurn]->hand.begin()+aiIndex);
            }
            if (aiIndex == -1) {myClient->phaseIITurn(pTurn, -1);}
        }
        updatedP2info = myClient->pIIinfo();
        players[0]->setPoints(updatedP2info.at(2));
        players[1]->setPoints(updatedP2info.at(3));
        display->drawPII(players, cut, 2, updatedP2info.at(0), true, dealerPos);
    }
    myClient->prepPIII();
    updatedP2info = myClient->pIIinfo();
    players[0]->setPoints(updatedP2info.at(2));
    players[1]->setPoints(updatedP2info.at(3));
    display->drawPII(players, cut, 2, updatedP2info.at(0), true, dealerPos);
    phase = 3;
}

void phaseI(ShowCribbage* display){
    if(players[0]->hand.empty()){
    myClient->phaseI(); //deals cards to players
    players[0]->hand = intsToCards(myClient->readCards(0,0));
    players[1]->hand = intsToCards(myClient->readCards(1,0));
    }
    if(players[0]->hand.size() == 6){
    display->drawPII(players, cut, 1, 0, false, dealerPos);
    }
    else{
    display->drawPII(players, cut, 1, 0, true, dealerPos);
    }
    int gophase = 0;
    int selected;

    /*Discard human cards */
    while(players[0]->hand.size() > 4){
        selected = display->getCard(players[0], 1, 0); //getCard only allows a valid card to be played (play game to see)
        crib.push_back(players[0]->hand.at(selected)); //add discarded cards to crib
        myClient->tellDiscard(0, 0, cardToInt(players[0]->hand.at(selected)), 1); //add discarded cards to crib);
        players[0]->hand.erase(players[0]->hand.begin()+selected);
        display->drawPII(players, cut, 1, 0, true, dealerPos);
    }
    players[1]->hand.pop_back(); //remove card from hand
    players[1]->hand.pop_back(); //remove card from hand
    display->drawPII(players, cut, 1, 0, false, dealerPos);
    cut = intToCard(myClient->prepPII());
    phase = 2;
}

void initializeGame(){
    vector<int> gameInfo; //dealerPos, pTurn, phase
    vector<int> p0Info;
    vector<int> p1Info;
    gameInfo = myClient->getGameInfo();
    p0Info = myClient->getPlayerInfo(0);
    p1Info = myClient->getPlayerInfo(1);
    bool isHmn = (p0Info.at(1) == 1);
    bool isAHmn = (p1Info.at(1) == 1);
    Player * human = new Player(0, p0Info.at(0),  isHmn, intToCard(p0Info.at(2)));
    Player * AI = new Player(1, p1Info.at(0),  isAHmn, intToCard(p1Info.at(2)));
    players[0] = human;
    players[1] = AI;
    dealerPos = gameInfo.at(0);
    pTurn = gameInfo.at(1);
    phase = gameInfo.at(2);
    goPhaseNumber = gameInfo.at(3);
    vector<Card*> t;
    players[0]->scoreHand = t;
    players[0]->hand = intsToCards(myClient->readCards(0,0));
    players[1]->hand = intsToCards(myClient->readCards(1,0));
    if(phase == 2){cut = intToCard(myClient->getCutCard());}
}

int main(int argc, char* argv[]){
    myClient = clientConnection::createInstance();
    initializeGame();
    ShowCribbage * display = new ShowCribbage();
    while(players[0]->getPoints() < WINNING && players[1]->getPoints() < WINNING){
        if(phase == 1) phaseI(display);
        if(phase == 2) phaseII(display);
        if(phase == 3) phaseIII(display);
    }
    int winPos;
    if(players[0]->getPoints() > WINNING){winPos = 0; }
    else { winPos = 1;}
    display->displayWinner(players, winPos);
    delete players[0];
    delete players[1];
    delete display;
    return 0;
}

