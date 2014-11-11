#include "Card.h"
#include <vector>

class Deck{
public:
    Deck();

	std::vector<Card*> deck;

	std::vector<Card*> dealCards(void);

	Card* cutDeck(void);

	Card* removeCard(int index);

	Card* removeRandomCard();

	Card* at(int index);

	int size();
};
