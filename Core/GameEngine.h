/**
 * @file GameEngine.h
 * @brief The main game class that controls gameplay.
 */
#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <memory>
#include <vector>
#include "../Players/Player.h"
#include "../Systems/BattleSystem.h"
#include "../Systems/GameHistory.h"
#include "TurnManager.h"
#include "../UI/InputHandler.h"
#include "../UI/UIManager.h"
#include "GameState.h"
/**
 * @class GameEngine
 * @brief Main game controller class that manages the game loop and state.
 */

class GameEngine : public GameState {
private:
    bool showLogs;
    std::unique_ptr<TurnManager> turnManager;
    std::vector<std::unique_ptr<Player>> players;
    Player* currentPlayer;
    int turnCount;
    std::unique_ptr<UIManager> uiManager;
    InputHandler inputHandler;
    std::unique_ptr<BattleSystem> battleSystem;

    void initializeDecks();
    void drawCardForCurrentPlayer();

public:
    explicit GameEngine(bool showLogs = false);

    const std::vector<std::unique_ptr<Player>>& getPlayers() const;
    TurnManager* getTurnManager() const;
    Player* getCurrentPlayer() const;
    InputHandler& getInputHandler();
    BattleSystem& getBattleSystem();
    UIManager* getUIManager() const;
    bool isShowingLogs() const;

    Player* getOpponent() const;
    void addHumanPlayer(const std::string& name, int health, int mana);
    void addAIPlayer(int difficulty);

    UiActionsEnum showActionMenu(const std::vector<std::pair<UiActionsEnum, std::string>>& options);
    void showBoardState();
    void handleHumanTurn();
    void handleAiTurn();
    bool playerHasPlayableCards() const;
    bool playerCanAttack() const;
    bool isGameOver() const;
    void initializeGame();
    void toggleLogs();
    void showSettingsMenu();
    void startGame();
    void displayGameState() const;
    bool checkGameOver();
};

#endif