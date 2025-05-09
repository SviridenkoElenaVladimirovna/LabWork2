#include "MediumAI.h"
#include "../Cards/SpellCard.h"
#include "../Core/GameEngine.h"
#include <iostream>
#include <climits>
#include <algorithm>

MediumAI::MediumAI(const std::string& name, int health, int mana, GameState* gameState, UIManager* uiManager)
        : AI(name, health, mana, gameState, uiManager) {}

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

bool MediumAI::hasPlayableCards() const {
    const auto& cards = getHand().getCards();
    return std::any_of(cards.begin(), cards.end(),
                       [this](const auto& card) {
                           return card->getCost() <= getMana() && !card->isPlayed();
                       });
}

bool MediumAI::shouldPlayCard(const Card* card) const {
    if (card->getType() == CardType::UNIT) {
        return true;
    }

    if (card->getType() == CardType::SPELL) {
        auto spell = dynamic_cast<const SpellCard*>(card);
        if (spell && spell->getEffect() == SpellEffect::DAMAGE) {
            for (const auto& enemy : getOpponent()->getBattlefield()) {
                if (enemy->getHealth() <= spell->getPower()) {
                    return true;
                }
            }
            if (getBattlefield().size() < 3) {
                return true;
            }
        }
    }
    return false;
}

int MediumAI::chooseCardToPlay() const {
    const auto& cards = getHand().getCards();
    int bestIndex = -1;
    int bestScore = INT_MIN;
    int currentMana = getMana();

    for (size_t i = 0; i < cards.size(); ++i) {
        if (cards[i]->getCost() <= currentMana && !cards[i]->isPlayed()) {
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
    if (card->getType() == CardType::UNIT) {
        auto unit = dynamic_cast<const UnitCard*>(card);
        return (unit->getAttack() * 3 + unit->getHealth() * 2) - unit->getCost();
    }
    else if (card->getType() == CardType::SPELL) {
        auto spell = dynamic_cast<const SpellCard*>(card);
        if (spell->getEffect() == SpellEffect::DAMAGE) {
            int killPotential = 0;
            for (const auto& enemy : getOpponent()->getBattlefield()) {
                if (enemy->getHealth() <= spell->getPower()) {
                    killPotential += 200;
                }
            }
            if (getOpponent()->getBattlefield().size() > getBattlefield().size()) {
                killPotential += 100;
            }
            return killPotential + 50 - spell->getCost();
        }
    }
    return 0;
}

void MediumAI::playSelectedCard(int cardIndex) {
    Hand& hand = getHandRef();
    auto* gameEngine = dynamic_cast<GameEngine*>(getGameState());

    if (!gameEngine || cardIndex < 0 || static_cast<size_t>(cardIndex) >= hand.getCards().size()) {
        return;
    }

    const auto& handCards = hand.getCards();
    auto card = handCards[cardIndex].get();
    int cardCost = card->getCost();

    if (cardCost > getMana()) {
        return;
    }
    try {
        if (auto* spellCard = dynamic_cast<SpellCard*>(card)) {
            spellCard->play(this, getOpponent());
            if (ui) {
                ui->showCardPlayedMessage(spellCard->getName(), true);
            }
        } else {
            playCard(cardIndex);
            if (ui) {
                ui->showCardPlayedMessage(card->getName(), false);
            }
        }
    } catch (const std::exception& e) {
        logEvent("AI Error", "Failed to play card: " + std::string(e.what()));
    }
}
void MediumAI::takeTurn() {
    if (ui) {
        ui->displayMessage(getName() + " makes a move...");
    }

    while (hasPlayableCards()) {
        int cardIndex = chooseCardToPlay();
        if (cardIndex == -1) break;

        auto& hand = getHandRef();
        auto card = hand.getCards()[cardIndex].get();
        int cardCost = card->getCost();

        try {
            if (shouldPlayCard(card)) {
                if (auto* spellCard = dynamic_cast<SpellCard*>(card)) {
                    spellCard->play(this, getOpponent());
                    std::cout << "Spell cast: " << spellCard->getName() << std::endl;
                } else {
                    playCard(cardIndex);
                    std::cout << "Played unit: " << card->getName() << std::endl;
                }

                hand.removeCard(cardIndex);
                setMana(getMana() - cardCost);
            }
        } catch (...) {
            continue;
        }
    }

    while (canAttack()) {
        int attackerIndex = chooseUnitToAttackWith();
        if (attackerIndex == -1) break;

        int targetIndex = chooseAttackTarget(attackerIndex);
        if (targetIndex != -1) {
            battleSystem->attack(
                    *getBattlefield()[attackerIndex].get(),
                    *getOpponent()->getBattlefield()[targetIndex].get());
        } else {
            getBattlefield()[attackerIndex]->attackPlayer(getOpponent());
        }
    }

    if (ui) {
        ui->displayMessage(getName() + " ends turn.");
    }
}
int MediumAI::chooseUnitToAttackWith() const {
    const auto& battlefield = getBattlefield();
    for (size_t i = 0; i < battlefield.size(); ++i) {
        if (battlefield[i]->canAttackNow() && !battlefield[i]->isDead()) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

UnitCard* MediumAI::findWeakestEnemy() const {
    const auto& enemies = getOpponent()->getBattlefield();
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
