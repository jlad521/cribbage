#include "ShowCribbage.h"
#include <cstdlib>
#include <sstream>
#include <math.h>
#include <iostream>
#include <ncurses.h>
#include <unistd.h>
#include "Player.h"
#include "Card.h"
//#ifndef DISPLAY_H
//#define DISPLAY_H
//#endif
//#include "display.h"
//#include "display.h"
//#include ""
using namespace std;
ShowCribbage::ShowCribbage(){
    xStart = 20;
    inc = 7;
    AIY = 5;
    humanY = 30;
    AIlpy = 10;
    hlpy = 25;
    yMsg= 17;
    cribx = xStart + 3*inc;
    scoreCribx = 40;
    xlp = 30;
}

void ShowCribbage::drawPII(Player* players[], Card* cutCard, int phase, int goPhaseNumber, bool isCrib, int dealerPos, bool myTurn, int myID, int oppCardNum){
    int otherID = (myID + 1) %  2;
    key = gameDisplay.captureInput();
    gameDisplay.drawBox(0,0,88,44,A_BOLD);
    if(phase == 1){
        messageStringBottom.str("");
        messageStringBottom << players[0]->name << ": " << players[0]->getPoints();
        gameDisplay.bannerBottom(messageStringBottom.str());
        messageStringTop.str("");
        messageStringTop << players[1]->name << ": " << players[1]->getPoints() << "                          Crib Owner: " << players[dealerPos]->name; //                             goPhaseNumber: " << goPhaseNumber;
        gameDisplay.bannerTop(messageStringTop.str());
    }
    /*Display unique set for discard and gophase */
    if(phase == 1 || phase == 2){
        for(int i = 0; i < players[myID]->hand.size(); i++){
            gameDisplay.displayCard(xStart, humanY, players[myID]->hand.at(i)->getSuit(), players[myID]->hand.at(i)->getPriority(), A_BOLD); //show facedown card as deck; draw another card over it if cutCard has been cut
            xStart += inc;
        }xStart = 20;
        for(int k = 0; k < oppCardNum; k++){
            gameDisplay.displayCard(xStart, AIY, -1, -1, A_BOLD); //show facedown card as deck; draw another card over it if cutCard has been cut
            xStart += inc;
        }xStart = 20;
        //Now display deck / cutcard:
        gameDisplay.displayCard(xStart, yMsg, cutCard->getSuit(), cutCard->getPriority(), A_BOLD);
        if(isCrib){ gameDisplay.displayCard(cribx, yMsg, -1, -1 , A_BOLD);}
    }
    if(phase == 2){
        messageStringTop.str("");
        messageStringTop << players[1]->name << ": " << players[1]->getPoints() << "                          Go Phase Number: " << goPhaseNumber;
        gameDisplay.bannerTop(messageStringTop.str());
        if(!myTurn){
            messageStringBottom.str("");
            messageStringBottom << players[0]->name << ": " << players[0]->getPoints() << "                   Other player thinking!";
            gameDisplay.bannerBottom(messageStringBottom.str());
        }
        else{
            messageStringBottom.str("");
            messageStringBottom << players[0]->name << ": " << players[0]->getPoints();
            gameDisplay.bannerBottom(messageStringBottom.str());
        }
        gameDisplay.displayCard(xlp,hlpy, players[0]->lastCard->getSuit(), players[0]->lastCard->getPriority(), A_BOLD);
        gameDisplay.displayCard(xlp,AIlpy, players[1]->lastCard->getSuit(), players[1]->lastCard->getPriority(), A_BOLD);
    }
}

