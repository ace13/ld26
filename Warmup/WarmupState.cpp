#include "WarmupState.hpp"
#include "PlayerInput.hpp"
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

    sys.addComponent(id, "PlayerInput");

    Components::Physical* phys = static_cast<Components::Physical*>(sys.createComponent("Components.Physical"));
    phys->setPos(sf::Vector2f(888, 189));
    phys->setRadius(10);
    phys->setRot(0);
    sys.addComponent(id, phys);
    
    Components::ShapeDrawable* shape = static_cast<Components::ShapeDrawable*>(sys.createComponent("Components.ShapeDrawable"));
    {   
        sf::ConvexShape* conv = new sf::ConvexShape(4);
        
        conv->setFillColor(sf::Color::Transparent);
        conv->setOutlineColor(sf::Color::White);
        conv->setOutlineThickness(3);

        conv->setPoint(0, sf::Vector2f(64, 0));
        conv->setPoint(1, sf::Vector2f(-16, -16));
        conv->setPoint(2, sf::Vector2f(-16, 16));
        conv->setPoint(3, sf::Vector2f(64, 0));

        shape->setShape(conv);
    }
    sys.addComponent(id, shape);

    

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
