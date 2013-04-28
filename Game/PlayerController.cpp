#include "PlayerController.hpp"
#include "Components.hpp"
#include "InputManager.hpp"
#include "Math.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <Kunlaboro/EntitySystem.hpp>
#include <iostream>

PlayerController::PlayerController(): Kunlaboro::Component("PlayerController"), mInput(NULL), mPhys(NULL), mInert(NULL), mView(NULL), mKills(0)
{
    mTexture.create(128, 128);
    const_cast<sf::Texture&>(mTexture.getTexture()).setSmooth(true);
}

PlayerController::~PlayerController()
{
}

void PlayerController::addedToEntity()
{
    requestMessage("LD26.Update", [this](const Kunlaboro::Message& msg) { update(boost::any_cast<float>(msg.payload)); });
    requestMessage("LD26.Draw",   [this](const Kunlaboro::Message& msg) { draw(*boost::any_cast<sf::RenderTarget*>(msg.payload)); });
    changeRequestPriority("LD26.Draw", 4);
    requestMessage("LD26.DrawUi", [this](const Kunlaboro::Message& msg)
    {
        sf::RenderTarget& target = *boost::any_cast<sf::RenderTarget*>(msg.payload);

        sf::Text kills;
        Kunlaboro::Message msg2 = sendGlobalQuestion("Get.Font");

        kills.setFont(*boost::any_cast<sf::Font*>(msg2.payload));
        kills.setCharacterSize(16);

        char text[4];
        sprintf(text, "%d", mKills);
        kills.setString("Pops:\n" + std::string(text));
        kills.setPosition((sf::Vector2f)target.getSize() - sf::Vector2f(kills.getLocalBounds().width + 8, target.getSize().y - 8));

        target.draw(kills);
    });

    requestMessage("Collision",   [this](const Kunlaboro::Message& msg)
    {
         Components::Physical* other = static_cast<Components::Physical*>(msg.sender);
         if (other == NULL)
             return;

         Kunlaboro::Message msg2 = sendQuestion("GetPoints");
         const std::vector<std::pair<sf::Vector2f, float> >& points = *boost::any_cast<const std::vector<std::pair<sf::Vector2f, float> >*>(msg2.payload);
         float ang = mPhys->getRot();
         sf::Vector2f pos = mPhys->getPos();
         sf::Vector2f X = sf::Vector2f(cos(ang), sin(ang));
         sf::Vector2f Y = sf::Vector2f(cos(ang + pi/2), sin(ang + pi/2));

         for (int i = 0; i < points.size(); ++i)
         {
             sf::Vector2f p = points[i].first;
             float prot = dot(pos+(X*p.x + Y*p.y), other->getPos());

             if (prot < other->getRadius()*other->getRadius())
             {
                 //getEntitySystem()->destroyEntity(other->getOwnerId());

                 sendMessageToEntity(other->getOwnerId(), "SetHealth", 0.f);
                 mKills++;

                 std::cerr << "TODO: Calculate damage" << std::endl;
             }
         }
    }, true);
    
    requireComponent("Components.MetaPhysical", [this](const Kunlaboro::Message& msg) { mMeta = static_cast<Components::MetaPhysical*>(msg.sender); mMeta->setMaxHealth(10); mMeta->setHealth(10); });
    requireComponent("Components.Physical", [this](const Kunlaboro::Message& msg) { mPhys = static_cast<Components::Physical*>(msg.sender); mPhys->setRadius(31); });
    requireComponent("Components.Inertia",  [this](const Kunlaboro::Message& msg) { mInert = static_cast<Components::Inertia*>(msg.sender); });
    requireComponent("Components.ShapeDrawable", [this](const Kunlaboro::Message& msg)
    {
        msg.sender->changeRequestPriority("LD26.Draw", 7);

        sf::ConvexShape* shape = new sf::ConvexShape(3);
        shape->setPoint(0, sf::Vector2f( 25,  0));
        shape->setPoint(1, sf::Vector2f(-25,  10));
        shape->setPoint(2, sf::Vector2f(-25, -10));
        //shape->setPoint(3, sf::Vector2f( 25,  0));

        shape->setFillColor(sf::Color::Transparent);
        shape->setOutlineColor(sf::Color::White);
        shape->setOutlineThickness(2.5f);

        sendMessage("SetShape", (sf::Shape*)shape);
        sendMessage("SetOrigin");
    });

    if (mView == NULL)
    {
        Kunlaboro::Message msg = sendGlobalQuestion("Get.GameView");
        if (msg.handled)
        {
            mView = boost::any_cast<sf::View*>(msg.payload);
        }
    }

    if (mInput == NULL)
    {
        Kunlaboro::Message msg = sendGlobalQuestion("Get.Input");
        if (msg.handled)
            mInput = boost::any_cast<InputManager*>(msg.payload);
    }
}

