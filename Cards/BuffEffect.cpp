/*
Sviridenko Elena st130482@student.spbu.ru
The system under development is a C++ console card game inspired by Hearthstone. 
*/
#include "BuffEffect.h"
#include "UnitCard.h"

BuffEffect::BuffEffect(int buffAmount)
        : Effect("Buff"), buffAmount(buffAmount) {}

void BuffEffect::applyEffect(Card* target) {
    if (auto unitCard = dynamic_cast<UnitCard*>(target)) {
        unitCard->setHealth(unitCard->getHealth() + buffAmount);
        unitCard->setAttack(unitCard->getAttack() + buffAmount);
    }
}


std::string BuffEffect::getDescription() const {
    return "Increases attack and health by " + std::to_string(buffAmount);
}
