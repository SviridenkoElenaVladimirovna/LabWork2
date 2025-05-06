#include "SpellCard.h"
#include "../Players/Player.h"
#include <iostream>

SpellCard::SpellCard(const std::string& name, int cost, SpellEffect effect, int power)
        : Card(name, cost, CardType::SPELL), effect(effect), power(power) {}

void SpellCard::play(Player* owner, Player* opponent) {
    if (!owner || !opponent) {
        std::cerr << "Error: Null player pointer in spell cast\n";
        return;
    }

    if (owner->getMana() < cost) {
        std::cout << "Not enough mana to cast " << name << " (needs " << cost << ")\n";
        return;
    }

    owner->setMana(owner->getMana() - cost);

    std::cout << "A spell is used: " << name << " - ";
    try {
    switch (effect) {
        case SpellEffect::DAMAGE:
            std::cout << "inflicts " << power << " damage!\n";
            opponent->takeDamage(power);
            break;

        case SpellEffect::HEAL: {
            int healed = owner->heal(power);
                std::cout << "recovers " << healed << " health.\n";
            break;
        }
        case SpellEffect::DRAW: {
            int drawn = 0;
            for (int i = 0; i < power; ++i) {
                if (owner->drawCard()) {
                    drawn++;
                } else {
                    break;
                }
            }
            std::cout << "Drew " << drawn << " cards.\n";
            break;
        }

        case SpellEffect::BUFF:
            std::cout << "amplifies the creatures on " << power << " to attack.\n";
            for (auto& unit : owner->getBattlefield()) {
                unit->setAttack(unit->getAttack() + power);
            }
            break;
    }
    } catch (const std::exception& e) {
        std::cerr << "Spell error: " << e.what() << "\n";
    }
}

std::string SpellCard::getDescription() const {
    switch (effect) {
        case SpellEffect::DAMAGE: return name + " [Damage: " + std::to_string(power) + "]";
        case SpellEffect::HEAL: return name + " [Heal: " + std::to_string(power) + "]";
        case SpellEffect::DRAW: return name + " [Get the card: " + std::to_string(power) + "]";
        case SpellEffect::BUFF: return name + " [Amplification: +" + std::to_string(power) + " to attack]";
        default: return name + " [Unknown effect]";
    }
}

void SpellCard::displayInfo() const {
    std::cout << getDescription() << " (" << getCost() << " manes)";
}