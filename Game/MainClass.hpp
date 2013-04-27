#include "StateManager.hpp"
#include "InputManager.hpp"
#include "SettingsManager.hpp"
#include "SoundManager.hpp"
#include "Telemetry.hpp"
#include <SFML/Graphics/View.hpp>

#pragma once

class MainClass
{
public:
    MainClass(int, char**);
    ~MainClass();

    int operator()();

private:
    sf::View mGame, mUi;
    SettingsManager mSettings;
    SoundManager mSounds;
    StateManager mManager;
    InputManager mInput;
    Telemetry mTelem;
};