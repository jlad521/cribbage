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
//#include "Cribbage.h"
#include "Deck.h"
#include <string>
#include "Player.h"
using namespace std;

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
//SHOULD TEST PLAYERS FIRST, BEFORE WRITING THIS
class makeGame : public xmlrpc_c::method {
    public:
        makeGame(){}
        void
            execute(xmlrpc_c::paramList const& paramList,
                    xmlrpc_c::value* returnP) {
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
                    sql = "CREATE TABLE GAME("  \
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
                sql = "INSERT INTO GAME () "  \
                //USE SERIALIZE FUNCTION IN VALUES COMMAND
                       "VALUES (1, serialize);";

                rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
                if( rc != SQLITE_OK ){
                    fprintf(stderr, "SQL error: %s\n", zErrMsg);
                    sqlite3_free(zErrMsg);
                }else{
                    fprintf(stdout, "Records created successfully\n");
                }

                *returnP = xmlrpc_c::value_int(5);
                //*returnP = &array;
                //xmlrpc_c::value *returnP(array);
                //*returnP->push_back(xmlrpc_c::value_int(k));
                //*returnP = xmlrpc_c::value_int(k);
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
            vector<Card*> aiHand;
            aiHand = deck->dealCards();
            vector<Card*> humanHand;
            humanHand = deck->dealCards();
            cut = deck->cutDeck();
            //now have to write to dB
            //--> need to create hands table

            }

};

int
main(int           const,
        const char ** const) {
    //Cribbage * game = new Cribbage();
    Deck * deck = new Deck();
    Card* c = new Card();
    try {
        xmlrpc_c::registry myRegistry;
        xmlrpc_c::methodPtr const sampleRandMethodP(new sampleRandMethod);
        myRegistry.addMethod("sample.rand", sampleRandMethodP);
        /* Now add my Method */
        xmlrpc_c::methodPtr const getPlayerInfoP(new getPlayerInfo);
        myRegistry.addMethod("define_players", getPlayerInfoP);

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
