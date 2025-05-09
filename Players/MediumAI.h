#include "AI.h"
#include <memory>

class MediumAI : public AI {
private:
    UnitCard* findWeakestEnemy() const;
    int evaluateCard(const Card* card) const;
    bool canPlayAnyCard() const;
protected:
    void playSelectedCard(int cardIndex);
public:
    MediumAI(const std::string& name, int health, int mana, GameState* gameState, UIManager* uiManager);
    void takeTurn() override;
    bool shouldPlayCard(const Card* card) const override;
    int chooseAttackTarget(int attackingUnitIndex) const override;
    int chooseCardToPlay() const override;
    void playCards();
    void performAttacks();
    void cleanup();

    int chooseUnitToAttackWith() const override;

};
