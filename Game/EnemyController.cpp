#include "EnemyController.hpp"
#include "Components.hpp"
#include "Math.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include <Kunlaboro/EntitySystem.hpp>

EnemyController::EnemyController(): Kunlaboro::Component("EnemyController")
{
}

EnemyController::~EnemyController()
{
}

void EnemyController::addedToEntity()
{
    requestMessage("LD26.Update", [this](const Kunlaboro::Message& msg)
    {
        update(boost::any_cast<float>(msg.payload));
    });

    requestMessage("LD26.Draw", [this](const Kunlaboro::Message& msg)
    {
        draw(*boost::any_cast<sf::RenderTarget*>(msg.payload));
    });
    changeRequestPriority("LD26.Draw", 1);

    requestMessage("Collide", [this](const Kunlaboro::Message& msg)
    {
        if (!getEntitySystem()->getAllComponentsOnEntity(msg.sender->getOwnerId(), "EnemyController").empty())
        {
            std::cerr << "TODO: Bounce." << std::endl;

        }
    }, true);

    requireComponent("Components.Physical", [this](const Kunlaboro::Message& msg) { mPhys = static_cast<Components::Physical*>(msg.sender); });
    requireComponent("Components.Inertia",  [this](const Kunlaboro::Message& msg) { mInert = static_cast<Components::Inertia*>(msg.sender); });
}

void EnemyController::update(float dt)
{
    if (mPhys == NULL || mInert == NULL)
        return;

    sf::Vector2f dir = mInert->getSpeed();

    dir.x = std::max(-120.f, std::min(dir.x + (60 - rand()%120), 120.f));
    dir.y = std::max(-120.f, std::min(dir.y + (60 - rand()%120), 120.f));

    mInert->setSpeed(dir);

    sf::Vector2f pos = mPhys->getPos();

    if (pos.x < 32 || pos.x > 15968 || pos.y < 32 || pos.y > 15968)
    {
        sf::Vector2f sp = mInert->getSpeed();
        if ((pos.x < 32 && sp.x < 0) || (pos.x > 15968 && sp.x > 0))
            sp.x = 0;
        if ((pos.y < 32 && sp.y < 0) || (pos.y > 15968 && sp.y > 0))
            sp.y = 0;
        mInert->setSpeed(sp);
    }
}

void EnemyController::draw(sf::RenderTarget& target)
{
    if (mPhys == NULL || mInert == NULL)
        return;
    /*
    sf::Text angs;
    Kunlaboro::Message msg = sendGlobalQuestion("Get.Font");
    sf::Font& font = *boost::any_cast<sf::Font*>(msg.payload);

    angs.setColor(sf::Color::Red);
    angs.setFont(font);
    angs.setCharacterSize(8);

    msg = sendQuestion("GetPoints");
    const std::vector<std::pair<sf::Vector2f, float> >& points = *boost::any_cast<const std::vector<std::pair<sf::Vector2f, float> >*>(msg.payload);

    float ang = mPhys->getRot() * deg2rad;
    sf::Vector2f Y(cos(ang + pi/2), sin(ang + pi/2));
    sf::Vector2f X(cos(ang), sin(ang));

    for (int i = 0; i < points.size(); ++i)
    {
        angs.setPosition(mPhys->getPos() + (X * points[i].first.x + Y * points[i].first.y));
        
        char tmp[4];
        sprintf_s(tmp, "%d", (int)(90 / points[i].second));

        angs.setString(tmp);

        target.draw(angs);
    }
    */
}