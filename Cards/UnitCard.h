#ifndef UNIT_CARD_H
#define UNIT_CARD_H

#include "Card.h"

class UnitCard : public Card {
private:
    int attack;
    int health;
    bool exhausted;
    bool hasAbility;
    bool summonedThisTurn;

public:
    UnitCard(const std::string& name, int cost, int attack, int health, bool hasAbility = false);
    ~UnitCard() override = default;

    bool isExhausted() const;
    bool canAttackNow() const;
    void setExhausted(bool value);

    void play(Player* owner, Player* opponent) override;
    std::string getDescription() const override;
    void displayInfo() const override;

    std::unique_ptr<Card> clone() const override;

    bool isDead() const;
    void attackTarget(UnitCard* target);
    void attackPlayer(Player* player);
    void takeDamage(int damage);
    void onPlay();
    void refresh();

    int getAttack() const;
    int getHealth() const;
    void setAttack(int value);
    void setHealth(int value);
};

#endif