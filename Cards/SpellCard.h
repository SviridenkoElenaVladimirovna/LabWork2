#ifndef SPELL_CARD_H
#define SPELL_CARD_H

#include "Card.h"

enum class SpellEffect {
    DAMAGE,
    HEAL,
    DRAW,
    BUFF
};

class SpellCard : public Card {
private:
    SpellEffect effect;
    int power;

public:
    SpellCard(const std::string& name, int cost, SpellEffect effect, int power);
    std::unique_ptr<Card> clone() const override;
    SpellEffect getEffect() const;
    int getPower() const;
    void play(Player* owner, Player* opponent) override;
    std::string getDescription() const override;
    void displayInfo() const override;
};

#endif