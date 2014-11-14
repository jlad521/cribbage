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
#include "clientCribbage.h"
using namespace std;


int main(int argc, char* argv[])
{

    /*
     * First, check game field isGame to see if game in progress
     *   If true:
     *      call getAllInfo, which returns all hard state info
     *      initialize all objects with relevant info
     *      initialize all variables (like dealerPos, cutCard, etc)
     *      check which phase game was in, execute that phase fnction
     *  If false:
     *      create regular clientCribbage game
     }
     */
    clientCribbage disPLAY;

	return 0;
}

