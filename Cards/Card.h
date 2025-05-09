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
    Card(const std::string& name, int cost, CardType type);
    virtual ~Card() = default;

    bool isPlayed() const;
    void setPlayed(bool val);
    std::string getName() const;
    int getCost() const;
    CardType getType() const;

    virtual void play(Player* owner, Player* opponent) = 0;
    virtual std::string getDescription() const = 0;
    virtual void displayInfo() const;
    virtual std::unique_ptr<Card> clone() const = 0;

};

#endif 
