#include "HumanPlayer.h"
#include <vector>
#include <chrono>
#include <thread>
#include "../Cards/SpellCard.h"


HumanPlayer::HumanPlayer(const std::string& name, int health, int mana, GameState* gameState) :
        Player(name, health, mana, gameState) {}

bool HumanPlayer::hasPlayableCards() const {
    const auto& cards = getHand().getCards();
    return std::any_of(cards.begin(), cards.end(),
                       [this](const auto& card) {
                           return card->getCost() <= getMana() && !card->isPlayed();
                       });
}
void HumanPlayer::takeTurn() {
    GameEngine* gameEngine = dynamic_cast<GameEngine*>(getGameState());
    if (!gameEngine) return;

    UIManager* ui = gameEngine->getUIManager();
    bool turnEnded = false;

    while (!turnEnded && !gameEngine->isGameOver()) {
        gameEngine->showBoardState();

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
                break;

            case UiActionsEnum::END_TURN:
                turnEnded = true;
                gameEngine->updateState();
                break;

            default:
                ui->displayMessage("Invalid choice!");
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
                break;
        }
    }
}

void HumanPlayer::handleAttack() {
    auto* gameEngine = dynamic_cast<GameEngine*>(getGameState());
    if (!gameEngine) return;

    UIManager* ui = gameEngine->getUIManager();

    cleanBattlefield();
    getOpponent()->cleanBattlefield();

    auto& battlefield = getBattlefield();
    std::vector<size_t> canAttackIndices;
    for (size_t i = 0; i < battlefield.size(); ++i) {
        if (battlefield[i] && battlefield[i]->canAttackNow() && !battlefield[i]->isDead()) {
            canAttackIndices.push_back(i);
        }
    }

    if (canAttackIndices.empty()) {
        ui->displayMessage("No units available to attack");
        return;
    }

    ui->displayAttackOptions(battlefield, canAttackIndices);
    int attackerChoice = gameEngine->getInputHandler().getIntInput(
            0, canAttackIndices.size(), "Choose attacker (0 to cancel): ");

    if (attackerChoice == 0) return;

    size_t attackerIndex = canAttackIndices[attackerChoice - 1];
    Player* opponent = getOpponent();

    bool hasEnemyUnits = std::any_of(opponent->getBattlefield().begin(),
                                     opponent->getBattlefield().end(),
                                     [](const auto& unit) {
                                         return unit && !unit->isDead();
                                     });

    ui->displayTargetChoice(opponent);
    int maxTarget = hasEnemyUnits ? opponent->getBattlefield().size() + 1 : 1;

    int targetChoice = gameEngine->getInputHandler().getIntInput(
            0, maxTarget, "Select target (0 to cancel): ");

    if (targetChoice == 0) return;

    size_t targetIndex = (hasEnemyUnits && targetChoice <= opponent->getBattlefield().size())
                         ? targetChoice - 1
                         : std::numeric_limits<size_t>::max();

    BattleSystem::BattleResult result = attackWithUnit(attackerIndex, targetIndex);

    cleanBattlefield();
    getOpponent()->cleanBattlefield();
    if (targetIndex != std::numeric_limits<size_t>::max()) {
        gameEngine->updateState();
    }
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

    int cardCost = handCards[cardIndex]->getCost();

    if (auto* spellCard = dynamic_cast<SpellCard*>(handCards[cardIndex].get())) {
        spellCard->play(this, gameEngine->getOpponent());
    } else if (auto* unitCard = dynamic_cast<UnitCard*>(handCards[cardIndex].get())) {
        playCard(cardIndex);
    }

    try {
        hand.removeCard(cardIndex);
        mana -= cardCost;

        logEvent("Card", "plays out " + cardName);
        if (gameState) {
            gameState->getGameHistory().recordEvent(
                    GameEvent::of(EventType::CARD,
                                  "Player " + getName() + " plays " + cardName)
            );
        }
    } catch (const std::exception& e) {
        logEvent("Error", "Failed to remove card: " + std::string(e.what()));
    }
    ui->showCardPlayedMessage(cardName, isSpell);
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
BattleSystem& HumanPlayer::getBattleSystem() {
    return *battleSystem;
}