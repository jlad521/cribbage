#include <cstdlib>
#include <cassert>
#include <stdexcept>
#include <iostream>
#include <ctime>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include "xmlrpc-c/base.hpp"
#include "xmlrpc-c/registry.hpp"
#include "xmlrpc-c/server_abyss.hpp"
#include "Deck.h"
#include <string>
#include "Player.h"
using namespace std;

//GLOBAL DECLARATOINS:
Player* players[2];
int dealerPos, pTurn;
vector<Card*> crib;
Card* cut;

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

void log_sql(int random)
{
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    const char* sql;
    const char* data = "Callback function called";

    rc = sqlite3_open("test.db", &db);

    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }

    sql = "CREATE TABLE GAME("  \
           "ID INT PRIMARY KEY     NOT NULL," \
           "NAME           TEXT    NOT NULL);";

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        fprintf(stdout, "Table created successfully\n");
    }

    sql = "INSERT INTO GAME (ID,NAME) "  \
           "VALUES (1, 'field1=5;field2=6;field3=7'); ";

    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        fprintf(stdout, "Records created successfully\n");
    }

    sql = "SELECT * from GAME";
    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        fprintf(stdout, "Operation done successfully\n");
    }

    sql = "UPDATE GAME set NAME = 'hello' where ID=1; " \
           "SELECT * from GAME";

    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        fprintf(stdout, "Operation done successfully\n");
    }

    sql = "DELETE from GAME where ID=1; " \
           "SELECT * from GAME";

    rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        fprintf(stdout, "Operation done successfully\n");
    }

    sqlite3_close(db);
}

class sampleRandMethod : public xmlrpc_c::method {
    public:
        sampleRandMethod() {}
        void
            execute(xmlrpc_c::paramList const& paramList,
                    xmlrpc_c::value *   const  retvalP) {

                int random = rand();
                log_sql(random);
                *retvalP = xmlrpc_c::value_int(random);
            }
};

class getGameInfo : public xmlrpc_c::method {
    public:
        getGameInfo() {}
        void
            execute(xmlrpc_c::paramList const& paramList,
                    xmlrpc_c::value *   const  retvalP) {

            }
};

class getPlayerInfo : public xmlrpc_c::method {
    public:
        getPlayerInfo () {
            //initialize signature/help?
        }
        void
            execute(xmlrpc_c::paramList const& paramList,
                    //const xmlrpc_c::callInfo * const callInfoP,
                    xmlrpc_c::value* returnP) {
                Player* AI = new Player(false, "ROBOT");
                Player* human = new Player(true, "Jesus");
                sqlite3 *db;
                char *zErrMsg = 0;
                int rc;
                const char* sql;
                const char* data = "Callback function called";
                rc = sqlite3_open("test.db", &db);
                if( rc ){
                    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
                    exit(0);
                }else{
                    fprintf(stderr, "Opened database successfully\n");
                    sql = "CREATE TABLE PLAYERS("  \
                           "ID INT PRIMARY KEY     NOT NULL," \
                           "position INT, points INT, isHuman INT, name VARCHAR(50), lastPlayed INT);"; //NEED TO ADD SCOREHAND AND HAND
                }
                rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
                if( rc != SQLITE_OK ){
                    fprintf(stderr, "SQL error: %s\n", zErrMsg);
                    sqlite3_free(zErrMsg);
                }else{
                    fprintf(stdout, "Table created successfully\n");
                }
                //HOW TO INSERT PLAYER1 IN SECOND ROW???
                //I could just read the dB from client to return this info...
                sql = "INSERT INTO PLAYERS (position, points, isHuman, name, lastPlayed) "  \
                //USE SERIALIZE FUNCTION IN VALUES COMMAND
                       "VALUES (1, 'field1=position;field2=points;field3=isHuman;field4=name;field5=lastPlayed');";

                rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
                if( rc != SQLITE_OK ){
                    fprintf(stderr, "SQL error: %s\n", zErrMsg);
                    sqlite3_free(zErrMsg);
                }else{
                    fprintf(stdout, "Records created successfully\n");
                }


                int k = 5;
                vector<xmlrpc_c::value> array;
                array.push_back(xmlrpc_c::value_int(5));
                *returnP = xmlrpc_c::value_int(5);
                //*returnP = &array;
                //xmlrpc_c::value *returnP(array);
                //*returnP->push_back(xmlrpc_c::value_int(k));
                //*returnP = xmlrpc_c::value_int(k);
            }
};
//makeGame: Initializes game objects, creates database tables
void makeGame(){
    //PLAYERS ALREADY INITIALIZED, SO DON'T DO IT AGAIN IN MAIN
    Player * AI = new Player(false, "ROBOT");
    Player * human = new Player(true, "Jesus");
    players[0] = human;
    players[1] = AI;
    dealerPos = 0;
    pTurn = 1;

    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    const char* sql;
    const char* data = "Callback function called";
    rc = sqlite3_open("test.db", &db);
    if( rc ){
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
        sql = "CREATE TABLE GAME("
            "ID INT PRIMARY KEY     NOT NULL,"   //UPDATE TABLE ARGUMENTS IF THEY DON'T MATCH HOW YOU SERIALIZE
            "position INT, points INT, isHuman INT, name VARCHAR(50), lastPlayed INT);"; //NEED TO ADD SCOREHAND AND HAND }
}
rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
if( rc != SQLITE_OK ){
    fprintf(stderr, "SQL error: %s\n", zErrMsg);
    sqlite3_free(zErrMsg);
}else{
    fprintf(stdout, "Table created successfully\n");
}
//MAKE PLAYERS TABLE HERE

}

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
class AIDiscards : public xmlrpc_c::method {

