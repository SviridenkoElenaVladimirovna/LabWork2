/*
Sviridenko Elena st130482@student.spbu.ru
The system under development is a C++ console card game inspired by Hearthstone. 
*/
#include "Hand.h"
#include <algorithm>
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
void Hand::removeCard(size_t index) {
    if (index >= cards.size()) return;
    cards.erase(cards.begin() + index);
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
const Card* Hand::operator[](size_t index) const {
    return cards[index].get();
}

void Hand::clear() {
    cards.clear();
}

size_t Hand::size() const {
    return cards.size();
}

bool Hand::empty() const {
    return cards.empty();
}

auto Hand::begin() {
    return cards.begin();
}

auto Hand::end() {
    return cards.end();
}

Card* Hand::operator[](size_t index) {
    return cards[index].get();
}

std::vector<std::unique_ptr<Card>>& Hand::getCardsRef() {
    return cards;
}
