#ifndef EASYAI_H
#define EASYAI_H

#include "AI.h"

class EasyAI : public AI {
private:
    UnitCard* findWeakestEnemy() const;

protected:
    void playSelectedCard(int cardIndex);
    bool shouldPlayCard(const Card* card) const override;
    int chooseCardToPlay() const override;
    int chooseUnitToAttackWith() const override;
    int chooseAttackTarget(int attackingUnitIndex) const override;

public:
    EasyAI(const std::string& name, int health, int mana, GameState* gameState, UIManager* uiManager);
    virtual ~EasyAI() = default;

    void takeTurn() override;
    bool hasPlayableCards() const override;
    };

#endif