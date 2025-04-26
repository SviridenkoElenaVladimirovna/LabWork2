#include "Player.h"
#include "../Core/GameState.h"
#include "../Cards/SpellCard.h"
#include <iostream>
#include <algorithm>
#include <memory>
#include "../Systems/BattleSystem.h"

Player::Player(const std::string& name, int health, int mana, GameState* gameState)
        : name(name), health(health), maxHealth(health),
          mana(mana), maxMana(mana), gameState(gameState),
          battleSystem(new BattleSystem(gameState)) {}

Player::~Player() {
    delete battleSystem;
}

void Player::logEvent(const std::string& type, const std::string& details) {
    std::cout << "Event: " << "[" << type << "]: "<< details << "\n";
    if (gameState) {
        // gameState->logEvent(type, name + ": " + details);
    }
}
void Player::cleanBattlefield() {
    auto it = std::remove_if(battlefield.begin(), battlefield.end(),
                             [this](const std::unique_ptr<UnitCard>& unit) {
                                 if (unit && unit->getHealth() <= 0) {
                                     logEvent("Battlefield", unit->getName() + " has died and is removed from battlefield");
                                     if (gameState) {
                                         gameState->getGameHistory()
                                                 .recordEvent(
                                                         GameEvent::of(
                                                                 EventType::FIGHT,
                                                                 "Player " + getName() + "'s unit " + unit->getName() + " has died"
                                                         )
                                                 );
                                     }
                                     return true;
                                 }
                                 return false;
                             }
    );
    battlefield.erase(it, battlefield.end());
}

bool Player::canAffordAttack() const {
    return mana >= 2;  
}
BattleSystem::BattleResult Player::attackWithUnit(size_t attackerIndex, size_t targetIndex) {
    BattleSystem::BattleResult result;

    if (!canAffordAttack()) {
        logEvent("Attack", "Not enough mana for attack (need 2)");
        return result;
    }

    if (attackerIndex >= battlefield.size()) {
        logEvent("Attack", "Invalid attacker index");
        return result;
    }

    auto& attacker = battlefield[attackerIndex];
    if (!attacker || !attacker->canAttackNow()) {
        logEvent("Attack", attacker->getName() + " cannot attack now");
        return result;
    }

    mana -= 2;

    if (targetIndex == std::numeric_limits<size_t>::max()) {
        result = battleSystem->attackHero(*attacker, *opponent);
    } else if (targetIndex < opponent->getBattlefield().size()) {
        auto& target = opponent->getBattlefield()[targetIndex];
        if (target) {
            result = battleSystem->attack(*attacker, *target);
        }
    }

    cleanBattlefield();
    opponent->cleanBattlefield();

    attacker->setExhausted(true);
    logEvent("Attack", attacker->getName() + " attacked and spent 2 mana");

    return result;
}


void Player::addToBattlefield(std::unique_ptr<UnitCard> unit) {
    battlefield.push_back(std::move(unit));
}

void Player::playCard(size_t index) {
    try {
        if (index >= hand.getCards().size()) {
            throw std::runtime_error("Incorrect card index");
        }

        auto& card = hand.getCards()[index];
        if (card->getCost() > mana) {
            throw std::runtime_error("Not enough mana");
        }
        std::string cardName = card->getName();

        card->play(this, opponent);

        mana -= card->getCost();

        hand.removeCard(index);

        logEvent("Card", "plays out " + cardName);
        if (gameState) {
            gameState->getGameHistory().recordEvent(GameEvent::of(EventType::CARD, "Player " + name + " plays " + cardName));
        }
    } catch (const std::exception& e) {
        logEvent("Error", e.what());
        if (gameState) {
            gameState->getGameHistory().recordEvent(GameEvent::of(EventType::SYSTEM, "Exception " + name + ": " + e.what()));
        }
    }
}
void Player::playUnitCard(std::unique_ptr<Card> card) {

    if (auto unit = dynamic_cast<UnitCard*>(card.get())) {
        if (mana >= card->getCost()) {
            mana -= card->getCost();
            unit->onPlay();
            battlefield.push_back(std::unique_ptr<UnitCard>(static_cast<UnitCard*>(card.release())));
            logEvent("Play", "Played unit: " + battlefield.back()->getName());
        }
    } else {
        logEvent("Error", "Attempt to play non-unit card as unit");
    }
}
void Player::startTurn() {
    for (auto& unit : battlefield) {
        unit->refresh();
    }
}

void Player::setInitialMana(int mana) {
    this->mana = mana;
    this->maxMana = mana;
}

bool Player::canAttack() const {
    for (const auto& unit : battlefield) {
        if (unit->canAttackNow()) return true;
    }
    return false;
}

