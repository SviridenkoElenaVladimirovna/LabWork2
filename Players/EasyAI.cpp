#include "EasyAI.h"
#include "../Cards/UnitCard.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <memory>

#include "../Core/GameState.h"

EasyAI::EasyAI(const std::string& name, int health, int mana, GameState* gameState)
        : AI(name, health, mana, gameState) {}

int EasyAI::chooseAttackTarget(int attackingUnitIndex) const {
    if (!getOpponent() || attackingUnitIndex < 0 ||
        attackingUnitIndex >= static_cast<int>(getBattlefield().size())) {
        return -1;
    }

    const auto& enemies = getOpponent()->getBattlefield();
    if (enemies.empty()) return -1;

    return rand() % enemies.size();
}

bool EasyAI::shouldPlayCard(const Card* card) const {
    return true;
}

bool EasyAI::hasPlayableCards() const {
    const auto& cards = getHand().getCards();
    return std::any_of(cards.begin(), cards.end(),
                       [this](const auto& card) {
                           return card->getCost() <= getMana();
                       });
}

int EasyAI::chooseCardToPlay() const {
    const auto& cards = getHand().getCards();
    for (size_t i = 0; i < cards.size(); ++i) {
        if (cards[i]->getCost() <= getMana()) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int EasyAI::chooseUnitToAttackWith() const {
    const auto& battlefield = getBattlefield();
    for (size_t i = 0; i < battlefield.size(); ++i) {
        if (battlefield[i]->canAttackNow()) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

UnitCard* EasyAI::findWeakestEnemy() const {
    if (!getOpponent() || getOpponent()->getBattlefield().empty()) {
        return nullptr;
    }

    const auto& enemies = getOpponent()->getBattlefield();
    UnitCard* weakest = enemies[0].get();

    for (const auto& enemy : enemies) {
        if (enemy->getHealth() < weakest->getHealth()) {
            weakest = enemy.get();
        }
    }
    return weakest;
}

void EasyAI::takeTurn() {
    std::cout << getName() << " makes a move...\n";

    while (hasPlayableCards()) {
        int cardIndex = chooseCardToPlay();
        if (cardIndex == -1) break;

        playCard(cardIndex);
    }

    while (true) {
        int unitIndex = chooseUnitToAttackWith();
        if (unitIndex == -1) break;

        int targetIndex = chooseAttackTarget(unitIndex);
        if (targetIndex >= 0) {
            getBattlefield()[unitIndex]->attackTarget(
                    getOpponent()->getBattlefield()[targetIndex].get());
        } else {
            getBattlefield()[unitIndex]->attackPlayer(getOpponent());
        }
    }

    std::cout << getName() << " completes the move.\n";
}

