/**
 * @file TurnManager.h
 * @brief Controls turn ordering and round counting.
 */
#ifndef TURNMANAGER_H
#define TURNMANAGER_H

#include <vector>
#include "../Players/Player.h"
/**
 * @class TurnManager
 * @brief Manages the game turns and determines whose turn it is.
 *
 * This class keeps track of the current player, total turn count,
 * and provides methods to progress the turn and query state.
 */
class TurnManager {
private:
    std::vector<Player*> players;
    size_t currentPlayerIndex;
    int turnCount;
public:
    TurnManager(const std::vector<Player*>& order);
    void endTurn();
    Player* getCurrentPlayer() const;
    int getTurnCount() const;
    bool isPlayerTurn() const;
};

#endif