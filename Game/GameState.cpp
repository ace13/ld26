#include "GameState.hpp"
#include "Components.hpp"
#include "Math.hpp"
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/ConvexShape.hpp>

GameState::GameState(): IState("GameState")
{
}

GameState::~GameState()
{
}

void GameState::setup()
{
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

    for (int i = 0; i < 128; ++i)
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

    for (int i = 0; i < 1200; ++i)
    {
        Kunlaboro::EntityId e = sys.createEntity();
        sys.addComponent(e, "Components.Physical");
        sys.addComponent(e, "Components.Inertia");
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
        shape->setOutlineColor(sf::Color(255,255,255, 24));
        shape->setOutlineThickness(18.f + rand()%24);

        sendMessageToEntity(e, "SetShape", shape);
        sendMessageToEntity(e, "SetOrigin");
        sendMessageToEntity(e, "SetPos", sf::Vector2f(rand()%16000, rand()%16000));
        sendMessageToEntity(e, "SetRotSpeed", (float)(90 - rand()%180)/45.f);
    }
}
void GameState::update(float dt)
{
}
void GameState::draw(sf::RenderTarget& target)
{
}
void GameState::drawUi(sf::RenderTarget& target)
{

}
