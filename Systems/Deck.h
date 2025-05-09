#ifndef DECK_H
#define DECK_H

#include <vector>
#include <memory>
#include <random>
#include <algorithm>
#include "../Cards/Card.h"

class Deck {
private:
    std::vector<std::unique_ptr<Card>> cards;
    void addBasicUnits();
    void addSpells();
    void addSpecialUnits();
public:
    Deck();
    void initializeStandardDeck();
    void addCard(std::unique_ptr<Card> card);
    std::unique_ptr<Card> drawCard();
    void shuffle();
    bool isEmpty() const;
    size_t size() const;
    void clear();
};

#endif
