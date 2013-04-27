#include "PlayerController.hpp"
#include "Components.hpp"
#include "InputManager.hpp"
#include "Math.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <iostream>

PlayerController::PlayerController(): Kunlaboro::Component("PlayerController"), mInput(NULL), mPhys(NULL), mInert(NULL), mView(NULL), mSize(0)
{

}

PlayerController::~PlayerController()
{
}

void PlayerController::addedToEntity()
{
    requestMessage("LD26.Update", [this](const Kunlaboro::Message& msg) { update(boost::any_cast<float>(msg.payload)); });
    requestMessage("LD26.Draw",   [this](const Kunlaboro::Message& msg) { draw(*boost::any_cast<sf::RenderTarget*>(msg.payload)); });
    requestMessage("Collision",   [this](const Kunlaboro::Message& msg)
    {

    }, true);

    requireComponent("Components.Physical", [this](const Kunlaboro::Message& msg) { mPhys = static_cast<Components::Physical*>(msg.sender); });
    requireComponent("Components.Inertia",  [this](const Kunlaboro::Message& msg) { mInert = static_cast<Components::Inertia*>(msg.sender); });

    sf::ConvexShape* shape = new sf::ConvexShape(4);
    shape->setPoint(0, sf::Vector2f( 15,  0));
    shape->setPoint(1, sf::Vector2f(-15,  10));
    shape->setPoint(2, sf::Vector2f(-15, -10));
    shape->setPoint(3, sf::Vector2f( 15,  0));

    shape->setFillColor(sf::Color::Transparent);
    shape->setOutlineColor(sf::Color::White);
    shape->setOutlineThickness(2.5f);
    sendMessage("SetShape", (sf::Shape*)shape);

    if (mView == NULL)
    {
        Kunlaboro::Message msg = sendGlobalQuestion("Get.GameView");
        if (msg.handled)
            mView = boost::any_cast<sf::View*>(msg.payload);
    }

    if (mInput == NULL)
    {
        Kunlaboro::Message msg = sendGlobalQuestion("Get.Input");
        if (msg.handled)
            mInput = boost::any_cast<InputManager*>(msg.payload);
    }
}

void PlayerController::update(float dt)
{
    if (mPhys == NULL || mView == NULL || mInert == NULL)
        return;

    float up    = mInput->getInput("Up"),
          down  = mInput->getInput("Down"),
          left  = mInput->getInput("Left"),
          right = mInput->getInput("Right");

    sf::Vector2f pos = mPhys->getPos();
    float curAng = mPhys->getRot() * deg2rad;

    sf::Vector2f targetVec(right-left, down-up);
    if (up < 0.2f && down < 0.2f && left < 0.2f && right < 0.2f)
    {
        targetVec.x = 0;
        targetVec.y = 0;
    }

    float targetDot = targetVec.x * targetVec.x + targetVec.y * targetVec.y;
    float len = sqrt(targetDot);
    len = std::min(len, 1.f);
    
    float targetAng = atan2(targetVec.y, targetVec.x);
    if (targetVec == sf::Vector2f()) 
        targetAng = curAng;

    float slerpedAng = slerp(curAng, targetAng, dt * 1);

    mPhys->setRot(slerpedAng * rad2deg);

    targetVec.x = cos(curAng);
    targetVec.y = sin(curAng);

    sf::Vector2f curSpeed = mInert->getSpeed();
    float curDot = curSpeed.x * curSpeed.x + curSpeed.y * curSpeed.y;
    targetDot = targetVec.x * targetVec.x + targetVec.y * targetVec.y;

    {
        float sLen = sqrt(curDot);
        sLen = sLen / 240;
        
        float targt = (sLen + dt * 1.5f * (len - sLen));

        mInert->setSpeed(targetVec * targt * 240.f);
    }

    if (pos.x < 32 || pos.x > 15968 || pos.y < 32 || pos.y > 15968)
    {
        sf::Vector2f sp = mInert->getSpeed();
        if ((pos.x < 32 && sp.x < 0) || (pos.x > 15968 && sp.x > 0))
            sp.x = 0;
        if ((pos.y < 32 && sp.y < 0) || (pos.y > 15968 && sp.y > 0))
            sp.y = 0;
        mInert->setSpeed(sp);
    }

    {
        mView->setCenter(pos);
    }
}

void PlayerController::draw(sf::RenderTarget& target)
{
    if (mPhys == NULL)
        return;


}