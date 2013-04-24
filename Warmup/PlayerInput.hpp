#include "InputManager.hpp"
#include "Components.hpp"
#include <Kunlaboro/Component.hpp>
#include <SFML/Graphics/View.hpp>

#pragma once

class PlayerInput : public Kunlaboro::Component
{
public:
    PlayerInput();
    ~PlayerInput();

    void addedToEntity();

    void update(float dt);
    void draw(sf::RenderTarget&);

private:
    Components::Physical* mPhysical;
    Components::Inertia*  mInertial;
    InputManager* mInput;
    float mAcc;
    bool mFired;
    sf::View* mView;
};
