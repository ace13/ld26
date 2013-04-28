#include "MainClass.hpp"
#include "Components.hpp"
#include "PlayerController.hpp"
#include "EnemyController.hpp"

#include "GameState.hpp"
#include "MenuState.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <iostream>
#include <list>

static const int MAX_BINDS = 4;

MainClass::MainClass(int argc, char** argv) : mSounds(mSettings)
{
    std::list<std::string> stack;
    for (int i = 1; i < argc; i++)
        stack.push_front(argv[i]);

    mSettings.handleArgs(stack);

    mManager.setInput(mInput);
    mManager.setSettings(mSettings);
    mManager.setSound(mSounds);
    mManager.setViews(mGame, mUi);
    mManager.setTelemetry(mTelem);

    mSounds.registerMusicCallback([this](const std::string& song)
    {
        std::cout << "Now Playing: " << song << std::endl;
        mManager.getSystem().sendGlobalMessage("Now Playing", song);
    });

    Kunlaboro::EntitySystem& sys = mManager.getSystem();

    sys.registerComponent<Components::MetaPhysical>("Components.MetaPhysical");
    sys.registerComponent<Components::Physical>("Components.Physical");
    sys.registerComponent<Components::Inertia>("Components.Inertia");
    sys.registerComponent<Components::ShapeDrawable>("Components.ShapeDrawable");
    sys.registerComponent<Components::TexturedDrawable>("Components.TexturedDrawable");
    sys.registerComponent<Components::SpatialContainer>("Components.SpatialContainer");
    sys.registerComponent<Components::SharpCorners>("Components.SharpCorners");
    sys.registerComponent<PlayerController>("PlayerController");
    sys.registerComponent<EnemyController>("EnemyController");
    sys.registerComponent<GameState>("GameState");

    // Basic binds
    {
        sf::Event ev;
        ev.type = sf::Event::KeyPressed;
        ev.key.alt = ev.key.control = ev.key.shift = false;
        ev.key.code = sf::Keyboard::W;

        mInput.addBind("Up", ev);

        ev.key.code = sf::Keyboard::A;

        mInput.addBind("Left", ev);

        ev.key.code = sf::Keyboard::S;

        mInput.addBind("Down", ev);

        ev.key.code = sf::Keyboard::D;

        mInput.addBind("Right", ev);
    }

    // Music
    {
        mSounds.addMusic("Fast_and_exultant-device1.ogg", "Fast and exultant");
        mSounds.addMusic("The_caustic_boy_and_grandpa_s_rebellious_giant_from_Melbourne-device1.ogg", "The caustic boy and grandpa's rebellious giant from Melbourne");
        mSounds.addMusic("The_dependent_doctor_in_the_shed-device1.ogg", "The dependent doctor in the shed");
        mSounds.addMusic("The_swaggering_cow-device1.ogg", "The swaggering cow");
    }
}

MainClass::~MainClass()
{

}

int MainClass::operator()()
{
    int currentBind = (mSettings.getBool("Bind") ? 0 : 10);

    sf::RenderWindow app;

    if (mSettings.getBool("Fullscreen"))
        app.create(sf::VideoMode::getDesktopMode(), "Ludumdare #26", sf::Style::None);
    else
        app.create(sf::VideoMode(mSettings.getInt("Width"), mSettings.getInt("Height")), "Ludumdare #26");

    app.setFramerateLimit(60);
    sf::Event ev;

    {
        Kunlaboro::EntitySystem& sys = mManager.getSystem();

        Kunlaboro::EntityId id = sys.createEntity();
        sys.addComponent(id, "GameState");
        sys.finalizeEntity(id);
    }

    mGame = app.getDefaultView();
    mUi = app.getDefaultView();
    {
        float aspect = (float)app.getSize().x/(float)app.getSize().y;
        mGame.setSize(1080*aspect,1080);

        sf::Vector2f tSize = (sf::Vector2f)app.getSize();
        mUi.setSize(tSize);
        mUi.setCenter(tSize/2.f);
    }

    mSounds.startMusic();

    while (app.isOpen())
    {
        mTelem.startFrame();
        float dt = mTelem.getDT();

        while (app.pollEvent(ev))
        {
            mInput.handleEvent(ev);

            if (ev.type == sf::Event::Closed)
            {
                app.close();
            }
            else if (ev.type == sf::Event::Resized)
            {
                float aspect = (float)ev.size.width/(float)ev.size.height;
                mGame.setSize(sf::Vector2f(1080*aspect, 1080));

                sf::Vector2f tSize = (sf::Vector2f)app.getSize();
                mUi.setSize(tSize);
                mUi.setCenter(tSize/2.f);
            }
        }

        if (currentBind <= MAX_BINDS && !mInput.isBinding())
        {
            if (currentBind < MAX_BINDS)
                std::cout << "Press a key/button/axis you want to bind to ";

            std::string bind;
            switch(currentBind)
            {
            case 0:
                bind = "Up"; break;
            case 1:
                bind = "Down"; break;
            case 2:
                bind = "Left"; break;
            case 3:
                bind = "Right"; break;
            }

            if (!bind.empty())
            {
                std::cout << bind << std::endl;

                mInput.startBind(bind);
            }

            currentBind++;
        }
        else if (currentBind > MAX_BINDS)
        {
            mManager.update(dt);
            mSounds.update(dt);

            app.clear();

            app.setView(mGame);
            mManager.draw(app);

            app.setView(mUi);
            mManager.drawUi(app);

            app.display();
        }

        mTelem.endFrame();
    }

    return 0;
}