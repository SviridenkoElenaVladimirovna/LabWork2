/*
Sviridenko Elena st130482@student.spbu.ru
The system under development is a C++ console card game inspired by Hearthstone. 
*/
/**
 * @file HealEffect.h
 * @brief HealEffect
 */
#ifndef HEALEFFECT_H
#define HEALEFFECT_H

#include "Effect.h"
/**
 * @class HealEffect
 * @brief Restores health
 */
class HealEffect : public Effect {
private:
    int healAmount;

public:
    explicit HealEffect(int healAmount);
    void apply(Card* cardTarget, Player* playerTarget) override;
    void applyEffect(Card* target) override;
    std::string getDescription() const override;
};

#endif
