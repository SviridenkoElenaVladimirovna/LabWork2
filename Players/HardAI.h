#include "AI.h"
#include <vector>

class HardAI : public AI {

private:
    int evaluateCard(const Card* card) const;
    int evaluateAttack(UnitCard* attacker, UnitCard* target) const;

protected:
    int chooseAttackTarget(int attackingUnitIndex) const override;
    bool shouldPlayCard(const Card* card) const override;
    int chooseCardToPlay() const override;
    int chooseUnitToAttackWith() const override;

public:
    HardAI(const std::string& name, int health, int mana, GameState* gameState, UIManager* uiManager);
    void takeTurn() override;

};
