#include "StateManager.hpp"
#include "InputManager.hpp"
#include "SettingsManager.hpp"

StateManager::StateManager()
{
    mRUpdate = mSystem.getMessageRequestId(Kunlaboro::Reason_Message, "LD26.Update");
    mRDraw   = mSystem.getMessageRequestId(Kunlaboro::Reason_Message, "LD26.Draw"  );
    mRDrawUi = mSystem.getMessageRequestId(Kunlaboro::Reason_Message, "LD26.DrawUi");

    mSystem.registerComponent("StateManager.Internals", [this](){ return new StateManager::Internals(*this); });
    Kunlaboro::EntityId id = mSystem.createEntity();
    mSystem.addComponent(id, "StateManager.Internals");
}

StateManager::~StateManager()
{
}

void StateManager::setInput(InputManager& in)
{
    mInput = &in;
}
void StateManager::setSettings(SettingsManager& in)
{
    mSettings = &in;
}

Kunlaboro::EntitySystem& StateManager::getSystem() { return mSystem; }

void StateManager::update(float dt)
{
    Kunlaboro::Message msg(Kunlaboro::Type_Message, NULL, dt);
    mSystem.sendGlobalMessage(mRUpdate, msg);
}
void StateManager::draw(sf::RenderTarget& target)
{
    Kunlaboro::Message msg(Kunlaboro::Type_Message, NULL, &target);
    mSystem.sendGlobalMessage(mRDraw, msg);
}
void StateManager::drawUi(sf::RenderTarget& target)
{
    Kunlaboro::Message msg(Kunlaboro::Type_Message, NULL, &target);
    mSystem.sendGlobalMessage(mRDrawUi, msg);
}

IState::IState(const std::string& name) : Kunlaboro::Component(name)
{
}

void IState::addedToEntity()
{
    requestMessage("LD26.Update", &IState::update);
    requestMessage("LD26.Draw",   &IState::draw);
    requestMessage("LD26.DrawUi", &IState::drawUi);

    setup();
}

StateManager::Internals::Internals(StateManager& man) : Kunlaboro::Component("StateManager.Internals"), mState(man) { }
void StateManager::Internals::addedToEntity()
{
    StateManager* man = &mState;

    requestMessage("Get.Input",    [man](Kunlaboro::Message& msg){ msg.payload = man->mInput; msg.handled = true;    });
    requestMessage("Get.Settings", [man](Kunlaboro::Message& msg){ msg.payload = man->mSettings; msg.handled = true; });
}