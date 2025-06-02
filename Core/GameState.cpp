/*
Sviridenko Elena st130482@student.spbu.ru
The system under development is a C++ console card game inspired by Hearthstone. 
*/
#include "GameState.h"
#include "../Data/PlayerState.h"
#include "events/GameEvent.h"

GameState::GameState() : turn(0), history(std::make_unique<GameHistory>()) {}

void GameState::updateState() {
}

int GameState::getTurn() const {
    return turn;
}

GameHistory& GameState::getGameHistory() {
    return *history;
}

const GameHistory& GameState::getGameHistory() const {
    return *history;
}
