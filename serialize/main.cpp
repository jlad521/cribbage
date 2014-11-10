#include <iostream>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <cstddef>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h> 

using namespace std;

// Function called by sql query
// Stores data in *data pointer
static int callback(void *data, int argc, char **argv, char **azColName) {
  strcpy((char*)data, argv[0]);	  
  return 0;
 }

class Player {
public:
	// Serialize attributes of object to a string and return it
	string save() {
	  ostringstream stringStreamVal;
	  stringStreamVal << "var1=" << var1 << ";var2=" << var2 << ";var3=" << var3 << ";";
	  return stringStreamVal.str();
	}

	// Parse serialized object and reinitialize object
	void reset(string serial) {
	  string delimiter = ";";	  
          string delimiter2 = "=";
	  string token, token2, token3;
	  int position = 0, position2 = 0;
          while ((position = serial.find(delimiter)) != -1) {
            token = serial.substr(0, position);
            position2 = token.find(delimiter2);
            token2 = token.substr(0, position2);
	    token3 = token.substr(position2+1,token.size());
	    if (token2 == "var1") {
		var1 = atoi(token3.c_str());
	    } else if (token2 == "var2") {
		var2 = atof(token3.c_str());
	    } else if (token2 == "var3") {
		var3 = token3;
	    } else {
		cout << "error" << endl;
	    }
            serial.erase(0, position + delimiter.length());
	  }
	}

	int    var1;
	double var2;
	string var3;
};

int main(int argc, char* argv[])
{
   sqlite3 *db;
   char *zErrMsg = 0;
   int rc;
   const char* sql;
   char data[256];
   ostringstream stringStreamVal;
   Player player1, player2;
   string sql_string;  

   player1.var1 = 5;
   player1.var2 = 3.2;
   player1.var3 = "Hello";

   rc = sqlite3_open("test.db", &db);
   if( rc ){
      fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
      exit(0);
   }else{
      fprintf(stderr, "Opened database successfully\n");
   }

   sql = "CREATE TABLE GAME("  \
         "ID INT PRIMARY KEY NOT NULL," \
         "OBJECT_NAME TEXT NOT NULL," \
	 "OBJECT_TYPE TEXT NOT NULL," \
	 "OBJECT_VALUE TEXT NOT NULL);";
 
   rc = sqlite3_exec(db, sql, NULL, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
   fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      fprintf(stdout, "Table created successfully\n");
   }

   // Insert serialized version of player1 object to database
   sql_string = "INSERT INTO GAME VALUES (1, 'player1', 'Player', '" + player1.save() + "');";
   sql = sql_string.c_str();
   rc = sqlite3_exec(db, sql, callback, 0, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      fprintf(stdout, "Records created successfully\n");
   }

   // Read serialied version of player1 object from database
   sql = "SELECT OBJECT_VALUE from GAME WHERE OBJECT_NAME='player1'";
   rc = sqlite3_exec(db, sql, callback, (void*)data, &zErrMsg);
   if( rc != SQLITE_OK ){
      fprintf(stderr, "SQL error: %s\n", zErrMsg);
      sqlite3_free(zErrMsg);
   }else{
      fprintf(stdout, "Operation done successfully\n");
   }

   // Initialize player2 object based on player1 data read from database
   player2.reset(data);
   sqlite3_close(db);
}
