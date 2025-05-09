#ifndef HEALEFFECT_H
#define HEALEFFECT_H

#include "Effect.h"

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