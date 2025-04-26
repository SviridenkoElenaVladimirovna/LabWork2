
#include "BattleSystem.h"
#include "../Players/Player.h"
#include "../Core/GameState.h"
#include "../UI/UIManager.h"

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
    target.takeDamage(attacker.getAttack());
    result.damageDealt = initialDefenderHealth - target.getHealth();
    result.defenderDestroyed = target.isDead();

    if (!target.isDead()) {
        int initialAttackerHealth = attacker.getHealth();
        attacker.takeDamage(target.getAttack());
        result.attackerDestroyed = attacker.isDead();
    } else {
        result.attackerDestroyed = false;
    }

    attacker.setExhausted(true);

    return result;
}

BattleSystem::BattleResult BattleSystem::attackHero(UnitCard& attacker, Player& targetPlayer) {
    BattleResult result;
    result.attackerName = attacker.getName();
    result.defenderName = targetPlayer.getName();

    if (!attacker.canAttackNow()) {
        result.damageDealt = 0;
        return result;
    }

    int initialHealth = targetPlayer.getHealth();
    targetPlayer.takeDamage(attacker.getAttack());
    result.damageDealt = initialHealth - targetPlayer.getHealth();
    result.defenderDestroyed = false;
    result.attackerDestroyed = false;

    attacker.setExhausted(true);

    return result;
}