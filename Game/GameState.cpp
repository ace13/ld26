#include "GameState.hpp"
#include "Components.hpp"
#include <SFML/Graphics/CircleShape.hpp>

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

    for (int i = 0; i < 64; ++i)
    {
        sf::CircleShape* circ = new sf::CircleShape(12.f);

        Kunlaboro::EntityId e = sys.createEntity();
        sys.addComponent(e, "Components.MetaPhysical");
        sys.addComponent(e, "Components.Physical");
        sys.addComponent(e, "Components.Inertia");
        sys.addComponent(e, "Components.ShapeDrawable");
        //sys.addComponent(e, "Components.SharpCorners");
        sys.addComponent(e, "EnemyController");
        sys.finalizeEntity(e);

        sendMessageToEntity(e, "SetShape", (sf::Shape*)circ);
        sendMessageToEntity(e, "SetOrigin");
        sendMessageToEntity(e, "SetRadius", 12.f);
        sendMessageToEntity(e, "SetPos", sf::Vector2f(rand()%16000, rand()%16000));

        Kunlaboro::Message msg(Kunlaboro::Type_Message, sys.getAllComponentsOnEntity(e, "EnemyController")[0]);;
        sys.sendLocalMessage(mWorld, sys.getMessageRequestId(Kunlaboro::Reason_Message, "StoreMe"), msg);
    }

    Kunlaboro::Message msg(Kunlaboro::Type_Message, sys.getAllComponentsOnEntity(player, "PlayerController")[0]);;
    sys.sendLocalMessage(mWorld, sys.getMessageRequestId(Kunlaboro::Reason_Message, "StoreMe"), msg);
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