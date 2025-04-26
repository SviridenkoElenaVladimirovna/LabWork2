#include "HumanPlayer.h"
#include "../Core/GameEngine.h"
#include "../Cards/UnitCard.h"
#include "../UI/UIManager.h"
#include <vector>
#include <chrono>
#include <thread>
#include "../UI/InputHandler.h"


HumanPlayer::HumanPlayer(const std::string& name, int health, int mana, GameState* gameState) :
        Player(name, health, mana, gameState) {}

bool HumanPlayer::hasPlayableCards() const {
    const auto& cards = getHand().getCards();
    return std::any_of(cards.begin(), cards.end(),
                       [this](const auto& card) {
                           return card->getCost() <= getMana() && !card->isPlayed();
                       });
}
void HumanPlayer::handlePlayCard() {
    auto* gameEngine = dynamic_cast<GameEngine*>(getGameState());
    if (!gameEngine) return;

    UIManager* ui = gameEngine->getUIManager();

    if (!hasPlayableCards()) {
        ui->displayMessage("There are no cards available for drawing!");
        return;
    }

    const auto& handCards = getHand().getCards();
    const auto playableIndices = getPlayableCardIndices();

    int choice = ui->showCardSelection(handCards, playableIndices);
    if (choice == -1) {
        ui->displayMessage("Cancellation of the card draw");
        return;
    }

    size_t cardIndex = playableIndices[choice];
    auto cardName = handCards[cardIndex]->getName();
    bool isSpell = dynamic_cast<SpellCard*>(handCards[cardIndex].get()) != nullptr;

    playCard(cardIndex);
    ui->showCardPlayedMessage(cardName, isSpell);
}
void HumanPlayer::takeTurn() {
    GameEngine* gameEngine = dynamic_cast<GameEngine*>(getGameState());
    if (!gameEngine) return;

    UIManager* ui = gameEngine->getUIManager();
    bool turnEnded = false;

    while (!turnEnded && !gameEngine->isGameOver()) {
        std::vector<std::pair<UiActionsEnum, std::string>> options = ui->buildPlayerActions(this);
        UiActionsEnum choice = ui->showActionMenu(options);

        switch (choice) {
            case UiActionsEnum::PLAY_CARD:
                handlePlayCard();
                break;
            case UiActionsEnum::ATTACK:
                handleAttack();
                break;
            case UiActionsEnum::SHOW_BOARD:
                gameEngine->showBoardState();
                break;
            case UiActionsEnum::SETTINGS:
                gameEngine->showSettingsMenu();
                gameEngine->displayGameState();
                break;
            case UiActionsEnum::END_TURN:
                turnEnded = true;
                break;
            default:
                ui->displayMessage("Wrong choice!");
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                break;
        }
    }
}
void HumanPlayer::handleAttack() {
    auto* gameEngine = dynamic_cast<GameEngine*>(getGameState());
    if (!gameEngine) return;

    UIManager* ui = gameEngine->getUIManager();
    auto& battlefield = getBattlefield();

    std::vector<size_t> canAttackIndices;
    for (size_t i = 0; i < battlefield.size(); ++i) {
        if (battlefield[i] && battlefield[i]->canAttackNow() && !battlefield[i]->isDead()) {
            canAttackIndices.push_back(i);
        }
    }

    if (canAttackIndices.empty()) {
        ui->displayMessage("There are no creatures capable of attacking!");
        return;
    }

    ui->displayAttackOptions(battlefield, canAttackIndices);
    int attackerChoice = gameEngine->getInputHandler().getIntInput(
            0, canAttackIndices.size(), "Choose an attacking creature (0 cancel): ");
    if (attackerChoice == 0) return;

    size_t attackerIndex = canAttackIndices[attackerChoice - 1];
    auto* opponent = getOpponent();

    ui->displayTargetChoice(opponent);
    int targetChoice = gameEngine->getInputHandler().getIntInput(
            0, opponent->getBattlefield().size() + 1, "Select a target (0 cancel): ");
    if (targetChoice == 0) return;

    size_t targetIndex = targetChoice <= opponent->getBattlefield().size() ? targetChoice - 1 : std::numeric_limits<size_t>::max();

    BattleSystem::BattleResult result = attackWithUnit(attackerIndex, targetIndex);

    ui->displayBattleResults(result);

    gameEngine->updateState();
    gameEngine->showBoardState();
}


void HumanPlayer::startTurn() {
    Player::startTurn();
}
std::vector<size_t> HumanPlayer::getPlayableCardIndices() const {
    std::vector<size_t> indices;
    const auto& cards = getHand().getCards();

    for (size_t i = 0; i < cards.size(); ++i) {
        if (cards[i]->getCost() <= getMana()) {
            indices.push_back(i);
        }
    }
    return indices;
}

bool HumanPlayer::canAttack() const {
    return std::any_of(getBattlefield().begin(), getBattlefield().end(),
                       [](const auto& unit) { return unit->canAttackNow(); });
}
