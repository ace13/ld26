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
    cont->setImpl(new Components::QuadTree(*cont, sf::FloatRect(0,0,16000,16000), 0, 4));
    sys.addComponent(mWorldID, cont);
    sys.finalizeEntity(mWorldID);

    Kunlaboro::EntityId id = sys.createEntity();

    sys.addComponent(id, "PlayerInput");

    Components::Physical* phys = static_cast<Components::Physical*>(sys.createComponent("Components.Physical"));
    phys->setPos(sf::Vector2f(888, 189));
    phys->setRadius(64);
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
    sys.addComponent(id, "Components.Inertia");

    sys.finalizeEntity(id);

    cont->addEntity(id);

    mStars.resize(3200);
    for (int i = 0; i < mStars.size(); ++i)
    {
        sf::Vector2f pos(rand()%16000, rand()%16000);
        mStars[i].position = pos;
        mStars[i].color = sf::Color::White;
    }

    mBigStars.resize(1600);
    for (int i = 0; i < mBigStars.size()/4; i+=4)
    {
        sf::Vector2f pos(rand()%16000, rand()%16000);
        mBigStars[i].position = pos;
        mBigStars[i].color = sf::Color::White;
        mBigStars[i+1].position = pos + sf::Vector2f(8, 0);
        mBigStars[i+1].color = sf::Color::Yellow;
        mBigStars[i+2].position = pos + sf::Vector2f(8, 8);
        mBigStars[i+2].color = sf::Color::White;
        mBigStars[i+3].position = pos + sf::Vector2f(0, 8);
        mBigStars[i+3].color = sf::Color::Yellow;
    }
}

void GameState::update(float dt)
{
}

void GameState::draw(sf::RenderTarget& target)
{
    target.draw(&mStars.front(), mStars.size(), sf::Points);
    target.draw(&mBigStars.front(), mBigStars.size(), sf::Quads);

    // Draw Stars here
}

void GameState::drawUi(sf::RenderTarget& target)
{
    /*sf::Vector2f size = target.getView().getSize();
    sf::RectangleShape shape(sf::Vector2f(size.x, 32));

    shape.setFillColor(sf::Color(255, 255, 255, 200));

    target.draw(shape);*/
}
