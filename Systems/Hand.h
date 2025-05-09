#ifndef HAND_H
#define HAND_H

#include <vector>
#include <memory>
#include "../Cards/Card.h"

class Hand {
private:
    std::vector<std::unique_ptr<Card>> cards;
public:
    ~Hand() = default;
    void removeCard(size_t index);
    std::unique_ptr<Card> playCard(size_t index);
    std::vector<std::unique_ptr<Card>>& getCardsRef();
    void addCard(std::unique_ptr<Card> card);
    const std::vector<std::unique_ptr<Card>>& getCards() const;
    bool isEmpty() const;
    size_t getSize() const;
    void clear();
    size_t size() const;
    bool empty() const;
    auto begin();
    auto end();
    Card* operator[](size_t index);
    std::vector<size_t> getPlayableCardsIndices(int currentMana) const;
    const Card* operator[](size_t index) const;
};

#endif