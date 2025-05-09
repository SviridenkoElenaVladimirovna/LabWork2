#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include "Player.h"
#include "../Core/GameEngine.h"
#include <vector>
#include <utility>
#include <string>

class HumanPlayer : public Player {
private:
    BattleSystem& getBattleSystem();

public:
    HumanPlayer(const std::string& name, int health, int mana, GameState* gameState = nullptr);
    void startTurn() override;
    void takeTurn() override;
    void handlePlayCard();
    void handleAttack();
    bool hasPlayableCards() const;
    std::vector<size_t> getPlayableCardIndices() const;
};

#endif 
