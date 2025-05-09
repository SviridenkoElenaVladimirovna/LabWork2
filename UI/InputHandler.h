#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <string>
#include <functional>
#include <map>
#include <iostream>
#include <cctype>

#include "UiActionsEnum.h"

class InputHandler {
private:
    std::map<std::string, std::function<void()>> commandHandlers;

public:
    InputHandler() = default;

    std::string getInput() const;
    int getIntInput(int min, int max, const std::string& prompt = "") const;
    void registerCommand(const std::string& command, std::function<void()> handler);
    UiActionsEnum getActionInput(int min, int max, const std::string& prompt = "") const;
};

#endif