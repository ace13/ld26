#include <Kunlaboro/EntitySystem.hpp>
#include <Kunlaboro/Component.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#pragma once

namespace sf { class RenderTarget; }
class InputManager;
class SettingsManager;

class IState;

class StateManager
{
public:
    StateManager();
    ~StateManager();

    void setViews(sf::View& game, sf::View& ui);
    void setInput(InputManager&);
    void setSettings(SettingsManager&);
    InputManager* getInput();
    SettingsManager* getSettings();

    void update(float);

    void draw(sf::RenderTarget&);
    void drawUi(sf::RenderTarget&);

    Kunlaboro::EntitySystem& getSystem();

private:
    class Internals : public Kunlaboro::Component
    {
    public:
        Internals(StateManager&);

        void addedToEntity();

        StateManager& mState;
    };

    Kunlaboro::EntitySystem mSystem;
    InputManager* mInput;
    SettingsManager* mSettings;

    sf::View* mGameView;
    sf::View* mUiView;

    Kunlaboro::RequestId mRUpdate, mRDraw, mRDrawUi;

    friend class Internals;
};

class IState : public Kunlaboro::Component
{
public:
    IState(const std::string&);

    void addedToEntity();

    virtual void setup() = 0;
    virtual void update(float dt) = 0;
    virtual void draw(sf::RenderTarget& target) = 0;
    virtual void drawUi(sf::RenderTarget& target) = 0;

    InputManager* getInput()      { Kunlaboro::Message msg = sendGlobalQuestion("Get.Input"); if (msg.handled) return boost::any_cast<InputManager*>(msg.payload); return NULL; }
    SettingsManager* getSetings() { Kunlaboro::Message msg = sendGlobalQuestion("Get.Settings"); if (msg.handled) return boost::any_cast<SettingsManager*>(msg.payload); return NULL; }

private:
    inline void update(const Kunlaboro::Message& msg) { update(boost::any_cast<float>(msg.payload));              }
    inline void draw(const Kunlaboro::Message& msg)   { draw  (*boost::any_cast<sf::RenderTarget*>(msg.payload)); }
    inline void drawUi(const Kunlaboro::Message& msg) { drawUi(*boost::any_cast<sf::RenderTarget*>(msg.payload)); }
};