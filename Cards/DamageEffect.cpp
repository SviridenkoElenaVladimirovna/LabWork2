/*
Sviridenko Elena st130482@student.spbu.ru
The system under development is a C++ console card game inspired by Hearthstone. 
*/
#include "DamageEffect.h"
#include "UnitCard.h"
#include "../Players/Player.h"

DamageEffect::DamageEffect(int damage, bool logs)
        : Effect("Damage"), damage(damage), showLogs(logs) {
    if (damage < 0) this->damage = 0;
}

void DamageEffect::apply(Card* cardTarget, Player* playerTarget) {
    if (cardTarget) {
        applyEffect(cardTarget);
    } else if (playerTarget) {
        playerTarget->takeDamage(damage);
    }
}

void DamageEffect::applyEffect(Card* target) {
    if (auto* unit = dynamic_cast<UnitCard*>(target)) {
        unit->takeDamage(damage);
    }
}

std::string DamageEffect::getDescription() const {
    return "Inflicts " + std::to_string(damage) + " Damage";
}
