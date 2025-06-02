/*
Sviridenko Elena st130482@student.spbu.ru
The system under development is a C++ console card game inspired by Hearthstone. 
*/
/**
 * @file HardAI.h
 * @brief AI with advanced strategy (map and goal evaluation).
 */
#include "AI.h"
#include <vector>
/**
 * @class HardAI
 * @brief Represents an AI player with advanced strategy using evaluation heuristics.
 */
class HardAI : public AI {

protected:
    int chooseAttackTarget(int attackingUnitIndex) const override;
    bool shouldPlayCard(const Card* card) const override;
    int chooseCardToPlay() const override;
    int chooseUnitToAttackWith() const override;

public:
    HardAI(const std::string& name, int health, int mana, GameState* gameState, UIManager* uiManager);
    void takeTurn() override;
    bool shouldPlayAggressively() const;
    void playCardsWithStrategy(bool aggressiveStrategy);
    int findBestCardToPlay(bool aggressiveStrategy) const;
    int evaluateCardForStrategy(const Card* card, bool aggressiveStrategy) const;
    int countPotentialKills(int damage) const;
    int findBestDamageTarget(int damage) const;

    std::vector<std::pair<size_t, int>> getAttackOrder() const;
    const std::vector<std::unique_ptr<UnitCard>>& getBattlefieldRef() const;

    int evaluateAttack(UnitCard* attacker, UnitCard* target) const;
    int evaluateCard(const Card* card) const;
    void executeOptimizedAttacks(bool aggressiveStrategy);
    int chooseOptimalAttackTarget(int unitIndex, bool aggressiveStrategy) const;
};
