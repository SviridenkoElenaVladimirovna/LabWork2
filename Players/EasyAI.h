#ifndef EASYAI_H
#define EASYAI_H

#include "AI.h"

class EasyAI : public AI {
public:
    EasyAI(const std::string& name, int health, int mana, GameState* gameState, UIManager* uiManager);
    virtual ~EasyAI() = default;

    void takeTurn() override;
    bool hasPlayableCards() const override;

protected:
    bool shouldPlayCard(const Card* card) const override;
    int chooseCardToPlay() const override;
    int chooseUnitToAttackWith() const override;
    int chooseAttackTarget(int attackingUnitIndex) const override;

private:
    UnitCard* findWeakestEnemy() const;
};

#endif