#include "GameEngine.h"
#include "../Players/HumanPlayer.h"
#include "../Players/EasyAI.h"
#include "../Players/MediumAI.h"
#include "../Players/HardAI.h"
#include "../Cards/UnitCard.h"
#include "../Cards/SpellCard.h"
#include <iostream>
#include "../Core/TurnManager.h"
#include <limits>
#include <algorithm>
#include <thread>

GameEngine::GameEngine(bool showLogs)
        : showLogs(showLogs),
          turnCount(1),
          battleSystem(new BattleSystem(static_cast<GameState*>(this))),
          uiManager(new UIManager(this)),
          turnManager(nullptr)
{
}

void GameEngine::startGame() {
    uiManager->displayHighlightedMessage("GAME SETTING");

    int difficulty = uiManager->getInputHandler()
        .getIntInput(
            1,
            3,
            "Select the complexity of the AI:\n"
            "1. Easy\n"
            "2. Medium\n"
            "3. Hard\n"
            "Your choice: ");

    history->recordEvent(GameEvent::of(EventType::USER_ACTION, "Player select AI difficulty: " + difficulty));

    addHumanPlayer("Player", 30, 1);
    addAIPlayer(difficulty);

    initializeGame();

    while (!isGameOver()) {
        if (currentPlayer == players[0].get()) {
            displayGameState();
            handleHumanTurn();
        } else {
            uiManager->displayHighlightedMessage("THE ENEMY'S MOVE [" + currentPlayer->getName() + "]");
            currentPlayer->startTurn();
            currentPlayer->takeTurn();
        }

        turnManager->endTurn();
        currentPlayer = turnManager->getCurrentPlayer();
    }

    checkGameOver();
}


void GameEngine::handleHumanTurn() {
    currentPlayer->startTurn();
    currentPlayer->takeTurn();
}

void GameEngine::handleAiTurn() {
    uiManager->displayHighlightedMessage("THE ENEMY'S MOVE [" + currentPlayer->getName() + "]");
    currentPlayer->startTurn();
    currentPlayer->takeTurn();
}
void GameEngine::showSettingsMenu() {
    while (true) {
        uiManager->displayHighlightedMessage("SETTINGS");

        std::vector<std::pair<int, std::string>> options = {
                  {1, "Display the log"},
                {2, "Back in the game"}
        };

        int choice = uiManager->getInputHandler().getIntInput(1, 2,
                                                              "1. " + options[0].second + "\n2. " + options[1].second + "\nYour choice: ");
        if (choice == 1) {
            uiManager->displayHighlightedMessage("System Logs");
            uiManager->logGameEvents(history->getHistory());
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        } else {
            break;
        }
    }
}
void GameEngine::initializeGame() {
    for (auto& player : players) {
        player->setInitialMana(1);
        history->recordEvent(GameEvent::of(EventType::SYSTEM, "Set initial mana for user " + player -> getName()));
    }

    players[0]->setOpponent(players[1].get());
    players[1]->setOpponent(players[0].get());

    std::vector<Player*> playerOrder;
    for (auto& player : players) {
        playerOrder.push_back(player.get());
    }
    turnManager = std::make_unique<TurnManager>(playerOrder);

    initializeDecks();

    history->recordEvent(GameEvent::of(EventType::SYSTEM, "Initialized decks"));

    for (auto& player : players) {
        for (int i = 0; i < 3; ++i) {
            bool isDrawCard = player->drawCard();

            if (isDrawCard) {
                history->recordEvent(
                    GameEvent::of(
                        EventType::CARD,
                        "Player " + player->getName() + " has drawn card " +
                        player->getHand().getCards().back()->getName()));
            } else {
                uiManager->displayHighlightedMessage("THE DECK RAN OUT OF CARDS");

                history->recordEvent(
                    GameEvent::of(
                        EventType::CARD, "Player " + player->getName() + " does't draw card"));
            }
        }
    }

    currentPlayer = turnManager->getCurrentPlayer();
}
void GameEngine::addHumanPlayer(const std::string& name, int health, int mana) {
    players.emplace_back(new HumanPlayer(name, health, mana, static_cast<GameState*>(this)));
}

void GameEngine::addAIPlayer(int difficulty) {
    switch(difficulty) {
        case 1:
            players.emplace_back(new EasyAI("Easy AI", 30, 1, static_cast<GameState*>(this), uiManager.get()));

            break;
        case 2:
            players.emplace_back(new MediumAI("Medium AI", 30, 1, static_cast<GameState*>(this), uiManager.get()));
            break;
        case 3:
            players.emplace_back(new HardAI("Hard AI", 30, 1, static_cast<GameState*>(this), uiManager.get()));
            break;
    }
}
void GameEngine::displayGameState() const {
    Player* human = players[0].get();
    Player* opponent = players[1].get();

    if (uiManager) {
        uiManager->displayGameState(human, opponent, turnManager->getTurnCount());
    }
}

UiActionsEnum GameEngine::showActionMenu(const std::vector<std::pair<UiActionsEnum, std::string>>& options) {
    uiManager->displayHighlightedMessage("YOUR WAY");
    uiManager->displayMessage("Available actions");
    for (const auto& [action, desc] : options) {
        uiManager->displayMessage(std::to_string(uiActionsEnumToInt(action)) + ". " + desc);
    }

    return inputHandler.getActionInput(uiActionsEnumToInt(options.front().first),
                                uiActionsEnumToInt(options.back().first),
                                "Your choice: ");
}

void GameEngine::showBoardState() {
    uiManager->displayBoard(players[0].get(), players[1].get());
}


bool GameEngine::playerHasPlayableCards() const {
    const auto& cards = players[0]->getHand().getCards();
    return std::any_of(cards.begin(), cards.end(),
                       [this](const auto& card) {
                           return card->getCost() <= players[0]->getMana();
                       });
}

bool GameEngine::playerCanAttack() const {
    const auto& battlefield = players[0]->getBattlefield();
    return std::any_of(battlefield.begin(), battlefield.end(),
                       [](const auto& unit) {
                           return unit->canAttackNow();
                       });
}

void GameEngine::drawCardForCurrentPlayer() {
    if (!currentPlayer->getDeck().isEmpty()) {
        currentPlayer->drawCard();
        uiManager->displayMessage(currentPlayer->getName() + " takes the card!");
    } else {
        uiManager->displayMessage("deck " + currentPlayer->getName() + " empty!");
    }
}

bool GameEngine::checkGameOver() {
    for (const auto& player : players) {
        if (player->getHealth() <= 0) {
            uiManager->displayHighlightedMessage("END OF GAME");
            uiManager->displayMessage(player->getName() + " defeated!");
            uiManager->displayMessage(player->getOpponent()->getName() + " wins!");
            return true;
        }
    }
    return false;
}

void GameEngine::toggleLogs() {
    showLogs = !showLogs;
}

bool GameEngine::isGameOver() const {
    return players[0]->getHealth() <= 0 || players[1]->getHealth() <= 0;
}

void GameEngine::initializeDecks() {
    for (auto& player : players) {
        auto& deck = player->getDeck();
        deck.initializeStandardDeck();

        if (player.get() != players[0].get()) {
            deck.addCard(std::make_unique<UnitCard>("Dark Magician", 4, 3, 5, true));
        }
    }
}
