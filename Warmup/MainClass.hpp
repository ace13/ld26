#include "StateManager.hpp"
#include "InputManager.hpp"
#include "SettingsManager.hpp"

#pragma once

class MainClass
{
public:
    MainClass(int, char**);
    ~MainClass();

    int operator()();

private:
    SettingsManager mSettings;
    StateManager mManager;
    InputManager mInput;
};