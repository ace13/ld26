#include <Kunlaboro/Component.hpp>

#pragma once

namespace sf { class RenderTarget; }
namespace Components { class Physical; class Inertia; }

class EnemyController : public Kunlaboro::Component
{
public:
    EnemyController();
    ~EnemyController();

    void addedToEntity();

private:
    void update(float dt);
    void draw(sf::RenderTarget& target);

    Components::Physical* mPhys;
    Components::Inertia* mInert;
};
