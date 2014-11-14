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
#include "Cribbage.h"
using namespace std;

static void detectResize (int sig);
// stub artifact for what the game does when the screen resizes
void stub_PrintResize(void);

// The gameDisplay object is global, because the static signal handler object
// needs to access the dynamic object.
display gameDisplay;

/*
 * This is the main function that starts the driver artifact.
 * This function demonstrates some of the abilities of the Display class
 */
int main(int argc, char* argv[])
{
    char key;
    int cardX = 0;
    int cardY = 0;
    int suit = 0;
    int number = 0;
    int dragX = 0;
    int dragY = 0;

	// using a stringstream rather than a string to make making the banner easier stringstream messageString; //Cribbage* game = new Cribbage();

	// enable a interrupt triggered on a window resize
	signal(SIGWINCH, detectResize); // enable the window resize signal


  try {
        string const serverUrl("http://localhost:8080/RPC2");
        string const methodName("sample.rand");
        string const definePlayers("define_players");

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

/*
 * This is the interrupt service routine called when the resize screen
 * signal is captured.
 */
void detectResize(int sig) {
	// update the display class information with the new window size
    gameDisplay.handleResize(sig);
	// re-enable the interrupt for a window resize
    signal(SIGWINCH, detectResize);
	/*INSERT YOUR OWN SCREEN UPDATE CODE instead of stub_PrintResize*/
	stub_PrintResize();
}

/*
 * This is a simple stub that should be replaced with what the game does
 * when the screen resizes.
 */
void stub_PrintResize(void) {
	// gets the new screen size
	int cols = gameDisplay.getCols();
	int lines = gameDisplay.getLines();
	// setups a message stream
	stringstream messageString;
	messageString << "Terminal is " << cols << "x" << lines;
	// prints out the information of the new screen size in a top banner
	gameDisplay.bannerTop(messageString.str());
}

