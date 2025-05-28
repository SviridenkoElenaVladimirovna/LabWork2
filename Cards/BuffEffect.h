/*
Sviridenko Elena st130482@student.spbu.ru
The system under development is a C++ console card game inspired by Hearthstone. 
*/
/**
 * @file BuffEffect.h
 * @brief The effect of increasing a unit's attack and health
 */
#ifndef BUFFEFFECT_H
#define BUFFEFFECT_H

#include "Effect.h"
/**
 * @class BuffEffect
 * @brief Increases attack stats
 */
class BuffEffect : public Effect {
private:
    int buffAmount;

public:
    BuffEffect(int buffAmount);
    std::string getDescription() const;
    virtual void applyEffect(Card* target);
};

#endif
