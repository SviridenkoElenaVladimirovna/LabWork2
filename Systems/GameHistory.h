/*
Sviridenko Elena st130482@student.spbu.ru
The system under development is a C++ console card game inspired by Hearthstone. 
*/
/**
 * @file GameHistory.h
 * @brief Stores the history of game events.
 */
#ifndef GAMEHISTORY_H
#define GAMEHISTORY_H

#include <memory>
#include <vector>
#include "../Core/events/GameEvent.h"

/**
 * @class GameHistory
 * @brief Records and provides access to game events.
 */
class GameHistory {
private:
    std::vector<std::unique_ptr<GameEvent>> events;

public:
    ~GameHistory();

    void recordEvent(std::unique_ptr<GameEvent> event);
    void clearEvents();
    const std::vector<std::unique_ptr<GameEvent>>& getHistory() const;
};

#endif
