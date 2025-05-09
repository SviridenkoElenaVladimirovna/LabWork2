#include "HardAI.h"
#include "../Cards/SpellCard.h"
#include <climits>
#include <iostream>
HardAI::HardAI(const std::string& name, int health, int mana, GameState* gameState, UIManager* uiManager)
        : AI(name, health, mana, gameState, uiManager) {}
bool HardAI::shouldPlayCard(const Card* card) const {

    if (card->getType() == CardType::UNIT) {
        return true;
    }

    if (card->getType() == CardType::SPELL) {
        auto spell = dynamic_cast<const SpellCard*>(card);
        if (spell->getEffect() == SpellEffect::DAMAGE) {
            for (const auto& enemy : getOpponent()->getBattlefield()) {
                if (enemy->getHealth() <= spell->getPower()) {
                    return true;
                }
            }
            if (getOpponent()->getBattlefield().size() > getBattlefield().size()) {
                return true;
            }
        }
    }
    return false;
}

int HardAI::evaluateCard(const Card* card) const {
    if (card->getType() == CardType::UNIT) {
        auto unit = dynamic_cast<const UnitCard*>(card);
        return unit->getAttack() * 3 + unit->getHealth() * 2;
    }
    else if (card->getType() == CardType::SPELL) {
        auto spell = dynamic_cast<const SpellCard*>(card);
        if (spell->getEffect() == SpellEffect::DAMAGE) {
            int killCount = 0;
            for (const auto& enemy : getOpponent()->getBattlefield()) {
                if (enemy->getHealth() <= spell->getPower()) {
                    killCount++;
                }
            }
            if (killCount > 0) {
                return 1500 + killCount * 700;
            }
            if (getOpponent()->getBattlefield().size() > getBattlefield().size()) {
                return 1000;
            }
            return 500;
        }
    }
    return card->getCost() * 2;
}
void HardAI::takeTurn() {
    if (ui) {
        ui->displayMessage(getName() + " makes a move...");
    }

    bool cardPlayed = false;
    int bestCardIndex = chooseCardToPlay();

    if (bestCardIndex != -1) {
        auto& hand = getHandRef();
        auto card = hand.getCards()[bestCardIndex].get();
        int cardCost = card->getCost();

        try {
            if (auto* spellCard = dynamic_cast<SpellCard*>(card)) {
                spellCard->play(this, getOpponent());
                if (ui) {
                    ui->showCardPlayedMessage(spellCard->getName(), true);
                }
            } else {
                playCard(bestCardIndex);
                if (ui) {
                    ui->showCardPlayedMessage(card->getName(), false);
                }
            }

            hand.removeCard(bestCardIndex);
            setMana(getMana() - cardCost);
            cardPlayed = true;
        } catch (const std::exception& e) {
            logEvent("AI Error", "Failed to play card: " + std::string(e.what()));
        }
    }

    while (hasPlayableCards() && getMana() > 0) {
        int cardIndex = chooseCardToPlay();
        if (cardIndex == -1) break;

        auto& hand = getHandRef();
        auto card = hand.getCards()[cardIndex].get();
        int cardCost = card->getCost();

        try {
            if (shouldPlayCard(card)) {
                if (auto* spellCard = dynamic_cast<SpellCard*>(card)) {
                    spellCard->play(this, getOpponent());
                } else {
                    playCard(cardIndex);
                }

                hand.removeCard(cardIndex);
                setMana(getMana() - cardCost);
            }
        } catch (...) {
            break;
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

int HardAI::evaluateAttack(UnitCard* attacker, UnitCard* target) const {
    bool canKill = target->getHealth() <= attacker->getAttack();

    bool willSurvive = attacker->getHealth() > target->getAttack();

    if (canKill) {
        int killPriority = 10000;
        if (!willSurvive) {
            killPriority -= 2000;
        }

        killPriority += target->getAttack() * 3 + target->getHealth();

        return killPriority;
    }

    int value = (attacker->getAttack() - target->getHealth()) -
                (target->getAttack() - attacker->getHealth());

    value += target->getAttack() * 2 + target->getHealth();

    return value;
}



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


int HardAI::chooseUnitToAttackWith() const {
    const auto& battlefield = getBattlefield();
    for (size_t i = 0; i < battlefield.size(); ++i) {
        if (battlefield[i]->canAttackNow()) {
            return static_cast<int>(i);
        }
    }
    return -1;
}
