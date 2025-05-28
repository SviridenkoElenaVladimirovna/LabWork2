/*
Sviridenko Elena st130482@student.spbu.ru
The system under development is a C++ console card game inspired by Hearthstone. 
*/
#include "UnitCard.h"
#include "../Players/Player.h"

UnitCard::UnitCard(const std::string& name, int cost, int attack, int health, bool hasAbility)
        : Card(name, cost, CardType::UNIT), attack(attack), health(health),
          exhausted(true), hasAbility(hasAbility), summonedThisTurn(true) {}

bool UnitCard::isExhausted() const { return exhausted; }

bool UnitCard::canAttackNow() const {
    return !exhausted && health > 0 && !summonedThisTurn;
}

void UnitCard::setExhausted(bool value) { exhausted = value; }

void UnitCard::play(Player* owner, Player* opponent) {
    if (!owner) return;
    auto cardCopy = this->clone();
    owner->playUnitCard(std::move(cardCopy));
}
std::string UnitCard::getDescription() const {
    return name + " (" + std::to_string(attack) + "/" + std::to_string(health) + ")";
}

void UnitCard::displayInfo() const {
    Card::displayInfo();
    std::cout << " (" << attack << "/" << health << ")";
    if (hasAbility) {
        std::cout << " [Ability]";
    }
}
std::unique_ptr<Card> UnitCard::clone() const {
    return std::make_unique<UnitCard>(*this);
}

bool UnitCard::isDead() const { return health <= 0; }

void UnitCard::attackTarget(UnitCard* target) {
    if (canAttackNow()) {
        target->takeDamage(attack);
        exhausted = true;
    }
}

void UnitCard::attackPlayer(Player* player) {
    if (canAttackNow()) {
        player->takeDamage(attack);
        exhausted = true;
    }
}

void UnitCard::takeDamage(int damage) {
    health -= damage;
    if (health < 0) health = 0;
}
void UnitCard::onPlay() {
    exhausted = true;
    summonedThisTurn = true;
}

void UnitCard::refresh() {
    exhausted = false;
    summonedThisTurn = false;
}

int UnitCard::getAttack() const { return attack; }
int UnitCard::getHealth() const { return health; }
void UnitCard::setAttack(int value) { attack = value; }
void UnitCard::setHealth(int value) { health = std::max(0, value); }
