#ifndef AI_H
#define AI_H

#include "Player.h"
#include "../UI/UIManager.h"
#include <vector>

class GameEngine;

class AI : public Player {
public:
    AI(const std::string& name, int health, int mana, GameState* gameState, UIManager* uiManager);
    virtual ~AI() = default;
    virtual void takeTurn() override;

    virtual bool hasPlayableCards() const;
    virtual int chooseCardToPlay() const;
    virtual int chooseUnitToAttackWith() const;
    virtual int chooseAttackTarget(int attackingUnitIndex) const;

protected:
    UIManager* ui;
    virtual bool shouldPlayCard(const Card* card) const { return true; }
    virtual UnitCard* findWeakestEnemy() const;
    virtual UnitCard* findStrongestEnemy() const;
    virtual UnitCard* findBestAttackTarget(UnitCard* attacker) const;

    void attackWithUnit(int unitIndex);
};

#endif