void PlayerController::update(float dt)
{
    if (mPhys == NULL || mView == NULL || mInert == NULL)
        return;

    float up    = mInput->getInput("Up"),
          down  = mInput->getInput("Down"),
          left  = mInput->getInput("Left"),
          right = mInput->getInput("Right");

    sf::Vector2f pos = mPhys->getPos();
    float curAng = mPhys->getRot() * deg2rad;

    sf::Vector2f targetVec(right-left, down-up);
    if (up < 0.2f && down < 0.2f && left < 0.2f && right < 0.2f)
    {
        targetVec.x = 0;
        targetVec.y = 0;
    }

    float targetDot = targetVec.x * targetVec.x + targetVec.y * targetVec.y;
    float len = sqrt(targetDot);
    len = std::min(len, 1.f);
    
    float targetAng = atan2(targetVec.y, targetVec.x);
    if (targetVec == sf::Vector2f()) 
        targetAng = curAng;

    float slerpedAng = slerp(curAng, targetAng, dt * 1);

    mPhys->setRot(slerpedAng * rad2deg);

    targetVec.x = cos(curAng);
    targetVec.y = sin(curAng);

    sf::Vector2f curSpeed = mInert->getSpeed();
    //sendGlobalMessage("Background.Parallax", -curSpeed);
    float curDot = curSpeed.x * curSpeed.x + curSpeed.y * curSpeed.y;
    targetDot = targetVec.x * targetVec.x + targetVec.y * targetVec.y;

    {
        float sLen = sqrt(curDot);
        sLen = sLen / 360;
        
        float targt = (sLen + dt * 1.5f * (len - sLen));

        mInert->setSpeed(targetVec * targt * 360.f);
    }

    if (pos.x < 32 || pos.x > 15968 || pos.y < 32 || pos.y > 15968)
    {
        sf::Vector2f sp = mInert->getSpeed();
        if ((pos.x < 32 && sp.x < 0) || (pos.x > 15968 && sp.x > 0))
            sp.x = 0;
        if ((pos.y < 32 && sp.y < 0) || (pos.y > 15968 && sp.y > 0))
            sp.y = 0;
        mInert->setSpeed(sp);
    }

    {
        mView->setCenter(pos + curSpeed / 4.f);

        sf::Vector2f size = mView->getSize();
        float aspect = size.x/size.y;

        mView->setSize(512 * aspect, 512);
    }
}

void PlayerController::draw(sf::RenderTarget& target)
{
    if (mPhys == NULL || mMeta == NULL)
        return;

    mTexture.clear(sf::Color::Transparent);

    sf::CircleShape circ(62, 8);
    circ.setPosition(1,1);
    circ.setOutlineColor(sf::Color(127,127,127));
    circ.setOutlineThickness(1.f);

    mTexture.draw(circ);
    
    circ.setRadius(30);
    circ.move(32,32);
    //circ.setOutlineThickness(0.f);
    circ.setFillColor(sf::Color(0,0,0,1));
    
    mTexture.draw(circ, sf::BlendNone);

    for (int i = 0; i < 2; ++i)
    {
        float perc = mMeta->getHealth() / mMeta->getMaxHealth();

        float base = 0;
        if (i)
        {
            base = perc;
            perc = 1-perc;
        }

        sf::Vector2f mid(64,64);

        int pieces = ((int)(perc * 100) / 6);

        sf::ConvexShape pie(pieces + 3);
        pie.setPoint(0, mid);

        sf::Vector2f scoords(cos((base*pi*2)-pi/2.f), sin((base*pi*2)-pi/2.f));
        pie.setPoint(1, mid + scoords * 96.f);

        for (int j = 0; j < pieces; ++j)
        {
            sf::Vector2f mcoords(cos((base*pi*2)-pi/2.f + ((perc/pieces) * j+1) * pi*2), sin((base*pi*2)-pi/2.f + ((perc/pieces) * j+1) * pi*2));

            pie.setPoint(2 + j, mid + mcoords * 96.f);
        }

        sf::Vector2f ecoords(cos((base*pi*2)-pi/2.f + perc * pi*2), sin((base*pi*2)-pi/2.f + perc * pi*2));
        pie.setPoint(2 + pieces, mid + ecoords * 96.f);

        if (i)
            pie.setFillColor(sf::Color(0,127,0));
        else
            pie.setFillColor(sf::Color::Green);
        mTexture.draw(pie, sf::BlendMultiply);
    }

    mTexture.display();

    sf::Sprite sprite(mTexture.getTexture());
    sprite.setPosition(mPhys->getPos());
    sprite.setOrigin(64,64);
    float scale = (mPhys->getRadius()*2) / mTexture.getSize().x;
    sprite.setScale(scale, scale);
    sprite.setColor(sf::Color(255,255,255,72));

    target.draw(sprite);

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