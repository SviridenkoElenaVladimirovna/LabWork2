#ifndef GAMEEVENT_H
#define GAMEEVENT_H

#include <string>
#include <ctime>
#include <memory>

#include "EventType.h"

class GameEvent {
private:
    std::string eventType;
    std::string eventDetails;
    time_t timestamp;
    std::string details;

public:
    GameEvent(const std::string& type, const std::string& details);

    std::string getType() const;
    std::string getTimestamp() const;
    std::string toString() const;

    static std::unique_ptr<GameEvent> of(const EventType& type, const std::string& details);
};

#endif