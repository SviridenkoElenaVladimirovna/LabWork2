/*
Sviridenko Elena st130482@student.spbu.ru
The system under development is a C++ console card game inspired by Hearthstone. 
*/
/**
 * @file InputHandler.h
 * @brief Handles user input and commands.
 */
#ifndef INPUTHANDLER_H
#define INPUTHANDLER_H

#include <string>
#include <functional>
#include <map>
#include <iostream>
#include <cctype>

#include "UiActionsEnum.h"
/**
 * @class InputHandler
 * @brief Controls the input of numbers and text commands.
 */
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
