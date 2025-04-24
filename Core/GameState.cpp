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