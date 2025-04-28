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
    void setMaxMana(int newMaxMana);
    void setMana(int newMana);
    void playUnitCard(std::unique_ptr<Card> card);

    std::vector<std::unique_ptr<UnitCard>>& getBattlefield();
    const std::vector<std::unique_ptr<UnitCard>>& getBattlefield() const;
    const Hand& getHand() const;
    Hand& getHandRef();
    int getMaxMana() const;
    int getMana() const;
    virtual BattleSystem::BattleResult attackWithUnit(size_t attackerIndex, size_t targetIndex);
    void playCard(size_t index);
    bool canAffordAttack() const;
    virtual void startTurn();
    bool isDefeated() const;
    void setInitialMana(int mana);
    bool canAttack() const;
    GameState* getGameState() const;
    virtual void takeTurn() = 0;
    virtual bool hasPlayableCards() const = 0;
    bool drawCard();
    void endTurn();
    void takeDamage(int amount);
    int heal(int amount);
    int getMaxHealth() const;
    void cleanBattlefield();
    void displayHealthStatus() const;
    virtual void attack(int attackerIndex, int targetIndex) {}
    const std::string& getName() const;
    int getHealth() const;
    Deck& getDeck();
    Player* getOpponent() const;
    void setOpponent(Player* newOpponent);
};

#endif