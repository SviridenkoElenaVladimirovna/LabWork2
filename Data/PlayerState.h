#ifndef PLAYERSTATE_H
#define PLAYERSTATE_H

#include "../Players/Player.h"

class PlayerState {
private:
    Player* player;
    int health;
    int mana;

public:
    explicit PlayerState(Player* player);
    ~PlayerState();

    void updateState();

    Player* getPlayer() const;
    int getHealth() const;
    int getMana() const;
};

#endif