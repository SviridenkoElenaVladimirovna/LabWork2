#ifndef EFFECT_H
#define EFFECT_H

#include <string>

class Card;
class Player;

class Effect {
public:
    explicit Effect(const std::string& type);
    virtual ~Effect() = default;

    virtual void apply(Card* cardTarget, Player* playerTarget) = 0;
    virtual void applyEffect(Card* target) = 0;
    virtual std::string getDescription() const = 0;

    std::string getType() const { return type; }

protected:
    std::string type;
};

#endif