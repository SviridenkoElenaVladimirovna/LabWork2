/*
Sviridenko Elena st130482@student.spbu.ru
The system under development is a C++ console card game inspired by Hearthstone. 
*/
#include "HealEffect.h"
#include "UnitCard.h"
#include "../Players/Player.h"

HealEffect::HealEffect(int healAmount)
        : Effect("Heal"), healAmount(healAmount) {}

void HealEffect::apply(Card* cardTarget, Player* playerTarget) {
    if (auto unit = dynamic_cast<UnitCard*>(cardTarget)) {
        unit->setHealth(unit->getHealth() + healAmount);
    } else if (playerTarget) {
        playerTarget->heal(healAmount);
    }
}

void HealEffect::applyEffect(Card* target) {
    if (auto unit = dynamic_cast<UnitCard*>(target)) {
        unit->setHealth(unit->getHealth() + healAmount);
    }
}

std::string HealEffect::getDescription() const {
    return "Restores " + std::to_string(healAmount) + " health";
}
