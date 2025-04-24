#ifndef CARD_H
#define CARD_H

#include <string>
#include <memory>
#include <iostream>
#include <limits>

class Player;

enum class CardType {
    UNIT,
    SPELL
};

class Card {
protected:
    std::string name;
    bool played = false;
    int cost;
    CardType type;

public:
    bool isPlayed() const { return played; }
    void setPlayed(bool val) { played = val; }
    Card(const std::string& name, int cost, CardType type);
    virtual ~Card() = default;

    virtual void play(Player* owner, Player* opponent) = 0;
    virtual std::string getDescription() const = 0;
    virtual void displayInfo() const {
        std::cout << name << " (" << cost << " mana)";
    }

    std::string getName() const { return name; }
    int getCost() const { return cost; }
    CardType getType() const { return type; }

    virtual std::unique_ptr<Card> clone() const = 0;

};

#endif 
