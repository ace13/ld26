#include <unordered_map>
#include <string>

#pragma once

namespace sf { class Event; }

struct Input
{
    Input() : Bind(Bind_None), Value(0) { }

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

    void addBind(const std::string&, const sf::Event& bind);
    void startBind(const std::string&);
    void handleEvent(const sf::Event&);

    inline bool isBinding() const { return !mCurrentlyBinding.empty(); }

    float getInput(const std::string&) const;

private:
    Input parseInput(const sf::Event&);

    std::unordered_map<std::string, Input> mInputs;
    std::string mCurrentlyBinding;
};