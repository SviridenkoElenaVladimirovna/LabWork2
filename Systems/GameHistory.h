#ifndef GAMEHISTORY_H
#define GAMEHISTORY_H

#include <memory>
#include <vector>
#include "../Core/events/GameEvent.h"


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