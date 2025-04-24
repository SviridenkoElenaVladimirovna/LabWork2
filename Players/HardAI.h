
#pragma once
#include "AI.h"
#include <vector>

class HardAI : public AI {
public:
    HardAI(const std::string& name, int health, int mana, GameState* gameState);

    void takeTurn() override;

protected:
    int chooseAttackTarget(int attackingUnitIndex) const override;
    bool shouldPlayCard(const Card* card) const override;
    int chooseCardToPlay() const override;

    int chooseUnitToAttackWith() const override;


private:
    int evaluateCard(const Card* card) const;
    int evaluateAttack(UnitCard* attacker, UnitCard* target) const;
    UnitCard* findBestAttackTarget(int attackerIndex) const;
    int evaluateBoardState() const;
    int evaluateCardPlay(const Card* card) const;
};
