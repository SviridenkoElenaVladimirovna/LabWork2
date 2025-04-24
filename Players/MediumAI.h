
#pragma once
#include "AI.h"
#include <memory>

class MediumAI : public AI {
public:
    MediumAI(const std::string& name, int health, int mana, GameState* gameState);

    void takeTurn() override;

protected:
    int chooseAttackTarget(int attackingUnitIndex) const override;
    bool shouldPlayCard(const Card* card) const override;
    int chooseCardToPlay() const override;

    int chooseUnitToAttackWith() const override;


private:
    UnitCard* findOptimalAttackTarget() const;
};
