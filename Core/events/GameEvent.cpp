/*
Sviridenko Elena st130482@student.spbu.ru
The system under development is a C++ console card game inspired by Hearthstone. 
*/
#include "GameEvent.h"
#include <ctime>
#include <sstream>

GameEvent::GameEvent(const std::string& type, const std::string& details)
        : eventType(type), eventDetails(details), timestamp(std::time(nullptr)) {}

std::string GameEvent::getType() const {
    return eventType;
}

std::string GameEvent::getTimestamp() const {
    std::tm* ptm = std::localtime(&timestamp);
    char buffer[32];
    std::strftime(buffer, 32, "%Y-%m-%d %H:%M:%S", ptm);
    return buffer;
}

std::string GameEvent::toString() const {
    return "[" + getTimestamp() + "] " + "[" + getType() + "]: " + eventDetails;
}

std::unique_ptr<GameEvent> GameEvent::of(const EventType &type, const std::string &details) {
    return std::make_unique<GameEvent>(eventTypeToString(type), details);
}
