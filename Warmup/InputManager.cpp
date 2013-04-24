#include "InputManager.hpp"
#include <SFML/Window/Event.hpp>
#include <cmath>

InputManager::InputManager()
{
}

InputManager::~InputManager()
{

}

void InputManager::startBind(const std::string& bind)
{
    if (mInputs.count(bind) > 0)
        mCurrentlyBinding = bind;
}

void InputManager::addBind(const std::string& name, const sf::Event& bind)
{
    Input found = parseInput(bind);

    if (found.Bind != Input::Bind_None && found.Value > 0.75f)
    {
        found.Value = 0;
        mInputs[name] = found;
    }
}

void InputManager::handleEvent(const sf::Event& ev)
{
    Input found = parseInput(ev);

    if (found.Bind != Input::Bind_None)
    {
        if (!mCurrentlyBinding.empty() && found.Value > 0.75f)
            mInputs[mCurrentlyBinding] = found;
        else if (mCurrentlyBinding.empty())
            for (auto it = mInputs.begin(), end = mInputs.end(); it != end; ++it)
            {
                if (it->second.Bind != found.Bind)
                    continue;

                bool equal = false;
                switch(it->second.Bind)
                {
                case Input::Bind_Key:
                    equal = found.Key.Alt == it->second.Key.Alt &&
                            found.Key.Ctrl == it->second.Key.Ctrl &&
                            found.Key.Shift == it->second.Key.Shift &&
                            found.Key.Key == it->second.Key.Key;
                    break;
                }
                
                if (equal)
                    it->second.Value = found.Value;
            }
    }
}

Input InputManager::parseInput(const sf::Event& ev)
{
    Input found;
    found.Bind = Input::Bind_None;

    switch (ev.type)
    {
    case sf::Event::KeyPressed:
    case sf::Event::KeyReleased:
        {
            found.Bind = Input::Bind_Key;
            found.Key.Key = ev.key.code;
            found.Key.Alt = ev.key.alt;
            found.Key.Ctrl = ev.key.control;
            found.Key.Shift = ev.key.shift;
            found.Value = ev.type == sf::Event::KeyPressed;
        }
        break;

    case sf::Event::JoystickButtonPressed:
    case sf::Event::JoystickButtonReleased:
        {
            found.Bind = Input::Bind_Button;
            found.Button.Joystick = ev.joystickButton.joystickId;
            found.Button.Button = ev.joystickButton.button;
            found.Value = ev.type == sf::Event::JoystickButtonPressed;
        }
        break;

    case sf::Event::JoystickMoved:
        {
            found.Bind = Input::Bind_Axis;
            found.Axis.Joystick = ev.joystickMove.joystickId;
            found.Axis.Axis = ev.joystickMove.axis;
            found.Axis.Positive = ev.joystickMove.position > 0;
            found.Value = fabs(ev.joystickMove.position)/100.f;
        }
        break;
    }

    return found;
}

float InputManager::getInput(const std::string& bind) const
{
    if (mInputs.count(bind) > 0)
        return mInputs.at(bind).Value;

    return 0.f;
}
