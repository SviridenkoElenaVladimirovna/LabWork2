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

    std::vector<std::unique_ptr<Card>>& getCardsRef() { return cards; }
    void addCard(std::unique_ptr<Card> card);
    const std::vector<std::unique_ptr<Card>>& getCards() const;
    bool isEmpty() const;
    size_t getSize() const;
    void clear() {
        cards.clear();
    }
    size_t size() const { return cards.size(); }
    bool empty() const { return cards.empty(); }
    auto begin() { return cards.begin(); }
    auto end() { return cards.end(); }
    Card* operator[](size_t index) {
        return cards[index].get();
    }
    std::vector<size_t> getPlayableCardsIndices(int currentMana) const;

    const Card* operator[](size_t index) const {
        return cards[index].get();
    }
};

#endif