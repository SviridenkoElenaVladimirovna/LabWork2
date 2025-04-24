#ifndef GAMEENGINE_H
#define GAMEENGINE_H

#include <memory>
#include <vector>
#include "../Players/Player.h"
#include "../Data/PlayerState.h"
#include "../Systems/BattleSystem.h"
#include "../Systems/GameHistory.h"
#include "TurnManager.h"
#include "../Cards/SpellCard.h"
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
    std::vector<std::unique_ptr<PlayerState>> playerStates;
    InputHandler inputHandler;
    std::unique_ptr<BattleSystem> battleSystem;

    std::unique_ptr<UnitCard> createBasicUnitCards() const;
    std::unique_ptr<SpellCard> createBasicSpellCards() const;
    std::unique_ptr<UnitCard> createSpecialUnitCards() const;

    void initializeDecks();

    void nextTurn();
    bool checkGameOver();
    int selectDifficulty() const;
    void drawCardForCurrentPlayer();
    void displayPlayerState() const;

    int getValidPlayerChoice(int min, int max, const std::string& prompt) const;
    int getValidatedChoice(int min, int max, const std::string& prompt) const;
    void handlePlayCard();
    void handleAttack();

public:
    InputHandler& getInputHandler() {
        return uiManager->getInputHandler();
    }
    explicit GameEngine(bool showLogs = false);
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
    bool playerHasPlayableCards() const;
    bool playerCanAttack() const;
    bool isGameOver() const;
    UIManager* getUIManager() const { return uiManager.get(); }
    bool isGameRunning() const {
        return players.size() >= 2 &&
               players[0] && !players[0]->isDefeated() &&
               players[1] && !players[1]->isDefeated();
    }

    int getPlayerActionChoice() const;
    void initializeGame();
    void displayFullGameState();
    void toggleLogs();
    bool isShowingLogs() const { return showLogs; }
    void showSettingsMenu();
    ~GameEngine() = default;
    void displayGameHistory();
    void startGame();
    void playerTurn();
    void handleAiTurn();
    void displayGameState() const;

};

#endif
