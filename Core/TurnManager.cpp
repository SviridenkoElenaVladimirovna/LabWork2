#include "TurnManager.h"
#include <algorithm>

TurnManager::TurnManager(const std::vector<Player*>& order)
        : players(order), currentPlayerIndex(0), turnCount(1) {}

void TurnManager::endTurn() {
    currentPlayerIndex = (currentPlayerIndex + 1) % players.size();

    if (currentPlayerIndex == 0) {
        turnCount++;
        for (auto player : players) {
            if (player) {
                int newMaxMana = std::min(player->getMaxMana() + 1, 10);
                player->setMaxMana(newMaxMana);
                player->setMana(newMaxMana);

                player->drawCard();
            }
        }
    } else {
        if (players[currentPlayerIndex]) {
            players[currentPlayerIndex]->drawCard();
        }
    }
}

Player* TurnManager::getCurrentPlayer() const {
    return players.empty() ? nullptr : players[currentPlayerIndex];
}

int TurnManager::getTurnCount() const {
    return turnCount;
}

bool TurnManager::isPlayerTurn() const {
    return currentPlayerIndex == 0;
}
