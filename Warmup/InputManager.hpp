#include <unordered_map>
#include <string>

#pragma once

namespace sf { class Event; }

struct Input
{
    enum
    {
        Bind_None,
        Bind_Key,
        Bind_Button,
        Bind_Axis
    } Bind;

    union
    {
        struct
        {
            int Key;
            bool Ctrl;
            bool Alt;
            bool Shift;
        } Key;
        struct
        {
            int Joystick;
            int Button;
        } Button;
        struct
        {
            int Joystick;
            int Axis;
            bool Positive;
        } Axis;
    };

    float Value;
};

class InputManager
{
public:
    InputManager();
    ~InputManager();

    void startBind(const std::string&);
    void handleEvent(const sf::Event&);

    float getInput(const std::string&) const;

private:
    std::unordered_map<std::string, Input> mInputs;
    std::string mCurrentlyBinding;
};