/*
Sviridenko Elena st130482@student.spbu.ru
The system under development is a C++ console card game inspired by Hearthstone. 
*/
/**
 * @file PlayerState.h
 * @brief A class to store the snapshot state of a player.
 * @details Used to log or rollback states.
 */
#ifndef PLAYERSTATE_H
#define PLAYERSTATE_H

#include "../Players/Player.h"
/**
 * @class PlayerState
 * @brief Captures the current state of a player, including health and mana.
 */
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
