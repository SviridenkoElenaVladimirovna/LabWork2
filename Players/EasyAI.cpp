#include "EasyAI.h"
#include "../Cards/SpellCard.h"
#include "../Core/GameEngine.h"
#include <iostream>
#include <cstdlib>
#include <algorithm>

EasyAI::EasyAI(const std::string& name, int health, int mana, GameState* gameState, UIManager* uiManager)
        : AI(name, health, mana, gameState, uiManager) {}

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

void EasyAI::playSelectedCard(int cardIndex) {
    Hand& hand = const_cast<Hand&>(getHand());
    auto* gameEngine = dynamic_cast<GameEngine*>(getGameState());

    if (!gameEngine || cardIndex < 0 || static_cast<size_t>(cardIndex) >= hand.getCards().size()) {
        return;
    }

    const auto& handCards = hand.getCards();
    auto cardName = handCards[cardIndex]->getName();
    bool isSpell = dynamic_cast<SpellCard*>(handCards[cardIndex].get()) != nullptr;
    int cardCost = handCards[cardIndex]->getCost();

    try {
        if (auto* spellCard = dynamic_cast<SpellCard*>(handCards[cardIndex].get())) {
            spellCard->play(this, gameEngine->getOpponent());
        } else if (auto* unitCard = dynamic_cast<UnitCard*>(handCards[cardIndex].get())) {
            playCard(cardIndex);
        }

        hand.removeCard(cardIndex);
        setMana(getMana() - cardCost);

        logEvent("AI Card", "plays " + cardName);
        if (getGameState()) {
            getGameState()->getGameHistory().recordEvent(
                    GameEvent::of(EventType::CARD,
                                  "AI " + getName() + " plays " + cardName)
            );
        }

        if (ui) {
            ui->showCardPlayedMessage(cardName, isSpell);
        }
    } catch (const std::exception& e) {
        logEvent("AI Error", "Failed to play card: " + std::string(e.what()));
    }
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
    if (ui) {
        ui->displayMessage(getName() + " makes a move...");
    }

    try {
        while (hasPlayableCards()) {
            int cardIndex = chooseCardToPlay();
            if (cardIndex == -1) break;

            const auto& cards = getHand().getCards();
            if (cardIndex >= 0 && static_cast<size_t>(cardIndex) < cards.size()) {
                if (auto spell = dynamic_cast<SpellCard*>(cards[cardIndex].get())) {
                    if (spell->getEffect() == SpellEffect::DRAW && getDeck().isEmpty()) {
                        logEvent("AI", "Skip drawing - deck is empty");
                        continue;
                    }
                }
                playSelectedCard(cardIndex);
            }
        }

        while (true) {
            int unitIndex = chooseUnitToAttackWith();
            if (unitIndex == -1) break;

            const auto& battlefield = getBattlefield();
            if (unitIndex >= 0 && static_cast<size_t>(unitIndex) < battlefield.size()) {
                if (battlefield[unitIndex] && battlefield[unitIndex]->canAttackNow()) {
                    attackWithUnit(unitIndex);
                }
            }
        }
    }
    catch (const std::exception& e) {
        logEvent("AI Error", e.what());
        if (getGameState()) {
            getGameState()->getGameHistory().recordEvent(
                    GameEvent::of(EventType::SYSTEM, "AI error: " + std::string(e.what()))
            );
        }
    }

    if (ui) {
        ui->displayMessage(getName() + " ends turn.");
    }
}