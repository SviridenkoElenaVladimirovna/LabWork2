/*
Sviridenko Elena st130482@student.spbu.ru
The system under development is a C++ console card game inspired by Hearthstone. 
*/
#include "BattleSystem.h"
#include "../Players/Player.h"

BattleSystem::BattleSystem(GameState* state) : currentBattleState(state) {}

BattleSystem::~BattleSystem() {
}
BattleSystem::BattleResult BattleSystem::attack(UnitCard& attacker, UnitCard& target) {
    BattleResult result;
    result.attackerName = attacker.getName();
    result.defenderName = target.getName();

    if (!attacker.canAttackNow()) {
        result.damageDealt = 0;
        result.defenderDestroyed = false;
        result.attackerDestroyed = false;
        return result;
    }

    int initialDefenderHealth = target.getHealth();
    int initialAttackerHealth = attacker.getHealth();

    target.takeDamage(attacker.getAttack());
    attacker.takeDamage(target.getAttack());

    result.damageDealt = initialDefenderHealth - target.getHealth();
    result.defenderDestroyed = target.isDead();
    result.attackerDestroyed = attacker.isDead();

    attacker.setExhausted(true);

    return result;
}

BattleSystem::BattleResult BattleSystem::attackHero(UnitCard& attacker, Player& targetPlayer) {
    BattleResult result;
    result.attackerName = attacker.getName();
    result.defenderName = targetPlayer.getName();
    result.attackedHero = true;

    if (!attacker.canAttackNow()) {
        result.damageDealt = 0;
        return result;
    }

    int initialHealth = targetPlayer.getHealth();
    targetPlayer.takeDamage(attacker.getAttack());
    result.damageDealt = initialHealth - targetPlayer.getHealth();
    result.defenderDestroyed = targetPlayer.isDefeated();
    result.attackerDestroyed = false;

    attacker.setExhausted(true);

    return result;
}
