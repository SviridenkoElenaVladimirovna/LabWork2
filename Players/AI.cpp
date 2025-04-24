#include <algorithm>
#include "AI.h"
#include "../Core/GameState.h"
AI::AI(const std::string& name, int health, int mana, GameState* gameState)
        : Player(name, health, mana, gameState) {}

void AI::takeTurn() {
    while (hasPlayableCards()) {
        int cardIndex = chooseCardToPlay();
        if (cardIndex >= 0) {
            playCard(cardIndex);
        } else {
            break;
        }
    }

    int unitIndex = chooseUnitToAttackWith();
    while (unitIndex >= 0) {
        attackWithUnit(unitIndex);
        unitIndex = chooseUnitToAttackWith();
    }

    endTurn();
}

void AI::attackWithUnit(int unitIndex) {
    int targetIndex = chooseAttackTarget(unitIndex);
    if (targetIndex >= 0) {
        getBattlefield()[unitIndex]->attackTarget(
                getOpponent()->getBattlefield()[targetIndex].get());
    } else {
        getBattlefield()[unitIndex]->attackPlayer(getOpponent());
    }
    cleanDeadUnits();
}

bool AI::hasPlayableCards() const {
    const auto& cards = getHand().getCards();
    return std::any_of(cards.begin(), cards.end(),
                       [this](const auto& card) { return card->getCost() <= getMana(); });
}

int AI::chooseCardToPlay() const {
    const auto& cards = getHand().getCards();
    for (size_t i = 0; i < cards.size(); ++i) {
        if (cards[i]->getCost() <= getMana() && shouldPlayCard(cards[i].get())) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int AI::chooseUnitToAttackWith() const {
    const auto& battlefield = getBattlefield();
    for (size_t i = 0; i < battlefield.size(); ++i) {
        if (battlefield[i]->canAttackNow()) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int AI::chooseAttackTarget(int attackingUnitIndex) const {
    if (!getOpponent() || attackingUnitIndex < 0 ||
        attackingUnitIndex >= static_cast<int>(getBattlefield().size())) {
        return -1;
    }

    auto attacker = getBattlefield()[attackingUnitIndex].get();
    auto target = findBestAttackTarget(attacker);

    if (!target) return -1;

    const auto& enemies = getOpponent()->getBattlefield();
    for (size_t i = 0; i < enemies.size(); ++i) {
        if (enemies[i].get() == target) {
            return static_cast<int>(i);
        }
    }
    return -1;
}
void AI::cleanDeadUnits() {
    auto& battlefield = getBattlefield();
    battlefield.erase(
            std::remove_if(battlefield.begin(), battlefield.end(),
                           [](const auto& unit) { return unit->isDead(); }),
            battlefield.end());
}

UnitCard* AI::findWeakestEnemy() const {
    const auto& enemies = getOpponent()->getBattlefield();
    if (enemies.empty()) return nullptr;

    UnitCard* weakest = enemies.front().get();
    for (const auto& enemy : enemies) {
        if (enemy->getHealth() < weakest->getHealth()) {
            weakest = enemy.get();
        }
    }
    return weakest;
}

UnitCard* AI::findStrongestEnemy() const {
    const auto& enemies = getOpponent()->getBattlefield();
    if (enemies.empty()) return nullptr;

    UnitCard* strongest = enemies.front().get();
    for (const auto& enemy : enemies) {
        if (enemy->getAttack() > strongest->getAttack()) {
            strongest = enemy.get();
        }
    }
    return strongest;
}

UnitCard* AI::findBestAttackTarget(UnitCard* attacker) const {
    if (!attacker || !getOpponent()) return nullptr;

    const auto& enemies = getOpponent()->getBattlefield();
    if (enemies.empty()) return nullptr;

    return findWeakestEnemy();
}
