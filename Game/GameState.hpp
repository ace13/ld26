#include "StateManager.hpp"
#include <vector>

#pragma once

namespace sf { class Shape; }

class GameState : public IState
{
public:
    GameState();
    ~GameState();

    virtual void setup();
    virtual void update(float dt);
    virtual void draw(sf::RenderTarget& target);
    virtual void drawUi(sf::RenderTarget& target);

private:
    bool mModePressed;

    float mEditorZoom;
    float mEditorRot;

    float mToneIn;

    bool mInEditor;
    sf::View mSnapshot;

    Kunlaboro::EntityId mWorld;
    Kunlaboro::EntityId mPlayer;
};