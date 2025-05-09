#include "EasyAI.h"
#include "../Cards/SpellCard.h"
#include "../Core/GameEngine.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>

EasyAI::EasyAI(const std::string& name, int health, int mana, GameState* gameState, UIManager* uiManager)
        : AI(name, health, mana, gameState, uiManager) {}
void EasyAI::playSelectedCard(int cardIndex) {
    try {
        Hand& hand = getHandRef();
        if (cardIndex < 0 || static_cast<size_t>(cardIndex) >= hand.getCards().size()) {
            return;
        }

        auto card = hand.getCards()[cardIndex].get();
        int cardCost = card->getCost();

        if (cardCost > getMana()) {
            return;
        }

        if (auto* spellCard = dynamic_cast<SpellCard*>(card)) {
            spellCard->play(this, getOpponent());
            if (ui) {
                ui->showCardPlayedMessage(spellCard->getName(), true);
            }
        } else if (auto* unitCard = dynamic_cast<UnitCard*>(card)) {
            playCard(cardIndex);
            if (ui) {
                ui->showCardPlayedMessage(unitCard->getName(), false);
            }
        }

        hand.removeCard(cardIndex);
        setMana(getMana() - cardCost);

    } catch (const std::exception& e) {
        logEvent("AI Error", "Failed to play card: " + std::string(e.what()));
        throw;
    }
}
void EasyAI::takeTurn() {
    if (ui) {
        ui->displayMessage(getName() + " makes a move...");
    }
    while (hasPlayableCards()) {
        int cardIndex = chooseCardToPlay();
        if (cardIndex == -1) break;

        try {
            playSelectedCard(cardIndex);
        } catch (...) {
            continue;
        }
    }

    while (canAttack()) {
        int unitIndex = chooseUnitToAttackWith();
        if (unitIndex == -1) break;

        int targetIndex = chooseAttackTarget(unitIndex);
        if (targetIndex != -1) {
            battleSystem->attack(
                    *getBattlefield()[unitIndex].get(),
                    *getOpponent()->getBattlefield()[targetIndex].get());
        } else {
            getBattlefield()[unitIndex]->attackPlayer(getOpponent());
        }
    }

    if (ui) {
        ui->displayMessage(getName() + " ends turn.");
    }
}
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
    std::vector<int> playableIndices;

    for (size_t i = 0; i < cards.size(); ++i) {
        if (cards[i]->getCost() <= getMana()) {
            playableIndices.push_back(static_cast<int>(i));
        }
    }

    if (playableIndices.empty()) return -1;

    return playableIndices[rand() % playableIndices.size()];
}

int EasyAI::chooseUnitToAttackWith() const {
    const auto& battlefield = getBattlefield();
    std::vector<int> canAttackIndices;

    for (size_t i = 0; i < battlefield.size(); ++i) {
        if (battlefield[i]->canAttackNow() && !battlefield[i]->isDead()) {
            canAttackIndices.push_back(static_cast<int>(i));
        }
    }

    if (canAttackIndices.empty()) return -1;

    return canAttackIndices[rand() % canAttackIndices.size()];
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

