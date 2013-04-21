#include "StateManager.hpp"

#pragma once

class GameState : public IState
{
public:
    GameState();
    ~GameState();

    virtual void update(float dt);
    virtual void draw(sf::RenderTarget& target);
    virtual void drawUi(sf::RenderTarget& target);

private:

};