void ShowCribbage::drawPIII(Player* players[], vector<Card*> crib, int hpoints, int apoints){
    key = gameDisplay.captureInput();
    gameDisplay.drawBox(0,0,88,44,A_BOLD); //SHOULD USE GETWIDTH AND HEIGHT FOR DIMENSIONS OF BOX
    messageStringTop.str("");
    messageStringTop << players[1]->name << ": " << players[1]->getPoints() <<  "           " << players[1]->name << " scores " << apoints << " points with that hand. Click to begin next round";
    gameDisplay.bannerTop(messageStringTop.str());
    messageStringBottom.str("");
    messageStringBottom << players[0]->name << ": " << players[0]->getPoints() <<  "           " << players[0]->name << " scores " << hpoints << " points with that hand.";
    gameDisplay.bannerBottom(messageStringBottom.str());
    for(int s = 0; s < crib.size(); s++){
        gameDisplay.displayCard(xStart, yMsg, crib.at(s)->getSuit(), crib.at(s)->getPriority(), A_BOLD); //show facedown card as deck; draw another card over it if cutCard has been cut
        xStart+= inc;
    }xStart = 20;

    for(int j = 0; j < players[0]->scoreHand.size(); j++){
        gameDisplay.displayCard(xStart, humanY, players[0]->scoreHand.at(j)->getSuit(), players[0]->scoreHand.at(j)->getPriority(), A_BOLD); //show facedown card as deck; draw another card over it if cutCard has been cut
        xStart+= inc;
    }xStart = 20;

    for(int z = 0; z < players[1]->scoreHand.size(); z++){
        gameDisplay.displayCard(xStart, AIY, players[1]->scoreHand.at(z)->getSuit(), players[1]->scoreHand.at(z)->getPriority(), A_BOLD); //show facedown card as deck; draw another card over it if cutCard has been cut
        xStart+= inc;
    }xStart = 20;
}

int ShowCribbage::getCard(Player* p, int phase, int goPhaseNumber){
    int rtrnNum;
    for(;;){
        key = gameDisplay.captureInput();
        if( key == -1 ){
            mouseX = gameDisplay.getMouseEventX();
            mouseY = gameDisplay.getMouseEventY();
            if(mouseX >= xStart && mouseX <=xStart + 5 && mouseY >= humanY && mouseY <= humanY + 4) {rtrnNum = 0;}
            if(mouseX >= xStart + inc && mouseX <=xStart + inc + 5 && mouseY >= humanY && mouseY <= humanY + 4) {rtrnNum = 1;}
            if(mouseX >= xStart + 2*inc && mouseX <=xStart + 2*inc + 5 && mouseY >= humanY && mouseY <= humanY + 4) {rtrnNum = 2;}
            if(mouseX >= xStart + 3*inc && mouseX <=xStart + 3*inc + 5 && mouseY >= humanY && mouseY <= humanY + 4) {rtrnNum = 3;}
            if(mouseX >= xStart + 4*inc && mouseX <=xStart + 4*inc + 5 && mouseY >= humanY && mouseY <= humanY + 4) {rtrnNum = 4;}
            if(mouseX >= xStart + 5*inc && mouseX <=xStart + 5*inc + 5 && mouseY >= humanY && mouseY <= humanY + 4) {rtrnNum = 5;}
            if(mouseX >= xStart + 10 && mouseX <=xStart + 15 && mouseY >= humanY && mouseY <= humanY + 8) {rtrnNum = -1;} //if they played leftmost card
            if(rtrnNum < p->hand.size()){ //make sure valid card selection
                if(phase == 1){
                    return rtrnNum;
                }
                if(phase == 2){
                    if(p->hand.at(rtrnNum)->getValue() + goPhaseNumber <= 31) {return rtrnNum;}
                }
            }
        }
        else if (key > 0){
            return -1;
        }
    }
}

void ShowCribbage::spinWait() {
    for(;;){
        key = gameDisplay.captureInput();
        if( key == -1) { break;}
    }
}

void ShowCribbage::displayWinner(Player* players[], int winPos){
    gameDisplay.drawBox(0,0,88,44,A_BOLD); //SHOULD USE GETWIDTH AND HEIGHT FOR DIMENSIONS OF BOX
    for (;;){
        key = gameDisplay.captureInput();

        messageStringTop.str("");
        messageStringTop << players[winPos]->name << " wins this game with a score of: " << players[winPos]->getPoints() << " beating " <<players[(1+winPos) % 2]->name << " with " << players[(1+winPos) %2]->getPoints();
        gameDisplay.bannerTop(messageStringTop.str());
        messageStringBottom.str("");
        gameDisplay.bannerBottom(messageStringBottom.str());
        if(key == -1) {break;}
    }
}
