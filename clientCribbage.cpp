#include <cstdlib>
#include <string>
#include <iostream>
#include <xmlrpc-c/girerr.hpp>
#include <xmlrpc-c/base.hpp>
#include <xmlrpc-c/client_simple.hpp>
#include <signal.h>
#include <ncurses.h>
#include <sstream>
#include "display.h"
#include <unistd.h>
#include "clientCribbage.h"
using namespace std;

clientCribbage::clientCribbage(){

}

display gameDisplay;
//CRIBBAGE GAME ON CLIENT:

int main(int argc, char* argv[])
{
  try {
        string const serverUrl("http://localhost:8080/RPC2");
        string const methodName("sample.rand");
        string const definePlayers("define_players");
        string const srvrphaseI("phaseI");

	int sum;

	// infinite loop for the main program, you can press ctrl-c to quit
	for (;;) {
        //will call getGameInfo xmlrpc_c method on server
        xmlrpc_c::clientSimple myClient;// = new(xmlrpc_c::clientSimple);
        xmlrpc_c::value result;// = new(xmlrpc_c::value);
        xmlrpc_c::value result2;// = new(xmlrpc_c::value);
        xmlrpc_c::value testResult;// = new(xmlrpc_c::value);
        xmlrpc_c::paramList playerInfo;

        myClient.call(serverUrl, definePlayers, &testResult);
        int test = xmlrpc_c::value_int(testResult);

        //call testFunctoin
        //myClient.call(serverUrl, getGameInfo, &testResult);
        //int teset = xmlrpc_c::value_int(testResult);
		// calls the game display to capture some input
    	key = gameDisplay.captureInput();
		// if a mouse event occurred
		if (key == -1) {
			        myClient.call(serverUrl, methodName, &result);
       			        sum = xmlrpc_c::value_int(result);
				suit=sum%5;
			        myClient.call(serverUrl, methodName, &result2);
			        sum = xmlrpc_c::value_int(result2);
				number = sum%15;
				gameDisplay.displayCard(cardX,cardY,suit,number, A_BOLD);
				// erase a portion of the screen in the shape of a card
				gameDisplay.eraseBox(cardX,cardY,6,5);
		}
	}

    } catch (exception const& e) {
        cerr << "Client threw error: " << e.what() << endl;
	sleep(6000);
    } catch (...) {
        cerr << "Client threw unexpected error." << endl;
	sleep(6000);
    }

	return 0;
}
