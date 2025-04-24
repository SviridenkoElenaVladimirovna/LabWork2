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

void Player::attackWithUnit(size_t attackerIndex, size_t targetIndex) {
    if (attackerIndex >= battlefield.size()) return;

    if (targetIndex == std::numeric_limits<size_t>::max()) {
        battleSystem->attackHero(*battlefield[attackerIndex], *opponent);
    }
    else if (targetIndex < opponent->getBattlefield().size()) {
        battleSystem->attack(*battlefield[attackerIndex],
                             *opponent->getBattlefield()[targetIndex]);
    }

    battlefield[attackerIndex]->setExhausted(true);
    logEvent("attack", battlefield[attackerIndex]->getName() + " attacked");
}

void Player::addToBattlefield(std::unique_ptr<UnitCard> unit) {
    battlefield.push_back(std::move(unit));
}

void Player::playCard(size_t index) {
    try {
        if (index >= hand.getCards().size()) {
            throw std::runtime_error("Incorrect card index");
        }

        auto card = std::move(hand.playCard(index));
        if (!card) {
            throw std::runtime_error("Failed to play a card");
        }

        if (card->getCost() > mana) {
            hand.addCard(std::move(card));
            throw std::runtime_error("Not enough mana");
        }

        mana -= card->getCost();
        logEvent("Card", "plays out " + card->getName());
        if (gameState) {
            gameState->getGameHistory().recordEvent(GameEvent::of(EventType::CARD, "Player " + name + " plays " + card->getName()));
        }

        card->play(this, opponent);
    } catch (const std::exception& e) {
        logEvent("Error", e.what());
        if (gameState) {
            gameState->getGameHistory().recordEvent(GameEvent::of(EventType::SYSTEM, "Exception " + name + ": " + e.what()));
        }
    }
}

void Player::playUnitCard(std::unique_ptr<UnitCard> card) {
    if (mana >= card->getCost()) {
        mana -= card->getCost();
        card->onPlay();
        battlefield.push_back(std::move(card));
    }
}

void Player::startTurn() {
    if (maxMana < 10) {
        maxMana++;
    }

    gameState->getGameHistory().recordEvent(GameEvent::of(EventType::SYSTEM, "Player " + getName() + " increased mana to " + std::to_string(getMana())));

    mana = maxMana;

    for (const auto& unit : battlefield) {
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

void Player::cleanBattlefield() {
    auto& battlefield = getBattlefield();
    battlefield.erase(
            std::remove_if(battlefield.begin(), battlefield.end(),
                           [](const auto& unit) { return unit->getHealth() <= 0; }),
            battlefield.end()
    );
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