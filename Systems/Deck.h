/*
Sviridenko Elena st130482@student.spbu.ru
The system under development is a C++ console card game inspired by Hearthstone. 
*/
/**
 * @file Deck.h
 * @brief A class to manage the player's deck of cards.
 */
#ifndef DECK_H
#define DECK_H

#include <vector>
#include <memory>
#include <random>
#include <algorithm>
#include "../Cards/Card.h"
/**
 * @class Deck
 * @brief Contains cards and methods for working with the deck.
 */
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
