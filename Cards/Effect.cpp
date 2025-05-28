/*
Sviridenko Elena st130482@student.spbu.ru
The system under development is a C++ console card game inspired by Hearthstone. 
*/
#include "../Players/EasyAI.h"
#include "UnitCard.h" 
#include "Effect.h"
#include <iostream>

Effect::Effect(const std::string& type) : type(type) {}
