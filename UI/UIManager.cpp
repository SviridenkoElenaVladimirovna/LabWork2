#include "UIManager.h"
#include "../Core/GameEngine.h"
#include <iostream>
#include <thread>

UIManager::UIManager(GameEngine* engine) : gameState(engine) {
}

std::vector<std::pair<UiActionsEnum, std::string>> UIManager::buildPlayerActions(const Player* player) {
    std::vector<std::pair<UiActionsEnum, std::string>> options;

    bool hasValidPlayableCards = false;
    const auto& cards = player->getHand().getCards();
    for (const auto& card : cards) {
        if (card->getCost() <= player->getMana() && !card->isPlayed()) {
            hasValidPlayableCards = true;
            break;
        }
    }

    if (hasValidPlayableCards) {
        options.emplace_back(UiActionsEnum::PLAY_CARD, "Play card");
    }

    if (player->canAttack()) {
        options.emplace_back(UiActionsEnum::ATTACK, "Attack");
    }

    options.emplace_back(UiActionsEnum::SHOW_BOARD, "Show battlefield");
    options.emplace_back(UiActionsEnum::SETTINGS, "Settings");
    options.emplace_back(UiActionsEnum::END_TURN, "End turn");

    return options;
}

UiActionsEnum UIManager::showActionMenu(const std::vector<std::pair<UiActionsEnum, std::string>>& options) {
    displayHighlightedMessage("YOUR TURN");
    displayMessage("Available actions:");

    for (const auto& [action, desc] : options) {
        displayMessage(std::to_string(uiActionsEnumToInt(action)) + ". " + desc);
    }

    return inputHandler.getActionInput(uiActionsEnumToInt(options.front().first),
                                       uiActionsEnumToInt(options.back().first),
                                       "Your choice: ");
}

void UIManager::displayGameState(Player* currentPlayer, Player* opponent, int turnNumber) {
    std::cout << "\n******* TURN: " << turnNumber << " *******\n";

    std::cout << "=== OPPONENT === [" << opponent->getName() << "]\n";
    displayPlayerInfo(opponent);
    std::cout << "Cards in hand: " << opponent->getHand().getCards().size() << "\n";
    std::cout << "Opponent's battlefield:\n";
    displayBattlefield(opponent->getBattlefield());

    std::cout << "\n----------------------------------------\n";

    std::cout << "=== YOU === [" << currentPlayer->getName() << "]\n";
    displayPlayerInfo(currentPlayer);
    displayHighlightedMessage("YOUR HAND");
    displayCards(currentPlayer->getHand().getCards());
}

int UIManager::getValidatedChoice(int min, int max, const std::string& prompt) {
    return inputHandler.getIntInput(min, max, prompt);
}

void UIManager::displayMessage(const std::string& message) {
    std::cout << message << "\n";
}

void UIManager::displayLogMessage(const std::string& message) {
    std::cout << "[LOG] " << message << "\n";
}

void UIManager::displayHighlightedMessage(const std::string &message) {
    std::cout << "\n === " << message << " === \n";
}

void UIManager::displayAttackOptions(const std::vector<std::unique_ptr<UnitCard>>& units,
                                     const std::vector<size_t>& canAttackIndices) {
    std::cout << "\nChoose attacking unit:\n";
    for (size_t i = 0; i < canAttackIndices.size(); ++i) {
        size_t idx = canAttackIndices[i];
        std::cout << i+1 << ". ";
        units[idx]->displayInfo();
        std::cout << "\n";
    }
}

void UIManager::displayTargetChoice(Player* opponent) {
    std::cout << "\nChoose attack target:\n";
    const auto& enemyUnits = opponent->getBattlefield();

    for (size_t i = 0; i < enemyUnits.size(); ++i) {
        if (enemyUnits[i]->getHealth() > 0) {
            std::cout << i+1 << ". ";
            enemyUnits[i]->displayInfo();
            std::cout << "\n";
        }
    }

    std::cout << enemyUnits.size()+1 << ". Attack hero ("
              << opponent->getName() << ")\n";
}

int UIManager::showCardSelection(const std::vector<std::unique_ptr<Card>>& cards,
                                 const std::vector<size_t>& playableIndices) {
    std::cout << "\nChoose card to play:\n";
    for (size_t i = 0; i < playableIndices.size(); ++i) {
        std::cout << i + 1 << ". ";
        cards[playableIndices[i]]->displayInfo();
        std::cout << "\n";
    }

    int choice = getValidatedChoice(0, playableIndices.size(),
                                    "Your choice (0 to cancel): ");

    return (choice == 0) ? -1 : (choice - 1);
}

void UIManager::showCardPlayedMessage(const std::string& cardName, bool isSpell) {
    displayHighlightedMessage("CARD PLAYED");

    if (isSpell) {
        displayMessage("Spell cast: " + cardName);
    } else {
        displayMessage(cardName + " is now on the battlefield!");
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
}

void UIManager::displayPlayerInfo(Player* player) {
    std::cout << "Health: " << player->getHealth() << "/" << player->getMaxHealth() << "\n";
    std::cout << "Mana: " << player->getMana() << "/" << player->getMaxMana() << "\n";
}

void UIManager::displayCards(const std::vector<std::unique_ptr<Card>>& cards) {
    for (size_t i = 0; i < cards.size(); ++i) {
        std::cout << i+1 << ". ";
        cards[i]->displayInfo();
        std::cout << "\n";
    }
}

void UIManager::displayBattlefield(const std::vector<std::unique_ptr<UnitCard>>& units) {
    if (units.empty()) {
        std::cout << "Battlefield is empty\n";
        return;
    }

    for (size_t i = 0; i < units.size(); ++i) {
        std::cout << i+1 << ". ";
        if (!units[i]) {
            std::cout << "[CORRUPTED UNIT]\n";
            continue;
        }
        try {
            units[i]->displayInfo();
            std::cout << (units[i]->isExhausted() ? " [Exhausted]" : " [Ready]");
            std::cout << "\n";
        } catch (...) {
            std::cout << "[DISPLAY ERROR]\n";
        }
    }
}

void UIManager::displayBoard(Player* player, Player* opponent) {
    std::cout << "\n=== BATTLEFIELD ===" << std::endl;
    std::cout << "OPPONENT:" << std::endl;
    displayBattlefield(opponent->getBattlefield());
    std::cout << "YOU:" << std::endl;
    displayBattlefield(player->getBattlefield());
    std::cout << "----------------------------------------" << std::endl;
}

void UIManager::displayBattleResults(const BattleSystem::BattleResult& result) {
    std::cout << "\n=== BATTLE RESULT ===\n";
    std::cout << result.attackerName << " attacked " << result.defenderName
              << " and dealt " << result.damageDealt << " damage\n";

    if (result.counterDamage > 0) {
        std::cout << result.defenderName << " counterattacked for "
                  << result.counterDamage << " damage\n";
    }

    if (result.defenderDestroyed) {
        std::cout << result.defenderName << " was destroyed!\n";
    }
    if (result.attackerDestroyed) {
        std::cout << result.attackerName << " was destroyed!\n";
    }

    std::cout << "=====================\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(1500));
}

void UIManager::logGameEvents(const std::vector<std::unique_ptr<GameEvent>>& events) {
    for (int i = 0; i < events.size(); i++) {
        displayLogMessage(events.at(i)->toString());
    }
}
InputHandler& UIManager::getInputHandler() {
    return inputHandler;
}