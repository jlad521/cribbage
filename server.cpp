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
#include <string.h>
#include "Player.h"
using namespace std;

//GLOBAL DECLARATOINS:
Player* players[2];
int dealerPos, pTurn;
vector<Card*> crib;
Card* cut;

sqlite3 *db;
char *zErrMsg = 0;
int rc;

static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
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

vector<Card*> sortHand(vector<Card*> v){
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

int JackSameSuit(vector<Card*> v, Card* cut){ // 1 pt if player's card is Jack and same suit as starter card
    int i;

    for(i = 0; i < v.size(); i++){
        if(v.at(i)->getFace() == 'J' && v.at(i)->getSuit() == cut->getSuit()){
            return (1);
        }
    }
    return(0);
}


int Pairs(vector<Card*> v){ // 2 pts per pair (3 of a kind - 3 pairs; 4 of a kind - 6 pairs)
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


int ThirtyOne(vector<Card*> v){ // 2 pts for reaching 31 exactly
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

int Fifteen(vector<Card*> v){  // 2 pts per combination that equals fifteen
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


int RunScore(vector<Card*> v){ // n points per card in run
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



int FlushScore(vector<Card*> v){ // 4 points for 4 cards in player's hand all of same suit
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


int PairsGo(vector<Card*> v){
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


int RunScoreGo(vector<Card*> v){
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

int goPoints(vector<Card*> v){
    int pts = 0;
    pts += PairsGo(v);
    pts += RunScoreGo(v);
    return pts;
}

int AIGoChoice(Player* p, int goNum){
    for(int i = 0; i < p->hand.size(); i++){
        if(p->hand.at(i)->getValue() + goNum < 31) {return i;}
    }
    return -1; //return "go" if can't play valid card
}

int scorePhase(vector<Card*> hand, Card* cut){
    int points = 0;
    points += Pairs(hand);
    points += JackSameSuit(hand, cut);
    points += Fifteen(hand);
    points += RunScore(hand);
    points += FlushScore(hand);
    //cout << "IT FOUND: " << points;
    return points;
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

class writeNewGame : public xmlrpc_c::method {
    public:
        writeNewGame(){}
        void execute(xmlrpc_c::paramList const& paramList, const xmlrpc_c::callInfo * const callInfoP, xmlrpc_c::value* returnP){
            *returnP = xmlrpc_c::value_int(5);
        }

};

class readHardState : public xmlrpc_c::method {
    public:
        readHardState(){}

        void execute(xmlrpc_c::paramList const& paramList, const xmlrpc_c::callInfo * const callInfoP, xmlrpc_c::value* returnP){
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
            //DEFINITELY WRONG:
            sql = "SELECT OBJECT_VALUE from GAME WHERE OBJECT_NAME='player1'";
            rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
            if ( rc != SQLITE_OK){
                fprintf(stderr, "SQL error: %s\n", zErrMsg);
                sqlite3_free(zErrMsg);
            }
            else{
                fprintf(stdout, "Operation done successfully.\n");
            }

            *returnP = xmlrpc_c::value_int(5);
        }
};
/*
vector<int> getCards(int pIndex, int contxt){
    vector<int> cardInfo;
    char pOwner = (char)(((int)'0') + pIndex);
    char context = (char)(((int)'0') + contxt);
    sqlite3_stmt *stmt;
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char* sql;
    string sqlQuery;
    const char* data = "Callback function called";
    rc = sqlite3_open("test.db", &db);
    if( rc ){ // opens DB
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    sqlQuery = "SELECT points, isHuman, lastPlayed FROM PLAYERS WHERE playerID = ";
    sqlQuery += ID;
    sqlQuery += ';';
    sql = new char[sqlQuery.length() + 1];
    strcpy(sql, sqlQuery.c_str());
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if( rc != SQLITE_OK){
        fprintf(stderr, "SELECT FAILED:%s\n ", sqlite3_errmsg(db));
    }
    while(sqlite3_step(stmt) == SQLITE_ROW){
        cardInfo.push_back(sqlite3_column_int(stmt,0));
        cardInfo.push_back(sqlite3_column_int(stmt,1));
        cardInfo.push_back(sqlite3_column_int(stmt,2));
    }
    sqlite3_finalize(stmt);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        fprintf(stdout, "Operation done successfully\n");
    }
    return cardInfo;
}
*/
//string toSqlWord()
void writeCards(int pIndex, int context, vector<int> cards){
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    const char* sql;
    const char* data = "Callback function called";
    rc = sqlite3_open("test.db", &db);
    if( rc ){ // opens DB
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    string currentCard;
    char buffer[20];
    for(int i = 0; i<cards.size(); i++){
        sprintf(buffer, "%i, %i, %i", pIndex, context, cards.at(i));
        //WORK ON CONCATINATING STRING:::::
        //currentCard = "INSERT INTO HANDS(cardID, pIndex, context)" \
                      "VALUES ( " + buffer + ");" ;
        sql = currentCard.c_str();
        //"VALUES (1, 'player1', 'Player', '" + players[0]->serialize() + "');";
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
        if(rc!= SQLITE_OK ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        else{
            fprintf(stdout, "Records created successfully\n");
        }
    }
}

vector<int> getPlayer(int IDent){
    vector<int> playerInfo;
    char ID = (char)(((int)'0') + IDent);
    sqlite3_stmt *stmt;
    sqlite3 *db;
    char *zErrMsg = 0;
    int rc;
    char* sql;
    string sqlTest;
    const char* data = "Callback function called";
    rc = sqlite3_open("test.db", &db);
    if( rc ){ // opens DB
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    sqlTest = "SELECT points, isHuman, lastPlayed FROM PLAYERS WHERE playerID = ";
    sqlTest += ID;
    sqlTest += ';';
    sql = new char[sqlTest.length() + 1];
    strcpy(sql, sqlTest.c_str());
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if( rc != SQLITE_OK){
        fprintf(stderr, "SELECT FAILED:%s\n ", sqlite3_errmsg(db));
    }
    while(sqlite3_step(stmt) == SQLITE_ROW){
        playerInfo.push_back(sqlite3_column_int(stmt,0));
        playerInfo.push_back(sqlite3_column_int(stmt,1));
        playerInfo.push_back(sqlite3_column_int(stmt,2));
    }
    sqlite3_finalize(stmt);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        fprintf(stdout, "Operation done successfully\n");
    }
    return playerInfo;
}

//makeGame: Initializes game objects, creates database tables
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
class phaseIIturn : public xmlrpc_c::method {
    public:
        phaseIIturn(){}
        void
            execute(xmlrpc_c::paramList const& paramList,
                    xmlrpc_c::value* returnP) {
                int goNum, index, played, lastPlayed; //from dB
            }
};

class prepPII : public xmlrpc_c::method {
    public:
        prepPII(){}
        void
            execute(xmlrpc_c::paramList const& paramList,
                    xmlrpc_c::value* returnP) {
                players[0]->scoreHand = players[0]->hand;
                players[1]->scoreHand = players[1]->hand;
                int phase = 2;
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
                srand(time(NULL));
                getPlayer(0);

                Deck * deck = new Deck();
                Card * cut;
                players[0]->hand = deck->dealCards();
                //writeHand(pIndex,vector<int>)
                //players[0]->scoreHand = players[0]->hand;
                players[1]->hand = deck->dealCards();
                //players[1]->scoreHand = players[1]->hand;
                cut = deck->cutDeck(); //SHOULD I DO THIS HERE, OR P2??
                vector<xmlrpc_c::value> cardData;
                vector<int> converted = cardsToInts(players[0]->hand);
                vector<int> aiConvert = cardsToInts(players[1]->hand);
                for(int i = 0; i < converted.size(); i++){
                    cardData.push_back(xmlrpc_c::value_int(converted.at(i)));
                }
                for(int k = 0; k < converted.size(); k++){
                    cardData.push_back(xmlrpc_c::value_int(converted.at(k)));
                }
                cardData.push_back(xmlrpc_c::value_int(cardToInt(cut)));
                xmlrpc_c::value_array array(cardData);
                *returnP = array;
                delete deck;
                //now have to write to dB
                //--> need to create hands table
            }

};

class getPlayerInfo : public xmlrpc_c::method {
    public:
        getPlayerInfo(){}
        void
            execute(xmlrpc_c::paramList const& paramList,
                    xmlrpc_c::value* returnP) {
                int const pID(paramList.getInt(0));
                vector<int> playerInfo;
                playerInfo = getPlayer(pID);
                vector<xmlrpc_c::value> gameData;
                gameData.push_back(xmlrpc_c::value_int(playerInfo.at(0)));
                gameData.push_back(xmlrpc_c::value_int(playerInfo.at(1)));
                gameData.push_back(xmlrpc_c::value_int(playerInfo.at(2)));
                xmlrpc_c::value_array RAY(gameData);
                *returnP = RAY;
            }
};
//will return everything but cards
class getGameInfo : public xmlrpc_c::method {
    public:
        getGameInfo(){}
        void
            execute(xmlrpc_c::paramList const& paramList,
                    xmlrpc_c::value* returnP) {
                sqlite3_stmt *stmt;
                sqlite3 *db;
                char *zErrMsg = 0;
                int rc;
                const char* sqlQuery;
                const char* data = "Callback function called";
                rc = sqlite3_open("test.db", &db);
                if( rc ){ // opens DB
                    fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
                    exit(0);
                }else{
                    fprintf(stderr, "Opened database successfully\n");
                }

                sqlQuery = "SELECT dealerPos, pTurn, phase FROM GAME;";
                rc = sqlite3_prepare_v2(db, sqlQuery, -1, &stmt, NULL);
                if( rc != SQLITE_OK){
                    fprintf(stderr, "SELECT FAILED:%s\n ", sqlite3_errmsg(db));
                }
                int dealerPos, pTurn, phase; //Literally, what the fuck..
                while(sqlite3_step(stmt) == SQLITE_ROW){
                    dealerPos = sqlite3_column_int(stmt,0);
                    pTurn  = sqlite3_column_int(stmt,1);
                    phase = sqlite3_column_int(stmt,2);
                }
                sqlite3_finalize(stmt);
                vector<xmlrpc_c::value> gameData;
                gameData.push_back(xmlrpc_c::value_int(dealerPos));
                gameData.push_back(xmlrpc_c::value_int(pTurn));
                gameData.push_back(xmlrpc_c::value_int(phase));
                xmlrpc_c::value_array RAY(gameData);
                *returnP = RAY;
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
       if(!existingGame){
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
    }
    */

    try {
        xmlrpc_c::registry myRegistry;
        xmlrpc_c::methodPtr const getPlayerInfoP(new getPlayerInfo);
        myRegistry.addMethod("getPlayerInfo", getPlayerInfoP);
        xmlrpc_c::methodPtr const phaseIP(new phaseI);
        myRegistry.addMethod("server.phaseI", phaseIP);
        xmlrpc_c::methodPtr const tellDiscardP(new tellDiscard);
        myRegistry.addMethod("tellDiscard", tellDiscardP);
        xmlrpc_c::methodPtr const getGameInfoP(new getGameInfo);
        myRegistry.addMethod("getGameInfo", getGameInfoP);

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

/*
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
}
sql = "CREATE TABLE HANDS("  \
"handID INT PRIMARY KEY AUTOINCREMENT NOT NULL," \
"cardID INT, pIndex INT, context INT);"; //NEED TO ADD SCOREHAND AND HAND
rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
if( rc != SQLITE_OK ){
fprintf(stderr, "SQL error: %s\n", zErrMsg);
sqlite3_free(zErrMsg);
}else{
fprintf(stdout, "Table created successfully\n");
}
sql = "CREATE TABLE PLAYERS("
"playersID INT PRIMARY KEY     NOT NULL, "
"points INT NOT NULL, isHuman BOOLEAN, name TEXT, lastPlayed INT,"
"hand INT, FOREIGN KEY(hand) REFERENCES hand(handID), "
"scoreHand INT, FOREIGN KEY(scoreHand) REFERENCES hand(handID));"; //NEED TO ADD SCOREHAND AND HAND
rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
if( rc != SQLITE_OK ){
fprintf(stderr, "SQL error: %s\n", zErrMsg);
sqlite3_free(zErrMsg);
}else{
fprintf(stdout, "Table created successfully\n");
}
//THINK ABOUT WHAT THESE FIELDS SHOULD BE
sql = "CREATE TABLE GAME("
"ID INT PRIMARY KEY AUTOINCREMENT    NOT NULL, "   //UPDATE TABLE ARGUMENTS IF THEY DON'T MATCH HOW YOU SERIALIZE
"dealerPos INT NOT NULL, phase INT, player0 INT, FOREIGN KEY (player0) REFERENCES PLAYERS(playersID), "
"player1 INT, FOREIGN KEY (player1) REFERENCES PLAYERS(playersID), "
"crib INT, FOREIGN KEY(crib) REFERENCES hands(handsID));"; //NEED TO ADD SCOREHAND AND HAND }
rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
if( rc != SQLITE_OK ){
fprintf(stderr, "SQL error: %s\n", zErrMsg);
sqlite3_free(zErrMsg);
}else{
fprintf(stdout, "Table created successfully\n");
}

sql = "INSERT INTO GAME (ID, dealerPos, phase, player0, player1, crib) " \
"VALUES (NULL, 0, 1, 0, 1, 2);" \
"INSERT INTO PLAYERS (playersID, points, isHuman, name, lastPlayed, hand, scoreHand)"\
"VALUES (0, 0, true, 'Jesus', 0, 0, 1);" \
"INSERT INTO PLAYERS (playersID, points, isHuman, name, lastPlayed, hand, scoreHand)"\
"VALUES (1, 0, false, 'ROBOT', 0, 0, 1);";
rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
if( rc != SQLITE_OK ){
fprintf(stderr, "SQL error: %s\n", zErrMsg);
sqlite3_free(zErrMsg);
}else{
fprintf(stdout, "Wrote initial game state successfully\n");
}
}
*/
