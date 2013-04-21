#include "WarmupState.hpp"
#include <SFML/Graphics.hpp>

GameState::GameState() : IState("Warmup")
{
}

GameState::~GameState()
{

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
