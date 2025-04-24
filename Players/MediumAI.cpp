
#include "MediumAI.h"
#include "../Cards/UnitCard.h"
#include <algorithm>
#include <iostream>
#include <climits>

MediumAI::MediumAI(const std::string& name, int health, int mana, GameState* gameState)
        : AI(name, health, mana, gameState) {}

int MediumAI::chooseAttackTarget(int attackingUnitIndex) const {
    auto attacker = getBattlefield()[attackingUnitIndex].get();
    auto weakest = findWeakestEnemy();

    if (!weakest) return -1;

    const auto& enemies = getOpponent()->getBattlefield();
    for (size_t i = 0; i < enemies.size(); ++i) {
        if (enemies[i].get() == weakest) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

bool MediumAI::shouldPlayCard(const Card* card) const {
    return card->getType() == CardType::UNIT;
}

int MediumAI::chooseCardToPlay() const {
    const auto& cards = getHand().getCards();
    int bestIndex = -1;
    int lowestCost = 100;

    for (size_t i = 0; i < cards.size(); ++i) {
        if (cards[i]->getCost() <= getMana() &&
            cards[i]->getCost() < lowestCost) {
            lowestCost = cards[i]->getCost();
            bestIndex = static_cast<int>(i);
        }
    }

    return bestIndex;
}

void MediumAI::takeTurn() {
    std::cout << getName() << " makes a move...\n";

    while (hasPlayableCards()) {
        int cardIndex = chooseCardToPlay();
        if (cardIndex == -1) break;
        playCard(cardIndex);
    }

    int attackerIndex = chooseUnitToAttackWith();
    while (attackerIndex != -1) {
        int targetIndex = chooseAttackTarget(attackerIndex);
        if (targetIndex != -1) {
            getBattlefield()[attackerIndex]->attackTarget(
                    getOpponent()->getBattlefield()[targetIndex].get());
        } else {
            getBattlefield()[attackerIndex]->attackPlayer(getOpponent());
        }
        attackerIndex = chooseUnitToAttackWith();
    }
}

int MediumAI::chooseUnitToAttackWith() const {
    const auto& battlefield = getBattlefield();
    for (size_t i = 0; i < battlefield.size(); ++i) {
        if (battlefield[i]->canAttackNow()) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

UnitCard* MediumAI::findOptimalAttackTarget() const {
    const auto& enemies = getOpponent()->getBattlefield();
    if (enemies.empty()) return nullptr;

    UnitCard* optimalTarget = nullptr;
    int minHealth = INT_MAX;

    for (const auto& enemy : enemies) {
        if (enemy->getHealth() < minHealth) {
            optimalTarget = enemy.get();
            minHealth = enemy->getHealth();
        }
    }

    return optimalTarget;
}
