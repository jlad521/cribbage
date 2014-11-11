#include "Deck.h"
#include <vector>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

Deck::Deck () {
			char FACE[13] = {'A','2','3','4','5','6','7','8','9','T','J','Q','K'};
			int val;
			int count = 0;

			for (int j = 0; j < 4; j++){
				for(int i = 0; i < 13; i++) {
					Card*  newCard = new Card(FACE[i], i+1, j+1);
					if (i < 9) {
						newCard->setValue(i+1);
					}
					else {
						newCard->setValue(10);
					}
					deck.push_back(newCard);
					count++;
				}
			}
		}

	// DEALCARDS method, returns a vector of cards
std::vector<Card*> Deck::dealCards() {
	std::vector<Card*> hand;
		// Use comment to switch between "random mode" and "test mode"
	for (int i = 0; i < 6; i++) {
		hand.push_back(this->removeRandomCard());  //random mode
		//hand.push_back (this->removeCard(0));		//test mode (you know whats supposed to be removed and what's not)
		}
		return hand;
	}
	// Cuts the deck, removes the random card from the deck and returns it
Card* Deck::cutDeck() {
	Card* removed_card = this->removeRandomCard();
	return removed_card;
    }
	// Removes card at the specified index from deck
Card* Deck::removeCard(int index) {
		Card* removed_card = deck.at(index);
		deck.erase(deck.begin()+index);
		return removed_card;
	}
	// Removes a random card from deck
Card* Deck::removeRandomCard() {
		int random = rand() % deck.size();
		return this->removeCard(random);
	}
Card* Deck::at(int index) {
		return deck.at(index);
	}
int Deck::size () {
		return deck.size();
	}


