#include <Kunlaboro/Component.hpp>
#include <SFML/Graphics/Texture.hpp>

#pragma once

namespace Components
{
 
class Physical : public Kunlaboro::Component
{
public:
    Physical();

    void addedToEntity();

private:
    float mX, mY, mRot, mRadius;
};

class Inertia : public Kunlaboro::Component
{
public:
    Inertia();

    void addedToEntity();

private:
    float mX, mY, mRot;
};

class Drawable : public Kunlaboro::Component
{
public:
    Drawable();

    void addedToEntity();

private:
    sf::Texture mTex;
};

}
