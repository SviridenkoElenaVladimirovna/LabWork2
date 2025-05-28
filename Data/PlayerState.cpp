/*
Sviridenko Elena st130482@student.spbu.ru
The system under development is a C++ console card game inspired by Hearthstone. 
*/
#include "PlayerState.h"

PlayerState::PlayerState(Player* player)
        : player(player), health(player->getHealth()), mana(player->getMana()) {}
PlayerState::~PlayerState() = default;
void PlayerState::updateState() {
    health = player->getHealth();
    mana = player->getMana();
}

Player* PlayerState::getPlayer() const {
    return player;
}

int PlayerState::getHealth() const {
    return health;
}

int PlayerState::getMana() const {
    return mana;
}
