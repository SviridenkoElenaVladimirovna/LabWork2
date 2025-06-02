/*
Sviridenko Elena st130482@student.spbu.ru
The system under development is a C++ console card game inspired by Hearthstone. 
*/
#include "MediumAI.h"
#include "../Cards/SpellCard.h"
#include <algorithm>
#include <climits>

MediumAI::MediumAI(const std::string& name, int health, int mana,
                   GameState* gameState, UIManager* uiManager)
        : AI(name, health, mana, gameState, uiManager) {}

void MediumAI::takeTurn() {
    if (!ui) return;
    ui->displayMessage(getName() + " makes a move...");

    playCards();

    performAttacks();

    cleanup();

    if (ui) ui->displayMessage(getName() + " ends turn.");
}

int MediumAI::chooseUnitToAttackWith() const {
    const auto& units = getBattlefield();
    for (size_t i = 0; i < units.size(); ++i) {
        if (units[i] && units[i]->canAttackNow()) {
            return static_cast<int>(i);
        }
    }
    return -1;
}
void MediumAI::playCards() {
    const int maxPlayAttempts = 3;
    int attempts = 0;

    while (attempts++ < maxPlayAttempts && hasPlayableCards()) {
        int cardIndex = chooseCardToPlay();
        if (cardIndex == -1) break;

        try {
            auto& hand = getHandRef();
            if (cardIndex < 0 || static_cast<size_t>(cardIndex) >= hand.getCards().size()) {
                continue;
            }

            auto* card = hand.getCards()[cardIndex].get();
            if (!card || card->getCost() > getMana()) {
                continue;
            }

            if (shouldPlayCard(card)) {
                bool isSpell = dynamic_cast<SpellCard*>(card) != nullptr;
                std::string cardName = card->getName();

                if (isSpell) {
                    if (auto* spellCard = dynamic_cast<SpellCard*>(card)) {
                        spellCard->play(this, getOpponent());
                    }
                } else {
                    playCard(cardIndex);
                }

                setMana(getMana() - card->getCost());
                hand.removeCard(cardIndex);

                if (ui) ui->showCardPlayedMessage(cardName, isSpell);
            }
        } catch (...) {
            logEvent("AI Error", "Failed to play card");
        }
    }
}
void MediumAI::performAttacks() {
    auto opponent = getOpponent();
    if (!opponent || !battleSystem) return;

    const auto& battlefield = getBattlefield();
    const auto& enemies = opponent->getBattlefield();

    for (size_t i = 0; i < battlefield.size(); ++i) {
        if (!battlefield[i] || !battlefield[i]->canAttackNow() || battlefield[i]->isDead()) {
            continue;
        }

        if (enemies.empty()) {
            battleSystem->attackHero(*battlefield[i], *opponent);
            continue;
        }

        int weakestIndex = -1;
        int minHealth = INT_MAX;
        for (size_t j = 0; j < enemies.size(); ++j) {
            if (enemies[j] && enemies[j]->getHealth() < minHealth) {
                weakestIndex = j;
                minHealth = enemies[j]->getHealth();
            }
        }

        if (weakestIndex != -1) {
            battleSystem->attack(*battlefield[i], *enemies[weakestIndex]);
        }
    }
}

void MediumAI::cleanup() {
    cleanBattlefield();
    if (auto opponent = getOpponent()) {
        opponent->cleanBattlefield();
    }
}

bool MediumAI::shouldPlayCard(const Card* card) const {
    if (!card) return false;

    if (card->getType() == CardType::UNIT) {
        return true;
    }

    if (card->getType() == CardType::SPELL) {
        auto* spell = dynamic_cast<const SpellCard*>(card);
        if (spell && spell->getEffect() == SpellEffect::DAMAGE) {
            auto opponent = getOpponent();
            if (!opponent) return false;

            for (const auto& enemy : opponent->getBattlefield()) {
                if (enemy && enemy->getHealth() <= spell->getPower()) {
                    return true;
                }
            }

            return opponent->getBattlefield().size() > getBattlefield().size();
        }
    }
    return false;
}

int MediumAI::chooseAttackTarget(int attackingUnitIndex) const {
    if (attackingUnitIndex < 0 || attackingUnitIndex >= static_cast<int>(getBattlefield().size())) {
        return -1;
    }
    UnitCard* weakest = findWeakestEnemy();
    if (!weakest) return -1;

    auto opponent = getOpponent();
    if (!opponent) return -1;

    const auto& enemies = opponent->getBattlefield();
    for (size_t i = 0; i < enemies.size(); ++i) {
        if (enemies[i].get() == weakest) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

UnitCard* MediumAI::findWeakestEnemy() const {
    auto opponent = getOpponent();
    if (!opponent) return nullptr;

    const auto& enemies = opponent->getBattlefield();
    if (enemies.empty()) return nullptr;

    UnitCard* weakest = nullptr;
    int minHealth = INT_MAX;

    for (const auto& enemy : enemies) {
        if (enemy && !enemy->isDead() && enemy->getHealth() < minHealth) {
            weakest = enemy.get();
            minHealth = enemy->getHealth();
        }
    }
    return weakest;
}

int MediumAI::chooseCardToPlay() const {
    const auto& cards = getHand().getCards();
    int bestIndex = -1;
    int bestScore = INT_MIN;

    for (size_t i = 0; i < cards.size(); ++i) {
        if (cards[i] && cards[i]->getCost() <= getMana()) {
            int score = evaluateCard(cards[i].get());
            if (score > bestScore) {
                bestScore = score;
                bestIndex = static_cast<int>(i);
            }
        }
    }
    return bestIndex;
}

int MediumAI::evaluateCard(const Card* card) const {
    if (!card) return 0;

    if (card->getType() == CardType::UNIT) {
        auto* unit = dynamic_cast<const UnitCard*>(card);
        if (unit) {
            return (unit->getAttack() * 3 + unit->getHealth() * 2) - unit->getCost();
        }
    }
    else if (card->getType() == CardType::SPELL) {
        auto* spell = dynamic_cast<const SpellCard*>(card);
        if (spell && spell->getEffect() == SpellEffect::DAMAGE) {
            int killPotential = 0;
            auto opponent = getOpponent();
            if (opponent) {
                for (const auto& enemy : opponent->getBattlefield()) {
                    if (enemy && enemy->getHealth() <= spell->getPower()) {
                        killPotential += 200;
                    }
                }
                if (opponent->getBattlefield().size() > getBattlefield().size()) {
                    killPotential += 100;
                }
            }
            return killPotential + 50 - spell->getCost();
        }
    }
    return 0;
}
