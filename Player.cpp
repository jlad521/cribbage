#include "Player.h"
#include "Card.h"
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

Player::Player(int pos, int pts, bool isHuman, string nm, Card* lp /*vector<Card* Hand, scoreHand*/){
    position = pos;
    points = pts;
    isHuman = isHuman;
    name = nm;
    lastCard = new Card(); //THIS SHOULD HOLD THE INT TO CARD CONVERSION
}

void Player::setLastCard(Card* card) {
    lastCard = card;
}
Card* Player::discard(int index) {
    Card* discard = hand.at(index);
    hand.erase(hand.begin()+index);
    return discard;
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
