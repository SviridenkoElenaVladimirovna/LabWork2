#ifndef TURNMANAGER_H
#define TURNMANAGER_H

#include <vector>
#include <memory>
#include "../Players/Player.h"

class TurnManager {
private:
    Player* currentTurn;
    std::vector<Player*> turnOrder;
    int turnCount;

public:

    TurnManager(const std::vector<Player*>& order);

    void nextTurn();

    Player* getCurrentPlayer() const;


    int getTurnCount() const;
};

#endif