#include "Player.h"
#include "../Core/GameEngine.h"
#include "../Cards/SpellCard.h"
#include <iostream>
#include <algorithm>
#include <memory>

Player::Player(const std::string& name, int health, int mana, GameState* gameState)
        : name(name), health(health), maxHealth(health),
          mana(mana), maxMana(mana), gameState(gameState),
          battleSystem(new BattleSystem(gameState)) {}

Player::~Player() {
    delete battleSystem;
}

void Player::logEvent(const std::string& type, const std::string& details) {
    std::cout << "Event: " << "[" << type << "]: "<< details << "\n";
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
        logEvent("Attack", "Not enough mana for attack (requires 2)");
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
    }
    else if (targetIndex < opponent->getBattlefield().size()) {
        auto& target = opponent->getBattlefield()[targetIndex];
        if (target) {
            result = battleSystem->attack(*attacker, *target);
        }
    }

    attacker->setExhausted(true);
    logEvent("Attack", attacker->getName() + " attacked and spent 2 mana");

    return result;
}

void Player::playCard(size_t index) {
    try {
        if (index >= hand.getCards().size()) {
            throw std::runtime_error("Invalid card index");
        }

        auto& card = hand.getCards()[index];
        if (!card) {
            throw std::runtime_error("Null card in hand");
        }

        if (card->getCost() > mana) {
            throw std::runtime_error("Not enough mana");
        }
        if (auto spell = dynamic_cast<SpellCard*>(card.get())) {
            if (spell->getEffect() == SpellEffect::DRAW && getDeck().isEmpty()) {
                throw std::runtime_error("Cannot draw - deck is empty");
            }
        }

        std::string cardName = card->getName();
        card->play(this, opponent);
        mana -= card->getCost();
        hand.removeCard(index);

        logEvent("Card", "plays out " + cardName);
        if (gameState) {
            gameState->getGameHistory().recordEvent(
                    GameEvent::of(EventType::CARD,
                                  "Player " + name + " plays " + cardName)
            );
        }
    } catch (const std::exception& e) {
        logEvent("Error", e.what());
        if (gameState) {
            gameState->getGameHistory().recordEvent(
                    GameEvent::of(EventType::SYSTEM,
                                  "Play card error: " + std::string(e.what()))
            );
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
    if (mana < 2) return false;

    for (const auto& unit : battlefield) {
        if (unit && unit->canAttackNow() && !unit->isDead()) {
            return true;
        }
    }
    return false;
}
bool Player::hasPlayableCards() const {
    const auto& cards = hand.getCards();
    return std::any_of(cards.begin(), cards.end(),
                       [this](const auto& card) {
                           return card->getCost() <= mana;
                       });
}

bool Player::drawCard() {
    if (!deck.isEmpty()) {
        auto card = deck.drawCard();
        if (card) {
            std::string name = card->getName();
            hand.addCard(std::move(card));
            logEvent("Card", "takes the card: " + name);
            if (gameState) {
                gameState->getGameHistory().recordEvent(
                        GameEvent::of(EventType::CARD, "Player " + getName() + " has drawn card " + name));
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
    if (amount <= 0 || isDefeated()) return;

    int oldHealth = health;
    health = std::max(health - amount, 0);
    logEvent("Damage", "gets " + std::to_string(amount) + " damage");

    if (gameState) {
        gameState->getGameHistory().recordEvent(
                GameEvent::of(
                        EventType::DAMAGE,
                        "Player " + getName() + " took " + std::to_string(amount) +
                        " damage (" + std::to_string(oldHealth) + " → " +
                        std::to_string(health) + ")"
                )
        );

        if (health <= 0) {
            GameEngine* engine = dynamic_cast<GameEngine*>(gameState);
                engine->checkGameOver();

        }
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

void Player::displayHealthStatus() const {
    std::cout << "Health: " << health << "/" << maxHealth << "\n";
}
void Player::setMaxMana(int newMaxMana) {
    maxMana = newMaxMana;
    mana = std::min(mana, maxMana);
}

void Player::setMana(int newMana) {
    mana = std::min(newMana, maxMana);
}
std::vector<std::unique_ptr<UnitCard>>& Player::getBattlefield() {
    return battlefield;
}

const std::vector<std::unique_ptr<UnitCard>>& Player::getBattlefield() const {
    return battlefield;
}

const Hand& Player::getHand() const {
    return hand;
}

Hand& Player::getHandRef() {
    return hand;
}

int Player::getMaxMana() const {
    return maxMana;
}

int Player::getMana() const {
    return mana;
}
bool Player::isDefeated() const {
    return health <= 0;
}
GameState* Player::getGameState() const {
    return gameState;
}
int Player::getMaxHealth() const {
    return maxHealth;
}
const std::string& Player::getName() const {
    return name;
}

int Player::getHealth() const {
    return health;
}

Deck& Player::getDeck() {
    return deck;
}

Player* Player::getOpponent() const {
    return opponent;
}

void Player::setOpponent(Player* newOpponent) {
    opponent = newOpponent;
}
