#include "InputHandler.h"

std::string InputHandler::getInput() const {
    std::string input;
    std::getline(std::cin, input);
    return input;
}

int InputHandler::getIntInput(int min, int max, const std::string& prompt) const {
    bool needPrompt = true;

    while (true) {
        if (needPrompt) {
            std::cout << prompt;
        }

        std::string input = getInput();

        if (!input.empty() && !std::isdigit(input[0])) {
            auto it = commandHandlers.find(input);
            if (it != commandHandlers.end()) {
                it->second();
                needPrompt = true;
                continue;
            } else {
                std::cout << "Unknown command. ";
                std::cout << "Please enter a number between " << min << " and " << max << ".\n";
                needPrompt = true;
                continue;
            }
        }

        try {
            int choice = std::stoi(input);
            if (choice >= min && choice <= max) {
                return choice;
            }
        } catch (...) {
        }

        std::cout << "Invalid input. Please enter a number between " << min << " and " << max << ".\n";
        needPrompt = true;
    }
}

void InputHandler::registerCommand(const std::string& command, std::function<void()> handler) {
    commandHandlers[command] = handler;
}

UiActionsEnum InputHandler::getActionInput(int min, int max, const std::string& prompt) const {
    switch (getIntInput(min, max, prompt)) {
        case 1: return UiActionsEnum::PLAY_CARD;
        case 2: return UiActionsEnum::ATTACK;
        case 3: return UiActionsEnum::SHOW_BOARD;
        case 4: return UiActionsEnum::SETTINGS;
        case 5: return UiActionsEnum::END_TURN;
        default: return UiActionsEnum::CANCEL;
    }
}