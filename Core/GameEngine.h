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

    InputHandler& getInputHandler() { return uiManager->getInputHandler(); }
    BattleSystem& getBattleSystem() { return *battleSystem; }

    void addHumanPlayer(const std::string& name, int health, int mana);
    void addAIPlayer(int difficulty);

    Player* getCurrentPlayer() const { return currentPlayer; }
    Player* getOpponent() const {
        return (currentPlayer == players[0].get()) ? players[1].get() : players[0].get();
    }

    UiActionsEnum showActionMenu(const std::vector<std::pair<UiActionsEnum, std::string>>& options);
    void showBoardState();
    void handleHumanTurn();
    void handleAiTurn();

    bool playerHasPlayableCards() const;
    bool playerCanAttack() const;
    bool isGameOver() const;

    UIManager* getUIManager() const { return uiManager.get(); }
    bool isShowingLogs() const { return showLogs; }

    void initializeGame();
    void toggleLogs();
    void showSettingsMenu();
    void startGame();
    void displayGameState() const;

    bool checkGameOver();
};

#endif