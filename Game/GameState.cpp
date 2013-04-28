#include "GameState.hpp"
#include "Components.hpp"
#include "InputManager.hpp"
#include "Math.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>

GameState::GameState(): IState("GameState"), mModePressed(false), mEditorZoom(200), mEditorRot(90), mToneIn(-1), mInEditor(false)
{
}

GameState::~GameState()
{
}

void GameState::setup()
{
    changeRequestPriority("LD26.Draw", 5);
    Kunlaboro::EntitySystem& sys = *getEntitySystem();

    mWorld = sys.createEntity();
    Components::SpatialContainer* cont = static_cast<Components::SpatialContainer*>(sys.createComponent("Components.SpatialContainer"));
    cont->setImpl(new Components::QuadTree(*cont, sf::FloatRect(0, 0, 16000, 16000), 0, 5));
    sys.addComponent(mWorld, cont);
    sys.finalizeEntity(mWorld);

    Kunlaboro::EntityId player = sys.createEntity();
    sys.addComponent(player, "Components.MetaPhysical");
    sys.addComponent(player, "Components.Physical");
    sys.addComponent(player, "Components.Inertia");
    sys.addComponent(player, "Components.ShapeDrawable");
    sys.addComponent(player, "Components.SharpCorners");
    sys.addComponent(player, "PlayerController");
    sys.finalizeEntity(player);

    mPlayer = player;

    for (int i = 0; i < 128+rand()%64; ++i)
    {
        sf::CircleShape* circ = new sf::CircleShape(32.f);

        Kunlaboro::EntityId e = sys.createEntity();
        sys.addComponent(e, "Components.MetaPhysical");
        sys.addComponent(e, "Components.Physical");
        sys.addComponent(e, "Components.Inertia");
        sys.addComponent(e, "Components.ShapeDrawable");
        sys.addComponent(e, "Components.SharpCorners");
        sys.addComponent(e, "EnemyController");
        sys.finalizeEntity(e);

        sendMessageToEntity(e, "SetShape", (sf::Shape*)circ);
        sendMessageToEntity(e, "SetOrigin");
        sendMessageToEntity(e, "SetRadius", 32.f);
        sendMessageToEntity(e, "SetPos", sf::Vector2f(rand()%16000, rand()%16000));

        Kunlaboro::Message msg(Kunlaboro::Type_Message, sys.getAllComponentsOnEntity(e, "EnemyController")[0]);;
        sys.sendLocalMessage(mWorld, sys.getMessageRequestId(Kunlaboro::Reason_Message, "StoreMe"), msg);
    }

    Kunlaboro::Message msg(Kunlaboro::Type_Message, sys.getAllComponentsOnEntity(player, "PlayerController")[0]);;
    sys.sendLocalMessage(mWorld, sys.getMessageRequestId(Kunlaboro::Reason_Message, "StoreMe"), msg);

    for (int i = 0; i < 1200+rand()%640; ++i)
    {
        Kunlaboro::EntityId e = sys.createEntity();
        Kunlaboro::Component* phys = sys.createComponent("Components.Physical");
        Kunlaboro::Component* inert = sys.createComponent("Components.Inertia");
        sys.addComponent(e, phys);
        sys.addComponent(e, inert);
        sys.addComponent(e, "Components.ShapeDrawable");
        sys.finalizeEntity(e);

        int type = rand() % 3;
        sf::Shape* shape;

        switch(type)
        {
        case 0:
            shape = new sf::CircleShape(128 + rand()%256);
            break;

        case 1:
            shape = new sf::RectangleShape(sf::Vector2f(256 + rand()%256, 256 + rand()%256));
            break;

        case 2:
            shape = new sf::ConvexShape(3);
            sf::ConvexShape& tri = *(sf::ConvexShape*)shape;
            tri.setPoint(0, sf::Vector2f(-128 * 1+(rand()%100/100.f), 128 * 1+(rand()%100/100.f)));
            tri.setPoint(1, sf::Vector2f(128 * 1+(rand()%100/100.f), -128 * 1+(rand()%100/100.f)));
            tri.setPoint(2, sf::Vector2f(128 * 1+(rand()%100/100.f), 128 * 1+(rand()%100/100.f)));
            break;
        }

        shape->setFillColor(sf::Color::Transparent);
        shape->setOutlineColor(sf::Color(200+rand()%56,200+rand()%56,200+rand()%56, 24));
        float size = 1+(rand()%48)/2.f;
        shape->setOutlineThickness(18.f + size);
        
        sendMessageToEntity(e, "SetRadius", size);
        sendMessageToEntity(e, "SetShape", shape);
        sendMessageToEntity(e, "SetOrigin");
        sendMessageToEntity(e, "SetPos", sf::Vector2f(rand()%16000, rand()%16000));
        sendMessageToEntity(e, "SetRotSpeed", (float)(90 - rand()%180)/45.f);
        sendMessageToEntity(e, "SetSpeed", sf::Vector2f(4 - rand()%8, 4 - rand()%8) * 24.f / size);

        phys->requestMessage("LD26.Update", [phys](const Kunlaboro::Message& msg)
        {
            Components::Physical& p = (Components::Physical&)*phys;

            sf::Vector2f pos = p.getPos();
            if (pos.x < -1000)
                pos.x = 17000;
            if (pos.x > 17000)
                pos.x = -1000;
            if (pos.y < -1000)
                pos.y = 17000;
            if (pos.y > 17000)
                pos.y = -1000;

            p.setPos(pos);
        });
    }
}
void GameState::update(float dt)
{
    InputManager* input = getInput();
    Kunlaboro::Message msg = sendGlobalQuestion("Get.GameView");
    sf::View& gameView = *boost::any_cast<sf::View*>(msg.payload);
    bool mode = input->getInput("Mode") > 0.5f;
    bool act1 = input->getInput("Action1") > 0.75f;
    bool act2 = input->getInput("Action2") > 0.75f;

    float up    = input->getInput("Up"),
          down  = input->getInput("Down"),
          left  = input->getInput("Left"),
          right = input->getInput("Right");

    if (mode && !mModePressed)
    {
        mModePressed = true;
        std::cerr << "Switching mode to ";

        if (mInEditor)
            std::cerr << "Game";
        else
            std::cerr << "Editor";

        std::cerr << std::endl;
        if (mToneIn == -1)
            mToneIn = mInEditor;
        mInEditor = !mInEditor;

        if (mInEditor && mToneIn == 0)
        {
            mSnapshot = gameView;
        }
    }
    else if (!mode && mModePressed)
        mModePressed = false;

    sf::Vector2f currentSize = gameView.getSize();
    sf::Vector2f targetSize = currentSize;
    float currentAng = gameView.getRotation();
    float targetAng = currentAng;
    sf::Vector2f currentPos = gameView.getCenter();
    sf::Vector2f targetPos = currentPos;

    if (mInEditor)
    {
        if (mToneIn < 1)
        {
            mToneIn = std::max(0.f, std::min(1.f, mToneIn + dt));
        }

        sf::Vector2f inputVec(right-left, down-up);

        if (act2)
        {
            mEditorRot += (right - left) * 100 * dt;
            mEditorZoom += (down - up) * 150 * dt;
        }

        float aspect = currentSize.x/currentSize.y;
        targetSize = sf::Vector2f(mEditorZoom * aspect, mEditorZoom);

        msg = sendQuestionToEntity(mPlayer, "GetPos");
        targetPos = boost::any_cast<sf::Vector2f>(msg.payload);

        msg = sendQuestionToEntity(mPlayer, "GetRot");
        targetAng = boost::any_cast<float>(msg.payload) + mEditorRot;
    }
    else
    {
        if (mToneIn > 0)
        {
            mToneIn = std::max(0.f, std::min(1.f, mToneIn - dt));
        }

        targetSize = mSnapshot.getSize();
        targetPos = mSnapshot.getCenter();
        targetAng = mSnapshot.getRotation();
    }

    if (mToneIn > 0 && mToneIn < 1)
    {
        gameView.setSize(currentSize + dt * 3 * (targetSize - currentSize));
        gameView.setCenter(currentPos + dt * 3 * (targetPos - currentPos));
        gameView.setRotation(slerp(currentAng * deg2rad, targetAng * deg2rad, dt * 3) * rad2deg);
    }
    else if (mToneIn == 0 || mToneIn == 1)
    {
        gameView.setCenter(targetPos);
        gameView.setSize(targetSize);
        gameView.setRotation(targetAng);

        mToneIn = -1;
    }

    if (mInEditor || mToneIn > 0.25f)
        setHandled();
}
void GameState::draw(sf::RenderTarget& target)
{
    sf::View view = target.getView();

    if (mInEditor || mToneIn > 0)
    {
        float alpha = (mToneIn < 0 ? 1 : mToneIn) * 255;

        sf::RectangleShape tone(view.getSize());
    
        tone.setOrigin(view.getSize()/2.f);
        tone.setPosition(view.getCenter());
        tone.setRotation(view.getRotation());
        tone.setFillColor(sf::Color(0,0,0, alpha-55));

        target.draw(tone);
        
        Kunlaboro::Message msg = sendQuestionToEntity(mPlayer, "GetRadius");
        sf::CircleShape platform(boost::any_cast<float>(msg.payload) + 24.f);

        msg = sendQuestionToEntity(mPlayer, "GetPos");
        platform.setOrigin(platform.getRadius(), platform.getRadius());
        platform.setFillColor(sf::Color(100,96,96, alpha));
        platform.setOutlineColor(sf::Color(210,200,200, alpha));
        platform.setOutlineThickness(2.f);
        platform.setPosition(boost::any_cast<sf::Vector2f>(msg.payload));

        target.draw(platform);
    }
}
void GameState::drawUi(sf::RenderTarget& target)
{
    
}
