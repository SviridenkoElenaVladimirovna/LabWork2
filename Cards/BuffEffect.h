#ifndef BUFFEFFECT_H
#define BUFFEFFECT_H

#include "Effect.h"

class BuffEffect : public Effect {
private:
    int buffAmount;

public:
    BuffEffect(int buffAmount);
    std::string getDescription() const;
    virtual void applyEffect(Card* target);
};

#endif