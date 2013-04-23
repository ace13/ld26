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
    cont->setImpl(new Components::QuadTree(*cont, sf::FloatRect(0,0,2000,2000), 0, 3));
    sys.addComponent(mWorldID, cont);
    sys.finalizeEntity(mWorldID);
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
