#ifndef BATTLESYSTEM_H
#define BATTLESYSTEM_H

#include "../Cards/UnitCard.h"
#include <string>

class GameState;
class Player;

class BattleSystem {
public:
    struct BattleResult {
        std::string attackerName;
        std::string defenderName;
        int damageDealt;
        bool defenderDestroyed;
        bool attackerDestroyed;
    };

    explicit BattleSystem(GameState* state);
    ~BattleSystem();

    BattleResult attack(UnitCard& attacker, UnitCard& target);
    BattleResult attackHero(UnitCard& attacker, Player& targetPlayer);

private:
    GameState* currentBattleState;
};

#endif