/*
Sviridenko Elena st130482@student.spbu.ru
The system under development is a C++ console card game inspired by Hearthstone. 
*/
/**
 * @file EasyAI.h
 * @brief AI with random actions.
 */
#ifndef EASYAI_H
#define EASYAI_H

#include "AI.h"
/**
 * @class EasyAI
 * @brief Represents a simple AI that acts randomly.
 */
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
    void performAttacks();
    int chooseAttackTarget(size_t attackingUnitIndex) const;
    void takeTurn() override;
    bool hasPlayableCards() const override;
    };

#endif
