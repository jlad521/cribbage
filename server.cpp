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

/*
class dbConnection{
    private:
        sqlite3 *database;
        int rc;
        char *zErrMsg;
        static bool instanceFlag;
        static dbConnection* an_instance;
        sqlite3* openDB();
        dbConnection();

    public:
        sqlite3* dbRef();
        vector<int> getPlayer(int);
        static dbConnection* createInstance();
        ~dbConnection() {
            sqlite3_close(db);
            fprintf(stdout, "closed connection");
            instanceFlag = false;
        }
};

bool dbConnection::instanceFlag = false;
dbConnection* dbConnection::an_instance = NULL;

dbConnection* dbConnection::createInstance(){
    if(!instanceFlag){
        an_instance = new dbConnection();
        instanceFlag = true;
        fprintf(stdout, "hellooooooooooooo");
        return an_instance;
    }
}

dbConnection::dbConnection(){
    fprintf(stdout, "dbAddress before: %p", database);
}

sqlite3* dbConnection::dbRef(){
    fprintf(stdout, "dbAddress after: %p", &database);
    return database;
}
vector<int> dbConnection::getPlayer(int id){
    fprintf(stdout, "dbAddress : %p", database);
    vector<int> playerInfo;
    sqlite3_stmt *stmt;
    const char* sql;
    const char* data = "Callback function called";
    char buffer[20];
    sprintf(buffer, "%i;", id);
    string sqlTest = "SELECT points, isHuman, lastPlayed FROM PLAYERS WHERE playerID = ";
    sqlTest.append(buffer);
    sql = sqlTest.c_str();
    rc = sqlite3_prepare_v2(database, sql, -1, &stmt, NULL);
    if( rc != SQLITE_OK){
        fprintf(stderr, "SELECT FAILED:%s\n ", sqlite3_errmsg(database));
    }
    while(sqlite3_step(stmt) == SQLITE_ROW){
        playerInfo.push_back(sqlite3_column_int(stmt,0));
        fprintf(stdout, "pInfo: %d", playerInfo.back());
        playerInfo.push_back(sqlite3_column_int(stmt,1));
        fprintf(stdout, "pInfo: %d", playerInfo.back());
        playerInfo.push_back(sqlite3_column_int(stmt,2));
        fprintf(stdout, "pInfo: %d", playerInfo.back());
    }
    sqlite3_finalize(stmt);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        fprintf(stdout, "GAME INFO returned scuccesueflly done successfully\n");
    }
    return playerInfo;
}
   void databaseConnection::writeCard(int pIndex, int context, int cardNum){
   string currentCard;
   char buffer[20];
   sprintf(buffer, "%i, %i, %i );", pIndex, context, cardID);
   currentCard = "INSERT INTO HANDS(cardID, pIndex, context) " \
   "VALUES ( ";
   currentCard.append(buffer);
   fprintf(stdout, "currentCardQuery: %s\n", currentCard.c_str());
   sql = currentCard.c_str();
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   if ( rc != SQLITE_OK){
   fprintf(stderr, "SQL error: %s\n", zErrMsg);
   sqlite3_free(zErrMsg);
   }
   }
   */
