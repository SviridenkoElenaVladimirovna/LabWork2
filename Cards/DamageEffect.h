/*
Sviridenko Elena st130482@student.spbu.ru
The system under development is a C++ console card game inspired by Hearthstone. 
*/
/**
 * @file DamageEffect.h
 * @brief Damage Effect
 */
#include "Effect.h"
#include <string>
/**
 * @class DamageEffect
 * @brief Deals damage to the target (unit or player).
 */
class DamageEffect : public Effect {
private:
    int damage;
    bool showLogs;

public:
    explicit DamageEffect(int damage, bool logs = true);
    void apply(Card* cardTarget, Player* playerTarget) override;
    void applyEffect(Card* target) override;
    std::string getDescription() const override;
};
