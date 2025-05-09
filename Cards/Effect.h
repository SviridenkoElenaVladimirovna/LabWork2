#ifndef EFFECT_H
#define EFFECT_H

#include <string>

class Card;
class Player;

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