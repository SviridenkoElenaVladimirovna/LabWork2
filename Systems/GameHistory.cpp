#include "GameHistory.h"

GameHistory::~GameHistory() {
    events.clear();
}

void GameHistory::recordEvent(std::unique_ptr<GameEvent> event) {
    events.push_back(std::move(event));
}

void GameHistory::clearEvents() {
    events.clear();
}

const std::vector<std::unique_ptr<GameEvent>>& GameHistory::getHistory() const {
    return events;
}
