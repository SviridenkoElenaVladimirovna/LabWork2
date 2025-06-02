/*
Sviridenko Elena st130482@student.spbu.ru
The system under development is a C++ console card game inspired by Hearthstone. 
*/
/**
 * @file MediumAI.h
 * @brief AI attacking the weakest enemy.
 */
#include "AI.h"
#include <memory>
/**
 * @class MediumAI
 * @brief Represents an AI that attacks the weakest enemy and plays cards more sensibly.
 */
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
