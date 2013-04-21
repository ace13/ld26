#include "InputManager.hpp"
#include <SFML/Window/Event.hpp>

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

void InputManager::handleEvent(const sf::Event& ev)
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
            found.Value = std::abs(ev.joystickMove.position)/100.f;
        }
        break;
    }

    if (found.Bind != Input::Bind_None)
    {
        if (!mCurrentlyBinding.empty() && found.Value > 0.75f)
            mInputs[mCurrentlyBinding] = found;
        else if (mCurrentlyBinding.empty())
            for (auto it = mInputs.begin(), end = mInputs.end(); it != end; ++it)
            {
                if (it->second.Bind != found.Bind)
                    continue;
                
                it->second.Value = found.Value;
            }
    }
}

float InputManager::getInput(const std::string& bind) const
{
    if (mInputs.count(bind) > 0)
        return mInputs.at(bind).Value;

    return 0.f;
}
