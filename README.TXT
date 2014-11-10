Hello Professor Irwin:
In order to play the game, simply type make and ./play.  Note, your terminal must be full screen to look correct; the game layout was designed for the terminal covering half the screen. 

To play Cribbage:
   -- click on 2 cards to discard, which will go to the "crib"
   -- after, you will enter the "go" phase: simply click on a card to play it
   -- the "goPhaseNumber" cannot exceed 31 pts. if you don't have a card in your hand, whose face value added to the goPhaseNumber less than 31,
   -- press any key for a "go". This will continue until both players are out of cards
   -- Then, you will see 3 hands laid out: whoever owns the crib that round gets the points from the middle hand 
   -- when in doubt, blind clicking and button mashing should do the trick

In order to run the testing suite, you have to make minor modifications to 2 files. I'm certain this isn't the best way to do this, sorry for the extra complication.
    1. in main.cpp:
        A. comment out #include "Cribbage.h"
        B. make sure you have #include "testCribbage.h"
        C. comment out Cribbage game;
        D. uncomment testCribbage testGame;
    2.  Modify the Makefile:
        A. comment out first SOURCES line
        B. Include second SOURCES line  (only difference is second SOURCES line compiles testCribbage instead of Cribbage)
    3. Now simply type make and ./play to see the testing suit execute

Enjoy :D
