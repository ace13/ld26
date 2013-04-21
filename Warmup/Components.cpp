#include "Components.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>

using namespace Components;

Physical::Physical() : Kunlaboro::Component("Components.Physical")
{
}

void Physical::addedToEntity()
{
    requestMessage("GetPos",    [this](Kunlaboro::Message& msg){ msg.payload = sf::Vector2f(mX, mY); msg.handled = true; }, true);
    requestMessage("GetRot",    [this](Kunlaboro::Message& msg){ msg.payload = mRot; msg.handled = true; }, true);
    requestMessage("GetRadius", [this](Kunlaboro::Message& msg){ msg.payload = mRadius; msg.handled = true; }, true);

    requestMessage("SetPos",    [this](const Kunlaboro::Message& msg){ sf::Vector2f pos = boost::any_cast<sf::Vector2f>(msg.payload); mX = pos.x; mY = pos.y; }, true);
    requestMessage("SetRot",    [this](const Kunlaboro::Message& msg){ mRot = boost::any_cast<float>(msg.payload); }, true);
    requestMessage("SetRadius", [this](const Kunlaboro::Message& msg){ mRadius = boost::any_cast<float>(msg.payload); }, true);
}

Inertia::Inertia() : Kunlaboro::Component("Components.Inertia")
{
}

void Inertia::addedToEntity()
{
    requestMessage("LD26.Update", [this](const Kunlaboro::Message& msg)
        {
            float dt = boost::any_cast<float>(msg.payload);

            sf::Vector2f curPos = boost::any_cast<sf::Vector2f>(sendQuestionToEntity(getOwnerId(), "GetPos").payload);
            curPos.x += mX * dt;
            curPos.y += mY * dt;

            float curRot = boost::any_cast<float>(sendQuestionToEntity(getOwnerId(), "GetRot").payload);
            curRot += mRot * dt;

            sendMessageToEntity(getOwnerId(), "SetPos", curPos);
            sendMessageToEntity(getOwnerId(), "SetRot", curRot);
        });
}

Drawable::Drawable() : Kunlaboro::Component("Components.Drawable")
{
}

void Drawable::addedToEntity()
{
    requestMessage("SetTexture", [this](const Kunlaboro::Message& msg){ mTex.loadFromFile(boost::any_cast<std::string>(msg.payload)); }, true);
    requestMessage("LD26.Draw",  [this](const Kunlaboro::Message& msg)
        {
            sf::RenderTarget& target = *boost::any_cast<sf::RenderTarget*>(msg.payload);

            sf::Sprite sprite(mTex);
            sprite.setOrigin((sf::Vector2f)mTex.getSize()/2.f);

            sprite.setPosition(boost::any_cast<sf::Vector2f>(sendQuestionToEntity(getOwnerId(), "GetPos").payload));
            sprite.setRotation(boost::any_cast<float>(sendQuestionToEntity(getOwnerId(), "GetRot").payload));

            target.draw(sprite);
        });
}