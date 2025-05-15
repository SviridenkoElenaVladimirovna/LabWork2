/**
 * @file BattleSystem.h
 * @brief Handles combat interactions between units and players.
 */
#ifndef BATTLESYSTEM_H
#define BATTLESYSTEM_H

#include "../Cards/UnitCard.h"
#include <string>

class GameState;
class Player;
/**
 * @class BattleSystem
 * @brief Controls the mechanics of attacks and damage calculation.
 */
class BattleSystem {
private:
    GameState* currentBattleState;

public:
    /**
     * @struct BattleResult
     * @brief BattleResult.
     */
    struct BattleResult {
        std::string attackerName;
        std::string defenderName;
        int damageDealt = 0;
        int counterDamage = 0;
        bool defenderDestroyed = false;
        bool attackerDestroyed = false;
        bool attackedHero = false;
    };

    explicit BattleSystem(GameState* state);
    ~BattleSystem();

    BattleResult attack(UnitCard& attacker, UnitCard& target);
    BattleResult attackHero(UnitCard& attacker, Player& targetPlayer);
};

#endif