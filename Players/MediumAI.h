#include "AI.h"
#include <memory>

class MediumAI : public AI {
private:
    UnitCard* findWeakestEnemy() const;
    int evaluateCard(const Card* card) const;

protected:
    int chooseAttackTarget(int attackingUnitIndex) const override;
    bool shouldPlayCard(const Card* card) const override;
    int chooseCardToPlay() const override;
    int chooseUnitToAttackWith() const override;
    bool hasPlayableCards() const;
    void playSelectedCard(int cardIndex);
public:
    MediumAI(const std::string& name, int health, int mana, GameState* gameState, UIManager* uiManager);
    void takeTurn() override;

};
