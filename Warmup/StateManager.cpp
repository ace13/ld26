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
    mSystem.sendGlobalMessage(mRUpdate, Kunlaboro::Message(Kunlaboro::Type_Message, NULL, dt));
}
void StateManager::draw(sf::RenderTarget& target)
{
    mSystem.sendGlobalMessage(mRDraw, Kunlaboro::Message(Kunlaboro::Type_Message, NULL, &target));
}
void StateManager::drawUi(sf::RenderTarget& target)
{
    mSystem.sendGlobalMessage(mRDrawUi, Kunlaboro::Message(Kunlaboro::Type_Message, NULL, &target));
}

IState::IState(const std::string& name) : Kunlaboro::Component(name)
{
}

void IState::addedToEntity()
{
    requestMessage("LD26.Update", &IState::update);
    requestMessage("LD26.Draw",   &IState::draw);
    requestMessage("LD26.DrawUi", &IState::drawUi);
}

StateManager::Internals::Internals(StateManager& man) : Kunlaboro::Component("StateManager.Internals"), mState(man) { }
void StateManager::Internals::addedToEntity()
{
    StateManager* man = &mState;

    requestMessage("Get.Input",    [man](Kunlaboro::Message& msg){ msg.payload = man->mInput; msg.handled = true;    });
    requestMessage("Get.Settings", [man](Kunlaboro::Message& msg){ msg.payload = man->mSettings; msg.handled = true; });
}