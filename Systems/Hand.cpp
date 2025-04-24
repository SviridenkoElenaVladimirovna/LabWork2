#include "Hand.h"
#include <algorithm>
#include <iostream>
#include <stdexcept> 

void Hand::addCard(std::unique_ptr<Card> card) {
    cards.push_back(std::move(card));
}

std::unique_ptr<Card> Hand::playCard(size_t index) {
    if (index >= cards.size()) {
        throw std::out_of_range("Invalid card index");
    }
    auto card = std::move(cards[index]);
    cards.erase(cards.begin() + index);
    return card;
}

std::vector<size_t> Hand::getPlayableCardsIndices(int currentMana) const {
    std::vector<size_t> indices;
    for (size_t i = 0; i < cards.size(); ++i) {
        if (cards[i]->getCost() <= currentMana) {
            indices.push_back(i);
        }
    }
    return indices;
}

const std::vector<std::unique_ptr<Card>>& Hand::getCards() const {
    return cards;
}

bool Hand::isEmpty() const {
    return cards.empty();
}

size_t Hand::getSize() const {
    return cards.size();
}
