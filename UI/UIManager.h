/*
Sviridenko Elena st130482@student.spbu.ru
The system under development is a C++ console card game inspired by Hearthstone. 
*/
/**
 * @file UIManager.h
 * @brief Responsible for displaying the game interface.
 */
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
/**
 * @class UIManager
 * @brief Manages all UI: menus, battlefield, maps and logs.
 */
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

    int showCardSelection(const std::vector<std::unique_ptr<Card>>& cards,
                          const std::vector<size_t>& playableIndices);
    void displayAttackOptions(const std::vector<std::unique_ptr<UnitCard>>& units,
                              const std::vector<size_t>& canAttackIndices);
    void displayTargetChoice(Player* opponent);

    void displayPlayerInfo(Player* player);
    void displayBoard(Player* player, Player* opponent);


    void showCardPlayedMessage(const std::string& cardName, bool isSpell);

    int getValidatedChoice(int min, int max, const std::string& prompt);

    InputHandler& getInputHandler();

    void logGameEvents(const std::vector<std::unique_ptr<GameEvent>>& events);
};

#endif
