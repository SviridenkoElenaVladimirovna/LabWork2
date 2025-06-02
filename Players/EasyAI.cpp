/*
Sviridenko Elena st130482@student.spbu.ru
The system under development is a C++ console card game inspired by Hearthstone. 
*/
#include "EasyAI.h"
#include "../Cards/SpellCard.h"
#include "../Core/GameEngine.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <memory>

EasyAI::EasyAI(const std::string& name, int health, int mana, GameState* gameState, UIManager* uiManager)
    : AI(name, health, mana, gameState, uiManager) {}

void EasyAI::playSelectedCard(int cardIndex) {
    try {
        if (cardIndex < 0) return;

        Hand& hand = getHandRef();
        if (static_cast<size_t>(cardIndex) >= hand.getCards().size()) {
            logEvent("AI Warning", "Invalid card index");
            return;
        }

        Card* card = hand.getCards()[cardIndex].get();
        if (!card || card->getCost() > getMana()) {
            return;
        }

        bool isSpell = false;
        std::string cardName = card->getName();

        if (auto* spellCard = dynamic_cast<SpellCard*>(card)) {
            isSpell = true;
            if (getOpponent()) {
                spellCard->play(this, getOpponent());
            }
        } 
        else if (auto* unitCard = dynamic_cast<UnitCard*>(card)) {
            playCard(cardIndex);
        }

        hand.removeCard(cardIndex);
        setMana(getMana() - card->getCost());

        if (ui) {
            ui->showCardPlayedMessage(cardName, isSpell);
        }

    } catch (const std::exception& e) {
        logEvent("AI Error", std::string("Play card failed: ") + e.what());
    }
}

void EasyAI::takeTurn() {
    if (ui) {
        ui->displayMessage(getName() + " makes a move...");
    }

    while (hasPlayableCards()) {
        int cardIndex = chooseCardToPlay();
        if (cardIndex == -1) break;

        playSelectedCard(cardIndex);
    }

    performAttacks();

    cleanBattlefield();
    if (auto opponent = getOpponent()) {
        opponent->cleanBattlefield();
    }

    if (ui) {
        ui->displayMessage(getName() + " ends turn.");
    }
}

void EasyAI::performAttacks() {
    auto opponent = getOpponent();
    if (!opponent || !battleSystem) return;

    const auto& battlefield = getBattlefield();
    const auto& enemyField = opponent->getBattlefield();

    for (size_t i = 0; i < battlefield.size(); ++i) {
        if (!battlefield[i] || !battlefield[i]->canAttackNow() || battlefield[i]->isDead()) {
            continue;
        }

        try {
            int targetIndex = chooseAttackTarget(i);
            
            if (targetIndex != -1 && 
                targetIndex < static_cast<int>(enemyField.size()) &&
                enemyField[targetIndex]) {
        
                battleSystem->attack(*battlefield[i], *enemyField[targetIndex]);
                if (ui) {
                    ui->displayMessage(battlefield[i]->getName() + 
                                      " attacks " + 
                                      enemyField[targetIndex]->getName());
                }
            } else {
                battlefield[i]->attackPlayer(opponent);
                if (ui) {
                    ui->displayMessage(battlefield[i]->getName() + 
                                      " attacks the enemy hero");
                }
            }
        } catch (const std::exception& e) {
            logEvent("Attack Error", std::string("Unit attack failed: ") + e.what());
        }
    }
}
bool EasyAI::shouldPlayCard(const Card* card) const {
    return card && card->getCost() <= getMana();
}

int EasyAI::chooseUnitToAttackWith() const {
    const auto& battlefield = getBattlefield();
    for (size_t i = 0; i < battlefield.size(); ++i) {
        if (battlefield[i] && battlefield[i]->canAttackNow() && !battlefield[i]->isDead()) {
            return static_cast<int>(i);
        }
    }
    return -1;
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
int EasyAI::chooseAttackTarget(size_t attackingUnitIndex) const {
    if (!getOpponent() || attackingUnitIndex >= getBattlefield().size()) {
        return -1;
    }

    const auto& enemies = getOpponent()->getBattlefield();
    if (enemies.empty()) return -1;

    return rand() % enemies.size();
}

bool EasyAI::hasPlayableCards() const {
    const auto& cards = getHand().getCards();
    return std::any_of(cards.begin(), cards.end(),
        [this](const auto& card) {
            return card && card->getCost() <= getMana();
        });
}

int EasyAI::chooseCardToPlay() const {
    const auto& cards = getHand().getCards();
    std::vector<int> playableIndices;

    for (size_t i = 0; i < cards.size(); ++i) {
        if (cards[i] && cards[i]->getCost() <= getMana()) {
            playableIndices.push_back(i);
        }
    }

    if (playableIndices.empty()) return -1;
    return playableIndices[rand() % playableIndices.size()];
}

UnitCard* EasyAI::findWeakestEnemy() const {
    if (!getOpponent()) return nullptr;

    const auto& enemies = getOpponent()->getBattlefield();
    if (enemies.empty()) return nullptr;

    UnitCard* weakest = enemies.front().get();
    for (const auto& enemy : enemies) {
        if (enemy && enemy->getHealth() < weakest->getHealth()) {
            weakest = enemy.get();
        }
    }
    return weakest;
}
