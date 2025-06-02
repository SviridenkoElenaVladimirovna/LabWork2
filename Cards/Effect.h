/*
Sviridenko Elena st130482@student.spbu.ru
The system under development is a C++ console card game inspired by Hearthstone. 
*/
/**
 * @file Effect.h
 * @brief Base class for card effects (buffs, damage, etc)
 */
#ifndef EFFECT_H
#define EFFECT_H

#include <string>

class Card;
class Player;
/**
 * @class Effect
 * @brief An abstract class representing a map effect.
 */
class Effect {
protected:
    std::string type;
public:
    explicit Effect(const std::string& type);
    virtual ~Effect() = default;

    virtual void apply(Card* cardTarget, Player* playerTarget) = 0;
    virtual void applyEffect(Card* target) = 0;
    virtual std::string getDescription() const = 0;

};

#endif
