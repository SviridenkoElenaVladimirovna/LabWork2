#ifndef UIMANAGER_H
#define UIMANAGER_H

#include "../Players/Player.h"
#include "../Cards/Card.h"
#include "../Cards/UnitCard.h"
#include "../Systems/BattleSystem.h"
#include "../UI/InputHandler.h"
#include "UiActionsEnum.h"
#include <vector>
#include <memory>
#include <string>

class GameEngine;

class UIManager {
private:
    InputHandler inputHandler;
    GameEngine* gameState;

    void updateGameState();

public:
    explicit UIManager(GameEngine* engine);
    std::vector<std::pair<UiActionsEnum, std::string>> buildPlayerActions(const Player* player);
    UiActionsEnum showActionMenu(const std::vector<std::pair<UiActionsEnum, std::string>>& options);
    void displayGameState(Player* currentPlayer, Player* opponent, int turnNumber);
    void displayMessage(const std::string& message);
    void displayLogMessage(const std::string& message);
    void displayHighlightedMessage(const std::string& message);

    void displayCards(const std::vector<std::unique_ptr<Card>>& cards);
    void displayBattlefield(const std::vector<std::unique_ptr<UnitCard>>& units);

    UiActionsEnum showPlayerTurnMenu(const Player* player);
    int showActionMenu(const std::vector<std::string>& options);
    int showCardSelection(const std::vector<std::unique_ptr<Card>>& cards,
                          const std::vector<size_t>& playableIndices);

    int selectAttacker(const std::vector<std::unique_ptr<UnitCard>>& units);
    std::pair<int, Player*> selectAttackTarget(Player* opponent,
                                               const std::vector<std::unique_ptr<UnitCard>>& enemyUnits);
    void displayBattleResults(const BattleSystem::BattleResult& result);
    void displayAttackOptions(const std::vector<std::unique_ptr<UnitCard>>& units,
                              const std::vector<size_t>& canAttackIndices);
    void displayTargetChoice(Player* opponent);

    void displayPlayerInfo(const Player* player);
    void displayPlayerInfo(Player* player);
    void displayHealthStatus(int health, int maxHealth);
    void displayManaStatus(int mana, int maxMana);
    void displayBoard(Player* player, Player* opponent);


    void showCardPlayedMessage(const std::string& cardName, bool isSpell);
    void showAvailableActions(Player* currentPlayer);

    int getValidatedChoice(int min, int max, const std::string& prompt);

    GameEngine* getGameState() const { return gameState; }
    InputHandler& getInputHandler() { return inputHandler; }

    void logGameEvents(const std::vector<std::unique_ptr<GameEvent>>& events);
};

#endif