#include "TurnManager.h"
#include <algorithm> 

#include "TurnManager.h"

TurnManager::TurnManager(const std::vector<Player*>& order)
        : turnOrder(order),
          currentTurn(order.empty() ? nullptr : order[0]),
          turnCount(1)
{
}

void TurnManager::nextTurn() {
    if (turnOrder.empty()) return;

    turnCount++;
    auto it = std::find(turnOrder.begin(), turnOrder.end(), currentTurn);
    if (it == turnOrder.end() || ++it == turnOrder.end()) {
        currentTurn = turnOrder[0];
    } else {
        currentTurn = *it;
    }
}

Player* TurnManager::getCurrentPlayer() const {
    return currentTurn;
}

int TurnManager::getTurnCount() const {
    return turnCount;
}
