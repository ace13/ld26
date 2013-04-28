#include <Kunlaboro/Component.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/RenderTexture.hpp>

#pragma once

namespace sf { class RenderTarget; class View; }
namespace Components { class MetaPhysical; class Physical; class Inertia; }
class InputManager;

class PlayerController : public Kunlaboro::Component
{
public:
    PlayerController();
    ~PlayerController();

    void addedToEntity();

private:
    void update(float dt);
    void draw(sf::RenderTarget& target);

    InputManager* mInput;
    Components::MetaPhysical* mMeta;
    Components::Physical* mPhys;
    Components::Inertia* mInert;
    sf::View* mView;
    int mKills;

    sf::RenderTexture mTexture;
};