
#include "Deck.h"
#include "../Cards/UnitCard.h"
#include "../Cards/SpellCard.h"
#include <algorithm>
#include <random>

Deck::Deck() = default;

void Deck::addBasicUnits() {
    for (int i = 0; i < 2; i++) {
        cards.push_back(std::make_unique<UnitCard>("Infantryman", 1, 2, 2));
        cards.push_back(std::make_unique<UnitCard>("Scout", 1, 1, 3));
        cards.push_back(std::make_unique<UnitCard>("Wolf", 1, 3, 1));
        cards.push_back(std::make_unique<UnitCard>("Archer", 2, 3, 2));
        cards.push_back(std::make_unique<UnitCard>("Dwarf inventor", 2, 2, 3));
        cards.push_back(std::make_unique<UnitCard>("Werewolf", 2, 4, 1));
        cards.push_back(std::make_unique<UnitCard>("Knight", 3, 4, 3));
        cards.push_back(std::make_unique<UnitCard>("Shaman", 3, 3, 4));
        cards.push_back(std::make_unique<UnitCard>("Gryphon", 3, 5, 2));
        cards.push_back(std::make_unique<UnitCard>("Ogre", 4, 5, 4));
        cards.push_back(std::make_unique<UnitCard>("Elemental", 4, 4, 5));
        cards.push_back(std::make_unique<UnitCard>("Centaur", 4, 6, 3));
    }
}

void Deck::addSpells() {
    cards.push_back(std::make_unique<SpellCard>("Fireball", 3, SpellEffect::DAMAGE, 4));
    cards.push_back(std::make_unique<SpellCard>("Ice arrow", 2, SpellEffect::DAMAGE, 2));
    cards.push_back(std::make_unique<SpellCard>("Lightning", 1, SpellEffect::DAMAGE, 1));
    cards.push_back(std::make_unique<SpellCard>("Heal", 2, SpellEffect::HEAL, 3));
    cards.push_back(std::make_unique<SpellCard>("Sorcerer's intelligence", 3, SpellEffect::DRAW, 2));
    cards.push_back(std::make_unique<SpellCard>("Acceleration", 1, SpellEffect::DRAW, 1));
    cards.push_back(std::make_unique<SpellCard>("Battle cry", 2, SpellEffect::BUFF, 2));

}
void Deck::addSpecialUnits() {
    cards.push_back(std::make_unique<UnitCard>("Dragon", 5, 6, 6, true));
    cards.push_back(std::make_unique<UnitCard>("Phoenix", 6, 4, 7, true));
    cards.push_back(std::make_unique<UnitCard>("Golem", 7, 7, 7, true));
    cards.push_back(std::make_unique<UnitCard>("Archimage", 5, 3, 5, true));
    cards.push_back(std::make_unique<UnitCard>("Dark Lord", 8, 8, 8, true));
}

void Deck::initializeStandardDeck() {
    clear();
    addBasicUnits();
    addSpells();
    addSpecialUnits();
    shuffle();
}
void Deck::addCard(std::unique_ptr<Card> card) {
    cards.push_back(std::move(card));
}

std::unique_ptr<Card> Deck::drawCard() {
    if (cards.empty()) return nullptr;
    auto card = std::move(cards.back());
    cards.pop_back();
    return card;
}

void Deck::shuffle() {
    static std::random_device rd;
    static std::mt19937 g(rd());
    std::shuffle(cards.begin(), cards.end(), g);
}

bool Deck::isEmpty() const {
    return cards.empty();
}

size_t Deck::size() const {
    return cards.size();
}

void Deck::clear() {
    cards.clear();
}