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

private:
    GameState* currentBattleState;
};

#endif