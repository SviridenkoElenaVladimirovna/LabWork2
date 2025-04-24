
#ifndef TURNMANAGER_H
#define TURNMANAGER_H

#include <vector>
#include "../Players/Player.h"

class TurnManager {
private:
    std::vector<Player*> players;
    size_t currentPlayerIndex;
    int turnCount;
    void updateManaForNewRound();
public:
    TurnManager(const std::vector<Player*>& order);
    void endTurn();
    Player* getCurrentPlayer() const;
    int getTurnCount() const;
    bool isNewRound() const;
    bool isPlayerTurn() const;
};

#endif
