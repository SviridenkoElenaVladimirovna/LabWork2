#include "Card.h"
#include "../Players/Player.h"

Card::Card(const std::string& name, int cost, CardType type)
        : name(name), cost(cost), type(type) {}

void Card::displayInfo() const {
    std::cout << name << " (" << cost << " mana)";
}
bool Card::isPlayed() const {
    return played;
}

void Card::setPlayed(bool val) {
    played = val;
}

std::string Card::getName() const {
    return name;
}

int Card::getCost() const {
    return cost;
}

CardType Card::getType() const {
    return type;
}
