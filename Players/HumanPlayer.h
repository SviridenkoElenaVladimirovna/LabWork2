#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include "Player.h"
#include "../Core/GameEngine.h"
#include <vector>
#include <utility>
#include <string>

class HumanPlayer : public Player {
public:
    HumanPlayer(const std::string& name, int health, int mana, GameState* gameState = nullptr);
    void startTurn() override;

    void takeTurn() override;

    UiActionsEnum getActionChoice(GameEngine& engine);
    void handlePlayCard();
    void handleAttack();
    bool canAttack() const;

    std::vector<std::pair<int, std::string>> getAvailableActions() const;
    void refreshUnits();

    bool hasPlayableCards() const;
    std::vector<size_t> getPlayableCardIndices() const;

private:
    BattleSystem& getBattleSystem() { return *battleSystem; }
    void displayCard(const Card* card) const;
    void displayUnit(const UnitCard* unit) const;
    void displayAttackOptions() const;
    int getValidatedChoice(int min, int max, const std::string& prompt) const;
};

#endif 
