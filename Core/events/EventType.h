/*
Sviridenko Elena st130482@student.spbu.ru
The system under development is a C++ console card game inspired by Hearthstone. 
*/
/**
 * @enum EventType
 * @brief Enumeration of all possible event types.
 */
#ifndef EVENTTYPE_H
#define EVENTTYPE_H

#include <string>

enum class EventType {
  CARD = 0,
  USER_ACTION = 1,
  AI_ACTION = 2,
  SYSTEM = 3,
  FIGHT = 4,
  HEALING = 5,
  DAMAGE = 6,
};
/**
 * @brief Converts EventType to a string.
 * @param type EventType.
 * @return String representation of type.
 */
inline std::string eventTypeToString(EventType type) {
  switch (type) {
    case EventType::CARD: return "Card";
    case EventType::USER_ACTION: return "User action";
    case EventType::AI_ACTION: return "AI action";
    case EventType::SYSTEM: return "System";
    case EventType::FIGHT: return "Fight";
    case EventType::HEALING: return "Healing";
    case EventType::DAMAGE: return "Damage";
    default: return "Unknown type";
  }
}

#endif
