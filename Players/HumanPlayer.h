/*
Sviridenko Elena st130482@student.spbu.ru
The system under development is a C++ console card game inspired by Hearthstone. 
*/
/**
 * @file HumanPlayer.h
 * @brief A class for controlling a person's moves via UI.
 */
#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include "Player.h"
#include "../Core/GameEngine.h"
#include <vector>
#include <utility>
#include <string>
/**
 * @class HumanPlayer
 * @brief Realizes interaction with the player through the menu.
 */
class HumanPlayer : public Player {
private:
    BattleSystem& getBattleSystem();

public:
    HumanPlayer(const std::string& name, int health, int mana, GameState* gameState = nullptr);
    void startTurn() override;
    void takeTurn() override;
    void handlePlayCard();
    void handleAttack();
    bool hasPlayableCards() const;
    std::vector<size_t> getPlayableCardIndices() const;
};

#endif 