    public:
        AIDiscards(){}
        void
            execute(xmlrpc_c::paramList const& paramList,
                    xmlrpc_c::value* returnP) {
                crib.push_back(players[1]->hand.back());
                players[1]->hand.pop_back();
                crib.push_back(players[1]->hand.back());
                players[1]->hand.pop_back();
                *returnP = xmlrpc_c::value_int(5);
            }
};

class tellDiscard : public xmlrpc_c::method {
    public:
        tellDiscard(){}
        void
            execute(xmlrpc_c::paramList const& paramList,
                    xmlrpc_c::value* returnP) {
                int const pIndex(paramList.getInt(0));
                int const cIndex(paramList.getInt(1));
                int const cardNum(paramList.getInt(2));
                paramList.verifyEnd(3);
                cout << "pIndex: " << pIndex << " cIndex: " <<cIndex << " cardNum: " <<cardNum << endl;
                crib.push_back(intToCard(cardNum));
                for(int k = 0; k < players[pIndex]->hand.size(); k++){
                    cout << "player cards before: " << players[pIndex]->hand.at(k)->getPriority() << " " <<endl;
                }
                players[pIndex]->hand.erase(players[pIndex]->hand.begin()+cIndex);
                cout << " --------------------------------- " << endl;

                for(int i = 0; i < players[pIndex]->hand.size(); i++){
                    cout << "player cards before: " << players[pIndex]->hand.at(i)->getPriority() << " " <<endl;
                }
                *returnP = xmlrpc_c::value_int(5);
            }
};

class phaseI : public xmlrpc_c::method {
    public:
        phaseI(){}
        void
            execute(xmlrpc_c::paramList const& paramList,
                    xmlrpc_c::value* returnP) {
                Deck * deck = new Deck();
                Card * cut;
                players[0]->hand = deck->dealCards();
                //players[0]->scoreHand = players[0]->hand;
                players[1]->hand = deck->dealCards();
                //players[1]->scoreHand = players[1]->hand;
                cut = deck->cutDeck(); //SHOULD I DO THIS HERE, OR P2??
                vector<xmlrpc_c::value> cardData;
                vector<int> converted = cardsToInts(players[0]->hand);
                for(int i = 0; i < converted.size(); i++){
                    cardData.push_back(xmlrpc_c::value_int(converted.at(i)));
                }
                xmlrpc_c::value_array array(cardData);
                *returnP = array;
                delete deck;
                //now have to write to dB
                //--> need to create hands table
            }

};


//NEED TO DOUBLE CHECK THIS IS HOW TO CHECK IF DATABASE IS EMPTY
bool existingGame(){
    sqlite3 *dB;
    int rc;
    rc = sqlite3_open("test.db", &dB);
    if( rc) {
        return false;
    }
    return true;
}

int
main(int           const,
        const char ** const) {
    /*
       if(existingGame){
    //readValues from dB and reinitialize values
    // Stub for now, but will read from dB to get these values
    Card* humanLPstub = new Card(1,4,1);
    Card* AILPstub = new Card(13,4,10);

    players[0] = new Player(0, 13, true, "Jesus", humanLPstub);
    players[1] = new Player(1, 21, false, "ROBOT", AILPstub);
    dealerPos = 1; //continue initializing variables
    }
    else{
    //TABLES CREATED IN MAKE GAME; MAKE THEM CORRECT
    makeGame(); //creates tables, initializes game objects
    }
    */
    makeGame(); //creates tables, initializes game objects

    /* CALL YOUR WRITE AND READ METHODS HERE!
     * ./compile, then ./server to test
     */
    int failed = 14;
    Card* test = new Card(14, 1, 1);
    int letsseee = cardToInt(test);
    Card* ahh = intToCard(failed);
    cout << "value: " << letsseee << "Card: " << ahh->getPriority();

    try {
        xmlrpc_c::registry myRegistry;
        xmlrpc_c::methodPtr const sampleRandMethodP(new sampleRandMethod);
        myRegistry.addMethod("sample.rand", sampleRandMethodP);
        /* Now add my Method */
        xmlrpc_c::methodPtr const getPlayerInfoP(new getPlayerInfo);
        myRegistry.addMethod("define_players", getPlayerInfoP);

        xmlrpc_c::methodPtr const phaseIP(new phaseI);
        myRegistry.addMethod("server.phaseI", phaseIP);

        xmlrpc_c::methodPtr const tellDiscardP(new tellDiscard);
        myRegistry.addMethod("tellDiscard", tellDiscardP);
        //xmlrpc_c::methodPtr const getGameInfoP(new getGameInfo);
        //myRegistry.addMethod("getGameInfo", getGameInfoP);

        xmlrpc_c::serverAbyss myAbyssServer(
                xmlrpc_c::serverAbyss::constrOpt()
                .registryP(&myRegistry)
                .portNumber(8080));
        srand(time(NULL));
        myAbyssServer.run();
        assert(false);
    } catch (exception const& e) {
        cerr << "Something failed.  " << e.what() << endl;
    }
    return 0;
}

/*
   sql = "INSERT INTO GAME () "  \
          //USE SERIALIZE FUNCTION IN VALUES COMMAND
          "VALUES (1, serialize);";

          rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
          if( rc != SQLITE_OK ){
          fprintf(stderr, "SQL error: %s\n", zErrMsg);
          sqlite3_free(zErrMsg);
          }else{
          fprintf(stdout, "Records created successfully\n");
          */
