#include "Card.h"
#include "../Players/Player.h"

Card::Card(const std::string& name, int cost, CardType type)
        : name(name), cost(cost), type(type) {}
