#include "WarmupState.hpp"
#include "Components.hpp"
#include <SFML/Graphics.hpp>

GameState::GameState() : IState("Warmup")
{
}

GameState::~GameState()
{

}

void GameState::setup()
{
    Kunlaboro::EntitySystem& sys = *getEntitySystem();
    mWorldID = sys.createEntity();
    Components::SpatialContainer* cont = static_cast<Components::SpatialContainer*>(sys.createComponent("Components.SpatialContainer"));
    cont->setImpl(new Components::QuadTree(*cont, sf::FloatRect(0,0,2000,2000), 0, 4));
    sys.addComponent(mWorldID, cont);
    sys.finalizeEntity(mWorldID);

    Kunlaboro::EntityId id = sys.createEntity();
    Components::Physical* phys = static_cast<Components::Physical*>(sys.createComponent("Components.Physical"));
    phys->setPos(sf::Vector2f(888, 189));
    sys.addComponent(id, phys);
    sys.finalizeEntity(id);

    cont->addEntity(id);
}

void GameState::update(float dt)
{

}

void GameState::draw(sf::RenderTarget& target)
{

}

void GameState::drawUi(sf::RenderTarget& target)
{
    sf::Vector2f size = target.getView().getSize();
    sf::RectangleShape shape(sf::Vector2f(size.x, 128));

    shape.setFillColor(sf::Color(255, 255, 255, 200));

    target.draw(shape);
}
