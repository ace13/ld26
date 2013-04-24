#include "InputManager.hpp"
#include "Components.hpp"
#include <Kunlaboro/Component.hpp>

#pragma once

class PlayerInput : public Kunlaboro::Component
{
public:
    PlayerInput();
    ~PlayerInput();

    void addedToEntity();

    void update(float dt);

private:
    Components::Physical* mPhysical;
    InputManager* mInput;
};