static int callback(void *NotUsed, int argc, char **argv, char **azColName){
    int i;
    for(i=0; i<argc; i++){
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}
//GLOBAL DECLARATOINS:
Player* players[2];
int dealerPos, pTurn;
vector<Card*> crib;
Card* cut;
//dbConnection* dbConnect;
char *zErrMsg;
int rc;
bool played, lastPlayed;
int hasScored;

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
    Card* convertedCard;
    Card* blank = new Card();
    if(card == 0) { return blank;}
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
    delete blank;
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
            if(v.at(i)->getPriority() == v.at(b)->getPriority()){
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

int AIGoChoice(vector<Card*> hand, int goNum){
    for(int i = 0; i < hand.size(); i++){
        if(hand.at(i)->getValue() + goNum < 31) {return i;}
    }
    return -1; //return "go" if can't play valid card
}

int scorePhase(vector<Card*> hand){
    int points = 0;
    points += Pairs(hand);
    points += Fifteen(hand);
    points += RunScore(hand);
    points += FlushScore(hand);
    return points;
}

vector<int> getCards(int pIndex, int contxt){
    sqlite3* db;
    rc = sqlite3_open("test.db", &db);
    if( rc ){ // opens DB
        fprintf(stderr, "Can't open db: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    vector<int> cardInfo;
    const char* sql;
    sqlite3_stmt* stmt;
    char buffer[20];
    string sqlQuery = "SELECT cardID FROM HANDS WHERE pIndex = ";
    sprintf(buffer, "%i AND context = %i;", pIndex, contxt);
    sqlQuery.append(buffer);
    fprintf(stdout, "readCards query: %s\n", sqlQuery.c_str());
    sql =  sqlQuery.c_str();
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if( rc != SQLITE_OK){
        fprintf(stderr, "SELECT FAILED:%s\n ", sqlite3_errmsg(db));
    }
    while(sqlite3_step(stmt) == SQLITE_ROW){
        cardInfo.push_back(sqlite3_column_int(stmt,0));
        fprintf(stdout, "cardRead: %d\n", cardInfo.back());
    }
    sqlite3_finalize(stmt);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        fprintf(stdout, "Operation done successfully\n");
    }
    sqlite3_close(db);
    return cardInfo;
}

int getCutCard(){
    sqlite3* db;
    rc = sqlite3_open("test.db", &db);
    if( rc ){ // opens DB
        fprintf(stderr, "Can't open db: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    int cutCard;
    const char* sql;
    sqlite3_stmt* stmt;
    char buffer[20];
    string sqlQuery = "SELECT cutCard FROM GAME;";
    sql =  sqlQuery.c_str();
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if( rc != SQLITE_OK){
        fprintf(stderr, "SELECT FAILED:%s\n ", sqlite3_errmsg(db));
    }
    while(sqlite3_step(stmt) == SQLITE_ROW){
        cutCard = sqlite3_column_int(stmt,0);
    }
    sqlite3_finalize(stmt);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        fprintf(stdout, "Operation done successfully\n");
    }
    sqlite3_close(db);
    return cutCard;
}

int getPlayerPoints(int pIndex){
    sqlite3* db;
    rc = sqlite3_open("test.db", &db);
    if( rc ){ // opens DB
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    int playerPts;
    sqlite3_stmt *stmt;
    const char* sql;
    string sqlQuery;
    const char* data = "Callback function called";
    char buffer[20];
    sprintf(buffer, "%i;", pIndex);
    sqlQuery = "SELECT points FROM PLAYERS WHERE playerID = ";
    sqlQuery.append(buffer);
    fprintf(stdout, "getPlayerPoints query: %s\n", sqlQuery.c_str());
    sql =  sqlQuery.c_str();
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if( rc != SQLITE_OK){
        fprintf(stderr, "SELECT FAILED:%s\n ", sqlite3_errmsg(db));
    }
    while(sqlite3_step(stmt) == SQLITE_ROW){
        playerPts = sqlite3_column_int(stmt,0);
        fprintf(stdout, "playerPoints read: %i\n", playerPts);
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return playerPts;
}

void updateScoreBool(int b){
    sqlite3* db;
    rc = sqlite3_open("test.db", &db);
    if( rc ){ // opens DB
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    const char* sql;
    const char* data = "Callback function called";
    char buffer[50];
    sprintf(buffer, "%i;", b);
    string currentCard = "UPDATE GAME SET scoreHand = ";
    currentCard.append(buffer);
    fprintf(stdout, "update player query: %s\n", currentCard.c_str());
    sql = currentCard.c_str();
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if(rc!= SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else{
        fprintf(stdout, "Records created successfully\n");
    }
    sqlite3_close(db);
}

void updateDealerPos(int p){
    sqlite3* db;
    rc = sqlite3_open("test.db", &db);
    if( rc ){ // opens DB
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    const char* sql;
    const char* data = "Callback function called";
    char buffer[50];
    sprintf(buffer, "%i;", p);
    string currentCard = "UPDATE GAME SET dealerPos = ";
    currentCard.append(buffer);
    fprintf(stdout, "update player query: %s\n", currentCard.c_str());
    sql = currentCard.c_str();
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if(rc!= SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else{
        fprintf(stdout, "Records created successfully\n");
    }
    sqlite3_close(db);
}

void updateLP(int pIndex, int card){
    sqlite3* db;
    rc = sqlite3_open("test.db", &db);
    if( rc ){ // opens DB
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    const char* sql;
    const char* data = "Callback function called";
    string currentCard;
    char buffer[50];
    sprintf(buffer, "%i WHERE playerID= %i;", card, pIndex);
    currentCard = "UPDATE PLAYERS SET lastPlayed = ";
    currentCard.append(buffer);
    fprintf(stdout, "update player query: %s\n", currentCard.c_str());
    sql = currentCard.c_str();
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if(rc!= SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else{
        fprintf(stdout, "Records created successfully\n");
    }
    sqlite3_close(db);
}

void updatePlayerPoints(int pIndex, int plusPoints){
    sqlite3* db;
    rc = sqlite3_open("test.db", &db);
    if( rc ){ // opens DB
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    const char* sql;
    const char* data = "Callback function called";
    int oldPts = getPlayerPoints(pIndex);
    int pts = oldPts + plusPoints;
    char buffer[50];
    sprintf(buffer, "%i WHERE playerID= %i;", pts, pIndex);
    string currentCard = "UPDATE PLAYERS SET points = ";
    currentCard.append(buffer);
    fprintf(stdout, "update playerPts query: %s\n", currentCard.c_str());
    sql = currentCard.c_str();
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if(rc!= SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else{
        fprintf(stdout, "Records created successfully\n");
    }
    sqlite3_close(db);
}

int getDealerPos(){
    sqlite3* db;
    rc = sqlite3_open("test.db", &db);
    if( rc ){ // opens DB
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    int dealerPos;
    sqlite3_stmt *stmt;
    const char* sql;
    string sqlQuery;
    const char* data = "Callback function called";
    sqlQuery = "SELECT dealerPos FROM GAME;";
    sql =  sqlQuery.c_str();
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if( rc != SQLITE_OK){
        fprintf(stderr, "SELECT FAILED:%s\n ", sqlite3_errmsg(db));
    }
    while(sqlite3_step(stmt) == SQLITE_ROW){
        dealerPos = sqlite3_column_int(stmt,0);
        fprintf(stdout, "dealerPos: %i\n", dealerPos);
    }
    sqlite3_finalize(stmt);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        fprintf(stdout, "Operation done successfully\n");
    }
    sqlite3_close(db);
    return dealerPos;
}
void updatePhase(int p){
    sqlite3* db;
    rc = sqlite3_open("test.db", &db);
    if( rc ){ // opens DB
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    const char* sql;
    const char* data = "Callback function called";
    string currentCard;
    char buffer[20];
    sprintf(buffer, "%i ;", p);
    currentCard = "UPDATE GAME SET phase = ";
    currentCard.append(buffer);
    fprintf(stdout, "update cut card query: %s\n", currentCard.c_str());
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
    sqlite3_close(db);
}
void updatePTurn(int n){
    sqlite3* db;
    rc = sqlite3_open("test.db", &db);
    if( rc ){ // opens DB
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    const char* sql;
    const char* data = "Callback function called";
    string currentCard;
    char buffer[20];
    sprintf(buffer, "%i ;", n);
    currentCard = "UPDATE GAME SET pTurn = ";
    currentCard.append(buffer);
    fprintf(stdout, "update cut card query: %s\n", currentCard.c_str());
    sql = currentCard.c_str();
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if(rc!= SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else{
        fprintf(stdout, "Records created successfully\n");
    }
    sqlite3_close(db);
}

void updateGoNum(int n){
    sqlite3* db;
    rc = sqlite3_open("test.db", &db);
    if( rc ){ // opens DB
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    const char* sql;
    const char* data = "Callback function called";
    char buffer[20];
    sprintf(buffer, "%i;", n);
    string currentCard = "UPDATE GAME SET goNumber = ";
    currentCard.append(buffer);
    fprintf(stdout, "update cut card query: %s\n", currentCard.c_str());
    sql = currentCard.c_str();
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if(rc!= SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else{
        fprintf(stdout, "Records created successfully\n");
    }
    sqlite3_close(db);
}

void updateCut(int cardNum){
    sqlite3* db;
    rc = sqlite3_open("test.db", &db);
    if( rc ){ // opens DB
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    const char* sql;
    const char* data = "Callback function called";
    rc = sqlite3_open("test.db", &db);
    char buffer[20];
    sprintf(buffer, "%i ;", cardNum);
    string currentCard = "UPDATE GAME SET cutCard = ";
    currentCard.append(buffer);
    fprintf(stdout, "update cut card query: %s\n", currentCard.c_str());
    sql = currentCard.c_str();
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if(rc!= SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else{
        fprintf(stdout, "Records created successfully\n");
    }
    sqlite3_close(db);
}

void removeCards(int pIndex, int context){
    sqlite3* db;
    rc = sqlite3_open("test.db", &db);
    if( rc ){ // opens DB
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    const char* sql;
    const char* data = "Callback function called";
    rc = sqlite3_open("test.db", &db);
    char buffer[50];
    sprintf(buffer, "%i AND context = %i;", pIndex, context);
    string currentQuery = "DELETE FROM HANDS WHERE pIndex = ";
    currentQuery.append(buffer);
    fprintf(stdout, "removeCard query: %s\n", currentQuery.c_str());
    sql = currentQuery.c_str();
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if(rc!= SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else{
        fprintf(stdout, "Records created successfully\n");
    }
    sqlite3_close(db);
}
void removeCard(int pIndex, int context, int cardNum){
    sqlite3* db;
    rc = sqlite3_open("test.db", &db);
    if( rc ){ // opens DB
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    const char* sql;
    const char* data = "Callback function called";
    string currentQuery;
    char buffer[50];
    sprintf(buffer, "%i AND context = %i AND cardID = %i;", pIndex, context, cardNum);
    currentQuery = "DELETE FROM HANDS WHERE pIndex = ";
    currentQuery.append(buffer);
    fprintf(stdout, "removeCard query: %s\n", currentQuery.c_str());
    sql = currentQuery.c_str();
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if(rc!= SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else{
        fprintf(stdout, "Records created successfully\n");
    }
    sqlite3_close(db);
}
void writeCard(int pIndex, int context, int card){
    sqlite3* db;
    rc = sqlite3_open("test.db", &db);
    if( rc ){ // opens DB
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    const char* sql;
    const char* data = "Callback function called";
    char buffer[50];
    sprintf(buffer, "%i, %i, %i );", pIndex, context, card);
    string currentCard = "INSERT INTO HANDS(pIndex, context, cardID) " \
                          "VALUES ( ";
    currentCard.append(buffer);
    fprintf(stdout, "currentCardQuery: %s\n", currentCard.c_str());
    sql = currentCard.c_str();
    rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
    if(rc!= SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }
    else{
        fprintf(stdout, "Records created successfully\n");
    }
    sqlite3_close(db);
}

void writeCards(int pIndex, int context, vector<int> cards){
    sqlite3* db;
    rc = sqlite3_open("test.db", &db);
    if( rc ){ // opens DB
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    const char* sql;
    const char* data = "Callback function called";
    string currentCard;
    char buffer[50];
    for(int i = 0; i<cards.size(); i++){
        sprintf(buffer, "%i, %i, %i );", pIndex, context, cards.at(i));
        currentCard = "INSERT INTO HANDS(pIndex, context, cardID) " \
                       "VALUES ( ";
        currentCard.append(buffer);
        fprintf(stdout, "currentCardQuery: %s\n", currentCard.c_str());
        sql = currentCard.c_str();
        rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
        if(rc!= SQLITE_OK ){
            fprintf(stderr, "SQL error: %s\n", zErrMsg);
            sqlite3_free(zErrMsg);
        }
        else{
            fprintf(stdout, "Records created successfully\n");
        }
    }
    sqlite3_close(db);
}

int getLP(int pIndex){
    sqlite3* db;
    rc = sqlite3_open("test.db", &db);
    if( rc ){ // opens DB
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    int lastCard;
    sqlite3_stmt *stmt;
    const char* sql;
    const char* data = "Callback function called";
    char buffer[20];
    sprintf(buffer, "%i;", pIndex);
    string sqlQuery = "SELECT lastPlayed FROM PLAYERS WHERE playerID = ";
    sqlQuery.append(buffer);
    sql =  sqlQuery.c_str();
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if( rc != SQLITE_OK){
        fprintf(stderr, "SELECT FAILED:%s\n ", sqlite3_errmsg(db));
    }
    while(sqlite3_step(stmt) == SQLITE_ROW){
        lastCard = sqlite3_column_int(stmt,0);
        fprintf(stdout, "LASTPLAYED CARD************** A**$## : %d\n", lastCard);
    }
    sqlite3_finalize(stmt);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        fprintf(stdout, "Operation done successfully\n");
    }
    sqlite3_close(db);
    return lastCard;
}


int getPTurn(){
    sqlite3* db;
    rc = sqlite3_open("test.db", &db);
    if( rc ){ // opens DB
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    int pTurn;
    sqlite3_stmt *stmt;
    const char* sql;
    string sqlQuery;
    const char* data = "Callback function called";
    sqlQuery = "SELECT pTurn FROM GAME;";
    sql =  sqlQuery.c_str();
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if( rc != SQLITE_OK){
        fprintf(stderr, "SELECT FAILED:%s\n ", sqlite3_errmsg(db));
    }
    while(sqlite3_step(stmt) == SQLITE_ROW){
        pTurn = sqlite3_column_int(stmt,0);
        fprintf(stdout, "pTurn: %d\n", pTurn);
    }
    sqlite3_finalize(stmt);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        fprintf(stdout, "Operation done successfully\n");
    }
    sqlite3_close(db);
    return pTurn;
}

int getScoredBool(){
    sqlite3* db;
    rc = sqlite3_open("test.db", &db);
    if( rc ){ // opens DB
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    int scoreBool;
    sqlite3_stmt *stmt;
    const char* sql;
    string sqlQuery;
    const char* data = "Callback function called";
    sqlQuery = "SELECT scoreHand FROM GAME";
    sql =  sqlQuery.c_str();
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if( rc != SQLITE_OK){
        fprintf(stderr, "SELECT FAILED:%s\n ", sqlite3_errmsg(db));
    }
    while(sqlite3_step(stmt) == SQLITE_ROW){
        scoreBool = sqlite3_column_int(stmt,0);
        fprintf(stdout, "scoreBool: %d", scoreBool);
    }
    sqlite3_finalize(stmt);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        fprintf(stdout, "Operation done successfully\n");
    }
    sqlite3_close(db);
    return scoreBool;
}

int getGoPhaseNum(){
    sqlite3* db;
    rc = sqlite3_open("test.db", &db);
    if( rc ){ // opens DB
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    int goNum;
    sqlite3_stmt *stmt;
    const char* sql;
    string sqlQuery;
    const char* data = "Callback function called";
    sqlQuery = "SELECT goNumber FROM GAME";
    sql =  sqlQuery.c_str();
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if( rc != SQLITE_OK){
        fprintf(stderr, "SELECT FAILED:%s\n ", sqlite3_errmsg(db));
    }
    while(sqlite3_step(stmt) == SQLITE_ROW){
        goNum = sqlite3_column_int(stmt,0);
        fprintf(stdout, "goNumber: %d", goNum);
    }
    sqlite3_finalize(stmt);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        fprintf(stdout, "Operation done successfully\n");
    }
    sqlite3_close(db);
    return goNum;
}

vector<int> getPlayer(int IDent){
    sqlite3* db;
    rc = sqlite3_open("test.db", &db);
    if( rc ){ // opens DB
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    vector<int> playerInfo;
    sqlite3_stmt *stmt;
    const char* sql;
    const char* data = "Callback function called";
    char buffer[20];
    sprintf(buffer, "%i;", IDent);
    string sqlTest = "SELECT points, isHuman, lastPlayed FROM PLAYERS WHERE playerID = ";
    sqlTest.append(buffer);
    sql = sqlTest.c_str();
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if( rc != SQLITE_OK){
        fprintf(stderr, "SELECT FAILED:%s\n ", sqlite3_errmsg(db));
    }
    while(sqlite3_step(stmt) == SQLITE_ROW){
        playerInfo.push_back(sqlite3_column_int(stmt,0));
        fprintf(stdout, "pInfo: %d", playerInfo.back());
        playerInfo.push_back(sqlite3_column_int(stmt,1));
        fprintf(stdout, "pInfo: %d", playerInfo.back());
        playerInfo.push_back(sqlite3_column_int(stmt,2));
        fprintf(stdout, "pInfo: %d", playerInfo.back());
    }
    sqlite3_finalize(stmt);
    if( rc != SQLITE_OK ){
        fprintf(stderr, "SQL error: %s\n", zErrMsg);
        sqlite3_free(zErrMsg);
    }else{
        fprintf(stdout, "GAME INFO returned scuccesueflly done successfully\n");
    }
    sqlite3_close(db);
    return playerInfo;
}

vector<int> getGame(){
    sqlite3* db;
    rc = sqlite3_open("test.db", &db);
    if( rc ){ // opens DB
        fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
        exit(0);
    }else{
        fprintf(stderr, "Opened database successfully\n");
    }
    vector<int> gameInfo;
    sqlite3_stmt *stmt;
    const char* data = "Callback function called";
    const char* sql;
    string sqlQuery = "SELECT dealerPos, pTurn, phase, goNumber FROM GAME;";
    fprintf(stdout, "getGame query: %s", sqlQuery.c_str());
    sql = sqlQuery.c_str();
    rc = sqlite3_prepare_v2(db, sql, -1, &stmt, NULL);
    if( rc != SQLITE_OK){
        fprintf(stderr, "SELECT FAILED:%s\n ", sqlite3_errmsg(db));
    }
    while(sqlite3_step(stmt) == SQLITE_ROW){
        gameInfo.push_back(sqlite3_column_int(stmt,0));
        gameInfo.push_back(sqlite3_column_int(stmt,1));
        gameInfo.push_back(sqlite3_column_int(stmt,2));
        gameInfo.push_back(sqlite3_column_int(stmt,3));
    }
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return gameInfo;
}
//makeGame: Initializes game objects, creates database tables

class AIDiscards : public xmlrpc_c::method {
    public:
        AIDiscards(){}
        void
            execute(xmlrpc_c::paramList const& paramList,
                    xmlrpc_c::value* returnP) {
                vector<int> aiHand;
                aiHand = getCards(1,0);
                writeCard(2,2, aiHand.back());
                aiHand.pop_back();
                removeCard(1,0,aiHand.back());
                writeCard(2,2, aiHand.back());
                aiHand.pop_back();
                removeCard(1,0,aiHand.back());
                *returnP = xmlrpc_c::value_int(5);
            }
};

class nextRound : public xmlrpc_c::method {
    public:
        nextRound(){}
        void
            execute(xmlrpc_c::paramList const& paramList,
                    xmlrpc_c::value* returnP) {
                int dPos = getDealerPos();
                removeCards(0,1);
                removeCards(1,1);
                removeCards(2,2);
                updatePhase(1);
                updateLP(0,0);
                updateLP(1,0);
                updateGoNum(0);
                updateDealerPos((dPos + 1) %2);
                updateScoreBool(0);
                *returnP = xmlrpc_c::value_int(-1);
            }
};

class fetchCutCard : public xmlrpc_c::method {
    public:
        fetchCutCard(){}
        void
            execute(xmlrpc_c::paramList const& paramList,
                    xmlrpc_c::value* returnP) {
                int index = getCutCard();
                *returnP = xmlrpc_c::value_int(index);
            }
};

class AITurn : public xmlrpc_c::method {
    public:
        AITurn(){}
        void
            execute(xmlrpc_c::paramList const& paramList,
                    xmlrpc_c::value* returnP) {
                int index = AIGoChoice(intsToCards(getCards(1,0)), getGoPhaseNum());
                *returnP = xmlrpc_c::value_int(index);
            }
};

class prepPIII : public xmlrpc_c::method {
    public:
        prepPIII(){}
        void
            execute(xmlrpc_c::paramList const& paramList,
                    xmlrpc_c::value* returnP) {
                updatePhase(3);
                updatePlayerPoints((getPTurn()+1)%2, 1);
                *returnP = xmlrpc_c::value_int(-1);
            }
};

class phaseIII : public xmlrpc_c::method {
    public:
        phaseIII(){}
        void
            execute(xmlrpc_c::paramList const& paramList,
                    xmlrpc_c::value* returnP) {
                vector<Card*> human;
                vector<Card*> ai;
                int p0Pts = scorePhase(intsToCards(getCards(0,1)));
                int p1Pts = scorePhase(intsToCards(getCards(1,1)));
                int cribPts = scorePhase(intsToCards(getCards(2,2)));
                int hasScored = getScoredBool();
                int dPos = getDealerPos();
                if(dPos == 0){
                    p0Pts+=cribPts;
                }
                else{
                    p1Pts+=cribPts;
                }
                vector<xmlrpc_c::value> gameData;
                gameData.push_back(xmlrpc_c::value_int(p0Pts));
                gameData.push_back(xmlrpc_c::value_int(p1Pts));
                gameData.push_back(xmlrpc_c::value_int(hasScored));
                if(hasScored == 0){
                    updatePlayerPoints(0,p0Pts);
                    updatePlayerPoints(1,p1Pts);
                    updateScoreBool(1);
                }
                xmlrpc_c::value_array RAY(gameData);
                *returnP = RAY;
            }
};

class phaseIIturn : public xmlrpc_c::method {
    public:
        phaseIIturn(){}
        void
            execute(xmlrpc_c::paramList const& paramList,
                    xmlrpc_c::value* returnP) {
                int const pIndex(paramList.getInt(0));
                int const cardNum(paramList.getInt(1));
                vector<Card*> roundCards;
                lastPlayed = played;
                if(cardNum == -1){played = false;}
                else{
                    played = true;
                    Card* justPlayed = intToCard(cardNum);
                    writeCard(3,3, cardNum);
                    removeCard(pIndex,0,cardNum);
                    int goNumber = getGoPhaseNum();
                    roundCards = intsToCards(getCards(3, 3));
                    updateLP(pIndex, cardNum);
                    goNumber += justPlayed->getValue();
                    updateGoNum(goNumber);
                    if(goNumber == 15) {updatePlayerPoints(pIndex, 2);}
                    if(goNumber == 31) {
                        updatePlayerPoints(pIndex, 2);
                        updateGoNum(0);
                        removeCards(3,3); //delete roundCards;
                    }
                    updatePlayerPoints(pIndex, goPoints(roundCards));
                }
                if(!lastPlayed && !played){
                    updatePlayerPoints(pIndex, 1);
                    updateGoNum(0);
                    removeCards(3,3); //deletemyID roundCards;
                }
                updatePTurn((pIndex +1)%2);
                *returnP = xmlrpc_c::value_int(-1);
            }
};

class prepPII : public xmlrpc_c::method {
    public:
        prepPII(){}
        void
            execute(xmlrpc_c::paramList const& paramList,
                    xmlrpc_c::value* returnP) {
                int const myID(paramList.getInt(0));
                /*
                //discard AI Cards
                vector<int> AICards = getCards(1,0);
                removeCard(1,0, AICards.back());
                writeCard(2,2, AICards.back()); //add to crib
                AICards.pop_back();
                removeCard(1,0, AICards.back());
                writeCard(2,2, AICards.back()); //add to crib
                AICards.pop_back();
                */
                writeCards(myID,1,getCards(myID,0)); //write score hand
                //writeCards(1,1,getCards(1,0));
                updatePhase(2);
                *returnP = xmlrpc_c::value_int(getCutCard());
            }
};

class tellDiscard : public xmlrpc_c::method {
    public:
        tellDiscard(){}
        void
            execute(xmlrpc_c::paramList const& paramList,
                    xmlrpc_c::value* returnP) {
                int const pIndex(paramList.getInt(0));
                int const ctxt(paramList.getInt(1));
                int const cardNum(paramList.getInt(2));
                writeCard(2,2, cardNum); //add to crib
                removeCard(pIndex, ctxt, cardNum);
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
                int cutCardNum, dPos;
                Deck * deck = new Deck();
                Card * cut;
                writeCards(0,0, cardsToInts(deck->dealCards()));
                writeCards(1,0, cardsToInts(deck->dealCards()));
                cutCardNum = cardToInt(deck->cutDeck());
                updateCut(cutCardNum);
                writeCard(0,1, cutCardNum);
                writeCard(1,1, cutCardNum);
                writeCard(2,2, cutCardNum);
                dPos = getDealerPos();
                if(cutCardNum % 13 == 11) {
                    updatePlayerPoints(dPos, 2);
                }
                delete deck;
                *returnP = xmlrpc_c::value_int(5);
            }
};

class readCards : public xmlrpc_c::method {
    public:
        readCards(){}
        void
            execute(xmlrpc_c::paramList const& paramList,
                    xmlrpc_c::value* returnP) {
                int const pID(paramList.getInt(0));
                int const context(paramList.getInt(1));
                vector<int> Cards;
                Cards = getCards(pID, context);
                vector<xmlrpc_c::value> gameData;
                if(Cards.empty()){
                    xmlrpc_c::value_array emptyArry(gameData);
                    *returnP = emptyArry;
                }
                else{
                    for(int i = 0; i < Cards.size(); i++){
                        gameData.push_back(xmlrpc_c::value_int(Cards.at(i)));
                    }
                    xmlrpc_c::value_array RAY(gameData);
                    *returnP = RAY;
                }
            }
};

class pIIinfo : public xmlrpc_c::method {
    public:
        pIIinfo(){}
        void
            execute(xmlrpc_c::paramList const& paramList,
                    xmlrpc_c::value* returnP) {
                int const myID(paramList.getInt(0));
                int otherID = (myID +1) % 2;
                int LP = getLP(otherID);
                vector<int>oppCards;
                oppCards = getCards(otherID, 0);
                vector<xmlrpc_c::value> gameData;
                gameData.push_back(xmlrpc_c::value_int(getGoPhaseNum()));
                gameData.push_back(xmlrpc_c::value_int(getPTurn()));
                gameData.push_back(xmlrpc_c::value_int(getPlayerPoints(0)));
                gameData.push_back(xmlrpc_c::value_int(getPlayerPoints(1)));
                gameData.push_back(xmlrpc_c::value_int(oppCards.size()));
                gameData.push_back(xmlrpc_c::value_int(LP));
                xmlrpc_c::value_array RAY(gameData);
                *returnP = RAY;
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

class getGameInfo : public xmlrpc_c::method {
    public:
        getGameInfo(){}
        void
            execute(xmlrpc_c::paramList const& paramList,
                    xmlrpc_c::value* returnP) {
                vector<int> gInfo;
                gInfo = getGame();
                vector<xmlrpc_c::value> gameData;
                gameData.push_back(xmlrpc_c::value_int(gInfo.at(0)));
                gameData.push_back(xmlrpc_c::value_int(gInfo.at(1)));
                gameData.push_back(xmlrpc_c::value_int(gInfo.at(2)));
                gameData.push_back(xmlrpc_c::value_int(gInfo.at(3)));
                xmlrpc_c::value_array RAY(gameData);
                *returnP = RAY;
            }
};

int
main(int           const,
        const char ** const) {
    try {
        xmlrpc_c::registry myRegistry;
        xmlrpc_c::methodPtr const getPlayerInfoP(new getPlayerInfo);
        myRegistry.addMethod("getPlayerInfo", getPlayerInfoP);
        xmlrpc_c::methodPtr const phaseIP(new phaseI);
        myRegistry.addMethod("server.phaseI", phaseIP);
        xmlrpc_c::methodPtr const tellDiscardP(new tellDiscard);
        myRegistry.addMethod("tellDiscard", tellDiscardP);
        xmlrpc_c::methodPtr const AIDiscardsP(new AIDiscards);
        myRegistry.addMethod("AIDiscards", AIDiscardsP);
        xmlrpc_c::methodPtr const getGameInfoP(new getGameInfo);
        myRegistry.addMethod("getGameInfo", getGameInfoP);
        xmlrpc_c::methodPtr const readCardsP(new readCards);
        myRegistry.addMethod("readCards", readCardsP);
        xmlrpc_c::methodPtr const prepPIIP(new prepPII);
        myRegistry.addMethod("prepPII", prepPIIP);
        xmlrpc_c::methodPtr const pIIinfoP(new pIIinfo);
        myRegistry.addMethod("pIIinfo", pIIinfoP);
        xmlrpc_c::methodPtr const AITurnP(new AITurn);
        myRegistry.addMethod("AITurn", AITurnP);
        xmlrpc_c::methodPtr const phaseIITurnP(new phaseIIturn);
        myRegistry.addMethod("phaseIITurn", phaseIITurnP);
        xmlrpc_c::methodPtr const phaseIIIP(new phaseIII);
        myRegistry.addMethod("phaseIII", phaseIIIP);
        xmlrpc_c::methodPtr const prepPIIIP(new prepPIII);
        myRegistry.addMethod("prepPIII", prepPIIIP);
        xmlrpc_c::methodPtr const nextRoundP(new nextRound);
        myRegistry.addMethod("nextRound", nextRoundP);
        xmlrpc_c::methodPtr const fetchCutCardP(new fetchCutCard);
        myRegistry.addMethod("getCutCard", fetchCutCardP);

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
