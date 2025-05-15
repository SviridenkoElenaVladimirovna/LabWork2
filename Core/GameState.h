/**
 * @file GameState.h
 * @brief Base class for storing game state.
 */
#ifndef GAMESTATE_H
#define GAMESTATE_H

#include <vector>
#include <memory>
#include "../Systems/GameHistory.h"

class PlayerState;
class Player;
/**
 * @class GameState
 * @brief Abstract base class representing the state of the game.
 * abstract methods for retrieving player-related state.

 */
class GameState {
protected:
    int turn;
    std::unique_ptr<GameHistory> history;

public:
    GameState();
    virtual ~GameState() = default;
    virtual Player* getCurrentPlayer() const = 0;
    virtual Player* getOpponent() const = 0;
    virtual void displayGameState() const = 0;
    void updateState();
    int getTurn() const;
    void logEvent(const std::string& type, const std::string& details);
    GameHistory& getGameHistory();
    const GameHistory& getGameHistory() const;
};

#endif