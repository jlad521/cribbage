#include "Card.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

Card::Card(char a, int x, int z){
        faceValue = a;  // face value
        value = 0;  // value
        priority = x;	  // priority
        suit = z;
	}
Card::Card() {

        faceValue = 0;  // face value
        value = 0;  // value
        priority = 0;	  // priority
        suit = 0;
}
Card::~Card() {}

	void Card::setFace(char a) {
		suit = a;
	}

	char Card::getFace() {
		return suit;
	}

	void Card::setValue(int x) {
		value = x;
	}

	int Card::getValue() {
		return value;
	}

	void Card::setPriority(int y) {
		priority = y;
	}

	int Card::getPriority(){
		return priority;
	}

	void Card::setSuit(int z) {
		suit = z;
	}
	int Card::getSuit() {
		return suit;
	}

