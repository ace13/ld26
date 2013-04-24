#include "PlayerInput.hpp"
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <Kunlaboro/EntitySystem.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>

const float rad = 3.14159f/180.f;

PlayerInput::PlayerInput() : Kunlaboro::Component("PlayerInput"), mPhysical(NULL), mInertial(NULL), mInput(NULL), mAcc(0), mView(NULL), mFired(false)
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
    requestMessage("LD26.Draw", [this](const Kunlaboro::Message& msg) {
        draw(*boost::any_cast<sf::RenderTarget*>(msg.payload));
    });

    requestMessage("Collision", [this](const Kunlaboro::Message& msg) {
        Components::Physical* other = static_cast<Components::Physical*>(msg.sender);
        if (other == NULL)
            return;

        sf::Vector2f diff = (other->getPos() - mPhysical->getPos());
        float len = diff.x*diff.x + diff.y*diff.y;
        diff /= sqrt(len);

        sf::Vector2f speed;
        Kunlaboro::Message msg2 = sendQuestionToEntity(other->getOwnerId(), "GetSpeed");
        if (!msg2.handled)
            return;
        speed = boost::any_cast<sf::Vector2f>(msg2.payload);
        sf::Vector2f mySpeed = mInertial->getSpeed();
        sf::Vector2f sdiff = mySpeed - speed;
        float speedLen = sdiff.x*sdiff.x + sdiff.y*sdiff.y;
        

        diff *= sqrt(speedLen);

        sendMessageToEntity(other->getOwnerId(), "SetSpeed", diff);

    }, true);

    changeRequestPriority("LD26.Draw", -1);

    requireComponent("Components.Physical", [this](const Kunlaboro::Message& msg) {
        mPhysical = static_cast<Components::Physical*>(msg.sender);
    });
    requireComponent("Components.Inertia", [this](const Kunlaboro::Message& msg) {
        mInertial = static_cast<Components::Inertia*>(msg.sender);
    });

    Kunlaboro::Message msg = sendGlobalQuestion("Get.Input");
    if (msg.handled)
        mInput = boost::any_cast<InputManager*>(msg.payload);

    msg = sendGlobalQuestion("Get.GameView");
    if (msg.handled)
        mView = boost::any_cast<sf::View*>(msg.payload);
}

void PlayerInput::update(float dt)
{
    if (mInput == NULL || mView == NULL)
    {
        if (mView == NULL)
        {
            Kunlaboro::Message msg = sendGlobalQuestion("Get.GameView");
            if (msg.handled)
                mView = boost::any_cast<sf::View*>(msg.payload);
        }

        return;
    }

    mAcc = mInput->getInput("Forward");
    float steer = mInput->getInput("Right") - mInput->getInput("Left");

    sf::Vector2f inertia = mInertial->getSpeed();
    sf::Vector2f pos = mPhysical->getPos();
    float rot = mPhysical->getRot();

    rot += steer * dt * 120;
    while (rot > 360)
        rot -= 360;
    while (rot < 0)
        rot += 360;

    if (mAcc > 0.1f)
    {
        inertia.x += std::cosf(rot*rad) * mAcc * dt * 512;
        inertia.y += std::sinf(rot*rad) * mAcc * dt * 512;
    }

    float len = inertia.x * inertia.x + inertia.y * inertia.y;
    if (len > 1024 * 1024)
    {
        inertia /= sqrt(len);
        inertia *= 1024.f;
    }

    bool fix = false;

    if (pos.x < 32) {
        pos.x = 15936;
        fix = true; 
    }
    else if (pos.x > 15936) {
        pos.x = 32;
        fix = true; 
    }

    if (pos.y < 32) {
        pos.y = 15936;
        fix = true; 
    }
    else if (pos.y > 15936) {
        pos.y = 32;
        fix = true; 
    }

    if (fix)
        mPhysical->setPos(pos);

    mInertial->setSpeed(inertia);
    mPhysical->setRot(rot);

    mView->setCenter(pos);

    bool fire = mInput->getInput("Fire") > 0.5f;
    if (fire && !mFired)
    {
        sf::Vector2f fireAng = sf::Vector2f(std::cosf(rot*rad), std::sinf(rot*rad));
        sf::Vector2f firePos = pos + fireAng * 64.f;
        sf::Vector2f fireSpeed = inertia + fireAng * 256.f;

        Kunlaboro::EntitySystem& sys = *getEntitySystem();

        Kunlaboro::EntityId shot = sys.createEntity();
        sys.addComponent(shot, "Components.Physical");
        sys.addComponent(shot, "Components.Inertia");
        // sys.addComponent(shot, "PlayerInput");
        Components::ShapeDrawable* shape = static_cast<Components::ShapeDrawable*>(sys.createComponent("Components.ShapeDrawable"));
        shape->setShape(new sf::CircleShape(8.f)); // sf::RectangleShape(sf::Vector2f(32, 16))
        sys.addComponent(shot, shape);
        sys.finalizeEntity(shot);
        
        mPhysical->getContainer()->addEntity(shot);

        sendMessageToEntity(shot, "SetPos", firePos);
        sendMessageToEntity(shot, "SetRadius", 8.f);
        sendMessageToEntity(shot, "SetRotSpeed", (float)(rand()%360-180));
        sendMessageToEntity(shot, "SetSpeed", fireSpeed);

        mFired = true;
    }
    else if (!fire && mFired)
        mFired = false;
}

void PlayerInput::draw(sf::RenderTarget& target)
{
    sf::Vector2f pos = mPhysical->getPos();
    float rot = mPhysical->getRot();

    sf::Vector2f X(cos((rot+90)*rad), sin((rot+90)*rad));
    sf::Vector2f Y(cos(rot*rad), sin(rot*rad));

    sf::ConvexShape flames(8);

    flames.setFillColor(sf::Color::Transparent);
    flames.setOutlineColor(sf::Color(24, 79, 255));
    flames.setOutlineThickness(3.f);

    float BASE = 42.f;
    float rand1 = 5*((rand()%100) / 100.f);
    float rand2 = 5*((rand()%100) / 100.f);

    flames.setPoint(0, X*16.f          * mAcc - Y*(BASE));
    flames.setPoint(1, X*(22.f+rand1)  * mAcc - Y*(BASE + (24+rand2)       * mAcc));
    flames.setPoint(2, X*8.f           * mAcc - Y*(BASE + 12               * mAcc));
    flames.setPoint(3, X*0.f                  - Y*(BASE + (64+rand1-rand2) * mAcc));
    flames.setPoint(4, X*-8.f          * mAcc - Y*(BASE + 12               * mAcc));
    flames.setPoint(5, X*(-22.f-rand2) * mAcc - Y*(BASE + (24+rand1)       * mAcc));
    flames.setPoint(6, X*-16.f         * mAcc - Y*(BASE));
    flames.setPoint(7, X*16.f          * mAcc - Y*(BASE));

    flames.setPosition(pos);

    if (mAcc > 0.1f)
        target.draw(flames);
}
