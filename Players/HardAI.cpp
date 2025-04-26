
#include "HardAI.h"
#include "../Cards/UnitCard.h"
#include "../Cards/SpellCard.h"
#include <algorithm>
#include <climits>
#include <iostream>

#include "../Systems/BattleSystem.h"

HardAI::HardAI(const std::string& name, int health, int mana, GameState* gameState, UIManager* uiManager)
        : AI(name, health, mana, gameState, uiManager) {}


int HardAI::chooseAttackTarget(int attackingUnitIndex) const {
    auto attacker = getBattlefield()[attackingUnitIndex].get();
    const auto& enemies = getOpponent()->getBattlefield();

    if (enemies.empty()) return -1;

    int bestScore = INT_MIN;
    int bestIndex = -1;

    for (size_t i = 0; i < enemies.size(); ++i) {
        int score = evaluateAttack(attacker, enemies[i].get());
        if (score > bestScore) {
            bestScore = score;
            bestIndex = static_cast<int>(i);
        }
    }

    return bestIndex;
}

int HardAI::evaluateAttack(UnitCard* attacker, UnitCard* target) const {
    if (target->getHealth() <= attacker->getAttack()) {

        if (attacker->getHealth() > target->getAttack()) {
            return 1000 + target->getAttack();
        }
    }

    int value = (attacker->getAttack() - target->getHealth()) -
                (target->getAttack() - attacker->getHealth());

    return value;
}

bool HardAI::shouldPlayCard(const Card* card) const {
    if (card->getType() == CardType::UNIT) return true;
    return getOpponent()->getBattlefield().size() > getBattlefield().size();
}

int HardAI::chooseCardToPlay() const {
    const auto& cards = getHand().getCards();
    int bestIndex = -1;
    int bestScore = INT_MIN;

    for (size_t i = 0; i < cards.size(); ++i) {
        if (cards[i]->getCost() <= getMana()) {
            int score = evaluateCard(cards[i].get());
            if (score > bestScore) {
                bestScore = score;
                bestIndex = static_cast<int>(i);
            }
        }
    }

    return bestIndex;
}

int HardAI::evaluateCard(const Card* card) const {
    if (card->getType() == CardType::UNIT) {
        auto unit = dynamic_cast<const UnitCard*>(card);
        return unit->getAttack() * 2 + unit->getHealth();
    }
    return card->getCost();
}

int HardAI::evaluateCardPlay(const Card* card) const {

    if (card->getType() == CardType::UNIT) {
        auto unit = dynamic_cast<const UnitCard*>(card);
        return unit->getAttack() + unit->getHealth();
    } else {
        return card->getCost() * 2;
    }
}
void HardAI::takeTurn() {
    std::cout << getName() << " makes a move...\n";

    while (hasPlayableCards()) {
        int cardIndex = chooseCardToPlay();
        if (cardIndex == -1) break;

        if (shouldPlayCard(getHand().getCards()[cardIndex].get())) {
            playCard(cardIndex);
        } else {
            break;
        }
    }

    int attackerIndex = chooseUnitToAttackWith();
    while (attackerIndex != -1) {
        int targetIndex = chooseAttackTarget(attackerIndex);
        if (targetIndex != -1) {
            battleSystem->attack(
                    *getBattlefield()[attackerIndex].get(),
                    *getOpponent()->getBattlefield()[targetIndex].get());
        } else {
            getBattlefield()[attackerIndex]->attackPlayer(getOpponent());
        }
        attackerIndex = chooseUnitToAttackWith();
    }
}

int HardAI::chooseUnitToAttackWith() const {
    const auto& battlefield = getBattlefield();
    for (size_t i = 0; i < battlefield.size(); ++i) {
        if (battlefield[i]->canAttackNow()) {
            return static_cast<int>(i);
        }
    }
    return -1;
}


UnitCard* HardAI::findBestAttackTarget(int attackerIndex) const {
    const auto& attacker = getBattlefield()[attackerIndex];
    const auto& enemies = getOpponent()->getBattlefield();

    if (enemies.empty()) return nullptr;

    for (const auto& enemy : enemies) {
        if (enemy->getHealth() <= attacker->getAttack() &&
            attacker->getHealth() > enemy->getAttack()) {
            return enemy.get();
        }
    }
    UnitCard* bestTarget = nullptr;
    int highestThreat = INT_MIN;

    for (const auto& enemy : enemies) {
        int threat = enemy->getAttack() * 2 + enemy->getHealth();
        if (threat > highestThreat) {
            highestThreat = threat;
            bestTarget = enemy.get();
        }
    }

    return bestTarget;
}