bool Player::drawCard() {
    if (!deck.isEmpty()) {
        auto card = deck.drawCard();
        if (card) {
            hand.addCard(std::move(card));
            const auto& cards = hand.getCards();
            logEvent("Card", "takes the card: " + cards.back()->getName());
            if (gameState) {
                gameState->getGameHistory()
                        .recordEvent(
                                GameEvent::of(
                                        EventType::CARD, "Player " + getName() + " has drawn card " + cards.back()->getName()));
            }
            return true;
        }
    }
    logEvent("Card", "deck is empty");
    return false;
}

void Player::endTurn() {
    for (auto& unit : battlefield) {
        if (unit) {
            unit->setExhausted(false);
        }
    }
    logEvent("Move", "completes the move");
}

void Player::takeDamage(int amount) {
    if (amount <= 0) return;

    int oldHealth = health;
    health = std::max(health - amount, 0);
    logEvent("Damage", "gets " + std::to_string(amount) + " damage");
    if (gameState) {
        gameState->getGameHistory()
                .recordEvent(
                        GameEvent::of(
                                EventType::DAMAGE,
                                "Player " + getName() + " get " + std::to_string(amount) +
                                " amount of damage (" + std::to_string(oldHealth) + " → " +
                                std::to_string(health) + ")"));
    }
    displayHealthStatus();
}

int Player::heal(int amount) {
    int oldHealth = health;
    health = std::min(health + amount, maxHealth);
    int healed = health - oldHealth;

    if (healed > 0) {
        logEvent("Heal", "recovers " + std::to_string(healed) + " health");
        if (gameState) {
            gameState->getGameHistory()
                    .recordEvent(
                            GameEvent::of(
                                    EventType::HEALING, "Player " + getName() + " healed " + std::to_string(healed) +
                                                        " HP (" + std::to_string(oldHealth) + " → " + std::to_string(health) + ")"));
        }
        displayHealthStatus();
    } else {
        logEvent("Heal", "health is already maxed out.");
    }

    return healed;
}
void Player::removeDeadUnits() {
    auto it = std::remove_if(
            battlefield.begin(),
            battlefield.end(),
            [this](const auto &unit) {
                if (unit->getHealth() <= 0) {
                    logEvent("Fighting", unit->getName() + " dies");
                    if (gameState) {
                        gameState->getGameHistory()
                                .recordEvent(
                                        GameEvent::of(
                                                EventType::FIGHT, "Player's " + getName() + " unit " + unit->getName() + " dies"));
                    }
                    return true;
                }
                return false;
            });
    battlefield.erase(it, battlefield.end());
}

void Player::displayHealthStatus() const {
    std::cout << "Health: " << health << "/" << maxHealth << "\n";
}

int Player::getAttackCost(const UnitCard& unit) const {
    return std::max(1, unit.getAttack() / 2);
}

std::vector<UnitCard*> Player::getAttackableUnits() const {
    std::vector<UnitCard*> attackableUnits;
    for (const auto& unit : battlefield) {
        if (unit->canAttackNow() && !unit->isExhausted()) {
            attackableUnits.push_back(unit.get());
        }
    }
    return attackableUnits;
}

int Player::selectAttacker(const std::vector<UnitCard*>& units) const {
    if (units.empty()) return -1;

    std::cout << "\nChoose an attacking creature:\n";
    for (size_t i = 0; i < units.size(); ++i) {
        std::cout << i+1 << ". ";
        units[i]->displayInfo();
        std::cout << " (Attack: " << units[i]->getAttack() << ")\n";
    }

    return getValidatedChoice(1, units.size(), "Your choice: ") - 1;
}

std::pair<Card*, Player*> Player::selectAttackTarget(Player* opponent) const {
    if (!opponent) return {nullptr, nullptr};

    std::cout << "\nSelect the target of the attack:\n";

    const auto& enemyUnits = opponent->getBattlefield();
    for (size_t i = 0; i < enemyUnits.size(); ++i) {
        std::cout << i+1 << ". ";
        enemyUnits[i]->displayInfo();
        std::cout << "\n";
    }

    std::cout << enemyUnits.size()+1 << ". Attack the hero ("
              << opponent->getName() << ")\n";

    int choice = getValidatedChoice(1, enemyUnits.size()+1, "Your choice: ");

    if (choice <= enemyUnits.size()) {
        return {enemyUnits[choice-1].get(), nullptr};
    } else {
        return {nullptr, opponent};
    }
}

int Player::getValidatedChoice(int min, int max, const std::string& prompt) const {
    while (true) {
        std::cout << prompt;
        std::string input;
        std::getline(std::cin, input);

        try {
            int choice = std::stoi(input);
            if (choice >= min && choice <= max) {
                return choice;
            }
        } catch (...) {
        }
        std::cout << "Wrong choice. Enter a number from " << min << " to " << max << ".\n";
    }
}
