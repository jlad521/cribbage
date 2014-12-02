#include "Player.h"
#include "Card.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <string>
#include <vector>

using namespace std;
Player::Player(bool h, string n){
    points = 0;
    isHuman = h;
    name = n;
    if(h) {position = 0;}
    else {position = 1;}
    lastCard = new Card();
}

Player::Player(int pos, int pts, bool isHuman, Card* lp){ /*vector<Card* Hand, scoreHand*/
    position = pos;
    points = pts;
    isHuman = isHuman;
    if(pos == 0) { name = "Player 0";}
    if(pos == 1) { name = "Player 1";}
    lastCard = lp;
}

void Player::setLastCard(Card* card) {
    lastCard = card;
}
Card* Player::discard(int index) {
    Card* discard = hand.at(index);
    hand.erase(hand.begin()+index);
    return discard;
}

void Player::setPoints(int pts) {
    points = pts;
}

void Player::addPoints(int morePoints) {
    points += morePoints;
}
Card* Player::getLastCard(){
    return lastCard;
}
int Player::getPoints() {
    return points;
}

std::vector<Card*> Player::getHand() {
    return hand;
}

std::vector<Card*> Player::getScoreHand(){
    return scoreHand;
}
int Player::getPosition(){
    return position;
}

string Player::serialize(){
 	ostringstream stringStreamVal;
	stringStreamVal << "var1" << position << "; var2=" << points << ";var3=" << isHuman << ";var4=" << name <<  ";";
	return stringStreamVal.str();
}

void Player::reset(string serial){
	string delimiter = ";";
        string delimiter2 = "=";
	string token, token2, token3;
	int position = 0, position2 = 0;

	while ((position = serial.find(delimiter)) != -1) {
        	token = serial.substr(0, position);
            	position2 = token.find(delimiter2);
            	token2 = token.substr(0, position2);
	    	token3 = token.substr(position2+1,token.size());
	    	if (token2 == "position") {
			position = atoi(token3.c_str());
		}

		else if (token2 == "points") {
			points = atoi(token3.c_str());
	    	}

		else if (token2 == "isHuman") {
			isHuman = atoi(token3.c_str());
	    	}

		else {
		cout << "error" << endl;
	    	}

		serial.erase(0, position + delimiter.length());
	}
}









