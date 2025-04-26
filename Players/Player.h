#ifndef PLAYER_H
#define PLAYER_H

#include "../Cards/Card.h"
#include "../Cards/UnitCard.h"
#include "../Systems/Deck.h"
#include "../Systems/BattleSystem.h"
#include "../Systems/Hand.h"
#include "../Core/events/GameEvent.h"
#include <vector>
#include <memory>
#include <string>

#include "../Core/GameState.h"

class GameState;


class Player {
protected:
    std::string name;
    int health;
    GameState* gameState;
    int maxHealth;
    int mana;
    int maxMana;
    Hand hand;
    std::vector<std::unique_ptr<UnitCard>> battlefield;
    Deck deck;
    Player* opponent;
    BattleSystem* battleSystem;
    std::vector<GameEvent> eventLog;

    void logEvent(const std::string& type, const std::string& details);

public:
    Player(const std::string& name, int health, int mana, GameState* gameState);
    virtual ~Player();
    void setMaxMana(int newMaxMana) {
        maxMana = newMaxMana;
        mana = std::min(mana, maxMana);
    }

    void setMana(int newMana) {
        mana = std::min(newMana, maxMana);
    }
    void playUnitCard(std::unique_ptr<Card> card);

    void playUnitCard(std::unique_ptr<UnitCard> unit) {
        playUnitCard(std::unique_ptr<Card>(unit.release()));
    }
    std::vector<std::unique_ptr<UnitCard>>& getBattlefield() { return battlefield; }
    const std::vector<std::unique_ptr<UnitCard>>& getBattlefield() const { return battlefield; }
    const Hand& getHand() const { return hand; }
    Hand& getHandRef() { return hand; }
    int getMaxMana() const { return maxMana; }
    int getMana() const { return mana; }
    virtual BattleSystem::BattleResult attackWithUnit(size_t attackerIndex, size_t targetIndex);
    void addToBattlefield(std::unique_ptr<UnitCard> unit);
    void playCard(size_t index);
    void performAttack(size_t attackerIndex, size_t targetIndex);
    bool canAffordAttack() const;
    virtual void startTurn();
    bool isDefeated() const { return health <= 0; }
    void setInitialMana(int mana);
    void spendMana(int amount) { mana -= amount; }
    void incrementMana(int amount) { mana += amount; }
    bool canAttack() const;
    GameState* getGameState() const { return gameState; }
    virtual void takeTurn() = 0;
    virtual bool hasPlayableCards() const = 0;
    bool drawCard();
    void endTurn();
    void takeDamage(int amount);
    int heal(int amount);
    int getMaxHealth() const { return maxHealth; }
    void cleanBattlefield();
    void removeDeadUnits();
    int getAttackCost(const UnitCard& unit) const;
    void displayHealthStatus() const;
    virtual void attack(int attackerIndex, int targetIndex) {}
    const std::string& getName() const { return name; }
    int getHealth() const { return health; }
    Deck& getDeck() { return deck; }
    Player* getOpponent() const { return opponent; }
    void setOpponent(Player* newOpponent) { opponent = newOpponent; }
    std::vector<UnitCard*> getAttackableUnits() const;
    virtual int selectAttacker(const std::vector<UnitCard*>& units) const;
    virtual std::pair<Card*, Player*> selectAttackTarget(Player* opponent) const;
    int getValidatedChoice(int min, int max, const std::string& prompt) const;
};

#endif