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
    if (!ui) return;
    ui->displayMessage(getName() + " makes a calculated move...");

    bool shouldPlayAggressive = shouldPlayAggressively();

    playCardsWithStrategy(shouldPlayAggressive);

    executeOptimizedAttacks(shouldPlayAggressive);

    cleanBattlefield();
    if (getOpponent()) getOpponent()->cleanBattlefield();

    if (ui) ui->displayMessage(getName() + " ends turn.");
}

bool HardAI::shouldPlayAggressively() const {

    int myUnitCount = getBattlefield().size();
    int opponentUnitCount = getOpponent() ? getOpponent()->getBattlefield().size() : 0;

    bool hasMoreUnits = myUnitCount > opponentUnitCount;
    bool hasHealthAdvantage = getHealth() > (getOpponent() ? getOpponent()->getHealth() + 5 : 10);
    bool opponentHasWeakUnits = false;

    if (getOpponent()) {
        for (const auto& unit : getOpponent()->getBattlefield()) {
            if (unit && unit->getHealth() <= 2) {
                opponentHasWeakUnits = true;
                break;
            }
        }
    }

    return hasMoreUnits || hasHealthAdvantage || opponentHasWeakUnits;
}

void HardAI::playCardsWithStrategy(bool aggressiveStrategy) {
    while (hasPlayableCards()) {
        int bestCardIndex = findBestCardToPlay(aggressiveStrategy);
        if (bestCardIndex == -1) break;

        try {
            auto& hand = getHandRef();
            if (bestCardIndex < 0 || static_cast<size_t>(bestCardIndex) >= hand.getCards().size()) {
                logEvent("AI Warning", "Invalid card index");
                break;
            }

            auto* card = hand.getCards()[bestCardIndex].get();
            if (!card || card->getCost() > getMana()) continue;

            bool isSpell = dynamic_cast<SpellCard*>(card) != nullptr;
            std::string cardName = card->getName();

            if (isSpell) {
                auto* spellCard = dynamic_cast<SpellCard*>(card);
                if (spellCard) {
                    if (spellCard->getEffect() == SpellEffect::DAMAGE) {
                        int bestTarget = findBestDamageTarget(spellCard->getPower());
                        if (bestTarget != -1 && getOpponent()) {
                            spellCard->play(this, getOpponent());
                        }
                    } else {
                        spellCard->play(this, getOpponent());
                    }
                }
            } else {
                playCard(bestCardIndex);
            }

            setMana(getMana() - card->getCost());
            hand.removeCard(bestCardIndex);

            if (ui) ui->showCardPlayedMessage(cardName, isSpell);

        } catch (...) {
            logEvent("AI Error", "Failed to play card");
        }
    }
}

int HardAI::findBestCardToPlay(bool aggressiveStrategy) const {
    const auto& cards = getHand().getCards();
    int bestIndex = -1;
    int bestScore = INT_MIN;

    for (size_t i = 0; i < cards.size(); ++i) {
        if (cards[i]->getCost() <= getMana()) {
            int score = evaluateCardForStrategy(cards[i].get(), aggressiveStrategy);
            if (score > bestScore) {
                bestScore = score;
                bestIndex = i;
            }
        }
    }

    return bestIndex;
}

int HardAI::evaluateCardForStrategy(const Card* card, bool aggressiveStrategy) const {
    if (!card) return 0;

    int baseValue = evaluateCard(card);

    if (card->getType() == CardType::UNIT) {
        auto* unit = dynamic_cast<const UnitCard*>(card);
        if (unit) {
            if (aggressiveStrategy) {
                baseValue += unit->getAttack() * 2;
            } else {
                baseValue += unit->getHealth() * 2;
            }

            if (getBattlefield().size() < 2) {
                baseValue += 300;
            }
        }
    } else if (card->getType() == CardType::SPELL) {
        auto* spell = dynamic_cast<const SpellCard*>(card);
        if (spell) {
            if (spell->getEffect() == SpellEffect::DAMAGE) {
                int killPotential = countPotentialKills(spell->getPower());
                baseValue += killPotential * 500;

                if (aggressiveStrategy) {
                    baseValue += 200;
                }
            }
        }
    }

    return baseValue;
}

int HardAI::countPotentialKills(int damage) const {
    if (!getOpponent()) return 0;

    int killCount = 0;
    for (const auto& unit : getOpponent()->getBattlefield()) {
        if (unit && unit->getHealth() <= damage) {
            killCount++;
        }
    }
    return killCount;
}

int HardAI::findBestDamageTarget(int damage) const {
    if (!getOpponent()) return -1;

    int bestIndex = -1;
    int bestValue = -1;

    const auto& enemies = getOpponent()->getBattlefield();
    for (size_t i = 0; i < enemies.size(); ++i) {
        if (!enemies[i]) continue;

        int value = enemies[i]->getAttack() * 2 + enemies[i]->getHealth();

        if (enemies[i]->getHealth() <= damage) {
            value += 500;
        }

        if (value > bestValue) {
            bestValue = value;
            bestIndex = i;
        }
    }

    return bestIndex;
}

std::vector<std::pair<size_t, int>> HardAI::getAttackOrder() const {
    std::vector<std::pair<size_t, int>> attackOrder;
    const auto& battlefield = getBattlefieldRef();

    for (size_t i = 0; i < battlefield.size(); ++i) {
        if (battlefield[i] && battlefield[i]->canAttackNow()) {
            int attackScore = battlefield[i]->getAttack() * 2 + battlefield[i]->getHealth();
            attackOrder.emplace_back(i, attackScore);
        }
    }

    std::sort(attackOrder.begin(), attackOrder.end(),
              [](const auto& a, const auto& b) { return a.second > b.second; });

    return attackOrder;
}

const std::vector<std::unique_ptr<UnitCard>>& HardAI::getBattlefieldRef() const {
    return getBattlefield();
}

void HardAI::executeOptimizedAttacks(bool aggressiveStrategy) {
    auto opponent = getOpponent();
    if (!opponent || !battleSystem) return;

    auto attackOrder = getAttackOrder();
    const auto& battlefield = getBattlefieldRef();
    const auto& enemyBattlefield = opponent->getBattlefield();

    for (const auto& [index, score] : attackOrder) {
        try {
            int targetIndex = chooseOptimalAttackTarget(index, aggressiveStrategy);

            if (targetIndex != -1 && targetIndex < static_cast<int>(enemyBattlefield.size())) {
                battleSystem->attack(
                        *battlefield[index],
                        *enemyBattlefield[targetIndex]);
            } else {
                battlefield[index]->attackPlayer(opponent);
            }
        } catch (...) {
            logEvent("AI Error", "Attack failed");
        }
    }
}
int HardAI::chooseOptimalAttackTarget(int unitIndex, bool aggressiveStrategy) const {
    auto* attacker = getBattlefield()[unitIndex].get();
    if (!attacker) return -1;

    const auto& enemies = getOpponent()->getBattlefield();
    if (enemies.empty()) return -1;

    int bestScore = INT_MIN;
    int bestIndex = -1;

    for (size_t i = 0; i < enemies.size(); ++i) {
        if (!enemies[i]) continue;

        int score = evaluateAttack(attacker, enemies[i].get());


        if (aggressiveStrategy) {
            score -= 100;
        } else {
            score += 200;
        }

        if (enemies[i]->getAttack() >= attacker->getHealth()) {
            score += 500;
        }

        if (score > bestScore) {
            bestScore = score;
            bestIndex = i;
        }
    }

    if (aggressiveStrategy && bestScore < 1000 && attacker->getAttack() >= 3) {
        return -1;
    }

    return bestIndex;
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
