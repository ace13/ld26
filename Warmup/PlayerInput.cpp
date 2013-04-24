#include "PlayerInput.hpp"

PlayerInput::PlayerInput() : Kunlaboro::Component("PlayerInput"), mPhysical(NULL), mInput(NULL)
{
}

PlayerInput::~PlayerInput()
{

}

void PlayerInput::addedToEntity()
{
    requestMessage("LD26.Update", [this](const Kunlaboro::Message& msg) {
        update(boost::any_cast<float>(msg.payload));
    });

    requireComponent("Components.Physical", [this](const Kunlaboro::Message& msg) {
        mPhysical = static_cast<Components::Physical*>(msg.sender);
    });

    Kunlaboro::Message msg = sendGlobalQuestion("Get.Input");
    if (msg.handled)
        mInput = boost::any_cast<InputManager*>(msg.payload);
}

void PlayerInput::update(float dt)
{
    static float rad = 3.14159f/180.f;

    if (mInput == NULL)
        return;

    float acc = mInput->getInput("Forward");
    float steer = mInput->getInput("Right") - mInput->getInput("Left");

    sf::Vector2f pos = mPhysical->getPos();
    float rot = mPhysical->getRot();

    rot += steer * dt * 120;
    while (rot > 360)
        rot -= 360;
    while (rot < 0)
        rot += 360;

    pos.x += std::cosf(rot*rad) * acc * dt * 160;
    pos.y += std::sinf(rot*rad) * acc * dt * 160;

    mPhysical->setPos(pos);
    mPhysical->setRot(rot);
}