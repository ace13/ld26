#include "Components.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <Kunlaboro/EntitySystem.hpp>

using namespace Components;

Physical::Physical() : Kunlaboro::Component("Components.Physical")
{
}

void Physical::addedToEntity()
{
    requestMessage("GetPos",    [this](Kunlaboro::Message& msg){ msg.payload = getPos(); msg.handled = true; }, true);
    requestMessage("GetRot",    [this](Kunlaboro::Message& msg){ msg.payload = getRot(); msg.handled = true; }, true);
    requestMessage("GetRadius", [this](Kunlaboro::Message& msg){ msg.payload = getRadius(); msg.handled = true; }, true);

    requestMessage("SetPos",    [this](const Kunlaboro::Message& msg){ setPos(boost::any_cast<sf::Vector2f>(msg.payload)); }, true);
    requestMessage("SetRot",    [this](const Kunlaboro::Message& msg){ setRot(boost::any_cast<float>(msg.payload)); }, true);
    requestMessage("SetRadius", [this](const Kunlaboro::Message& msg){ setRadius(boost::any_cast<float>(msg.payload)); }, true);
}

Drawable::Drawable() : Kunlaboro::Component("Components.Drawable")
{
}

void Drawable::addedToEntity()
{
    requireComponent("Components.Physical", [this](const Kunlaboro::Message& msg){ mPhysical = static_cast<Physical*>(msg.sender); });

    requestMessage("SetTexture", [this](const Kunlaboro::Message& msg){ mTex.loadFromFile(boost::any_cast<std::string>(msg.payload)); }, true);
    requestMessage("LD26.Draw",  [this](const Kunlaboro::Message& msg)
        {
            sf::RenderTarget& target = *boost::any_cast<sf::RenderTarget*>(msg.payload);

            sf::Sprite sprite(mTex);
            sprite.setOrigin((sf::Vector2f)mTex.getSize()/2.f);

            sprite.setPosition(mPhysical->getPos());
            sprite.setRotation(mPhysical->getRot());

            target.draw(sprite);
        });
}

SpatialContainer::SpatialContainer() : Kunlaboro::Component("Components.SpatialContainer"), mImpl(NULL)
{
}

SpatialContainer::~SpatialContainer()
{
    if (mImpl != NULL)
        delete mImpl;
}

void SpatialContainer::addedToEntity()
{
    requestMessage("LD26.Update", [this](const Kunlaboro::Message& msg){ if (mImpl == NULL) return; mImpl->update(boost::any_cast<float>(msg.payload)); });
    requestMessage("LD26.Draw", [this](const Kunlaboro::Message& msg){ if (mImpl == NULL) return; mImpl->draw(*boost::any_cast<sf::RenderTarget*>(msg.payload)); });

    requestMessage("StoreMe", [this](Kunlaboro::Message& msg){ if (mImpl == NULL) return; mImpl->addObject(msg.sender->getOwnerId()); msg.handled = true; }, true);
    requestMessage("GetObjects", [this](Kunlaboro::Message& msg) { if (mImpl == NULL) return; msg.payload = mImpl->getObjectsAt(boost::any_cast<sf::Vector2f>(msg.payload)); msg.handled = true; });
}

void SpatialContainer::setImpl(Impl* impl)
{
    if (mImpl != NULL)
        delete mImpl;

    mImpl = impl;
}

void SpatialContainer::addEntity(Kunlaboro::EntityId eid)
{
    if (mImpl != NULL)
        mImpl->addObject(eid);
}

QuadTree::QuadTree(SpatialContainer& cont, sf::FloatRect bounds, int level, int max, QuadTree* qt):
    mParent(qt), mContainer(cont), mBounds(bounds), mUpdate(0), mLevel(level), mMaxLevel(max),
    mNE(NULL), mSE(NULL), mSW(NULL), mNW(NULL)
{
    if (level == max)
        return;

    float x = mBounds.left, y = mBounds.top, w = mBounds.width, h = mBounds.height;

    mNW = new QuadTree(cont, sf::FloatRect(x, y, w/2.f, h/2.f), mLevel+1, mMaxLevel, this);
    mNE = new QuadTree(cont, sf::FloatRect(x + w/2.f, y, w/2.f, h/2.f), mLevel+1, mMaxLevel, this);
    mSW = new QuadTree(cont, sf::FloatRect(x, y + h/2.f, w/2.f, h/2.f), mLevel+1, mMaxLevel, this);
    mSE = new QuadTree(cont, sf::FloatRect(x + w/2.f, y + h/2.f, w/2.f, h/2.f), mLevel+1, mMaxLevel, this);
}
QuadTree::~QuadTree()
{
    if (mLevel == mMaxLevel)
        return;

    delete mNW;
    delete mNE;
    delete mSW;
    delete mSE;
}

void QuadTree::setBounds(sf::FloatRect bounds)
{
    mBounds = bounds;

    if (mLevel == mMaxLevel)
        return;

    float x = mBounds.left, y = mBounds.top, w = mBounds.width, h = mBounds.height;

    mNW->setBounds(sf::FloatRect(x, y, w/2.f, h/2.f));
    mNE->setBounds(sf::FloatRect(x + w/2.f, y, w/2.f, h/2.f));
    mSW->setBounds(sf::FloatRect(x, y + h/2.f, w/2.f, h/2.f));
    mSE->setBounds(sf::FloatRect(x + w/2.f, y + h/2.f, w/2.f, h/2.f));
}
sf::FloatRect QuadTree::getBounds()
{
    return mBounds;
}

void QuadTree::addObject(Kunlaboro::EntityId eid)
{
    if (mLevel == mMaxLevel)
    {
        mContained[eid] = NULL;
        return;
    }

    sf::Vector2f pos = getPosition(eid);

    if (contains(mNW, pos))
        mNW->addObject(eid);
    else if (contains(mNE, pos))
        mNE->addObject(eid);
    else if (contains(mSW, pos))
        mSW->addObject(eid);
    else if (contains(mSE, pos))
        mSE->addObject(eid);
    else if (contains(this, pos))
        mContained[eid] = NULL;
    else if (mParent != NULL)
        mParent->addObject(eid);
}
std::vector<Kunlaboro::EntityId> QuadTree::getObjectsAt(sf::Vector2f pos)
{
    std::vector<Kunlaboro::EntityId> tmp;

    if (mLevel == mMaxLevel || !mContained.empty())
    {
        for (auto it = mContained.begin(), end = mContained.end(); it != end; ++it)
            tmp.push_back(it->first);
    }
    
    if (mLevel != mMaxLevel)
    {
        std::vector<Kunlaboro::EntityId> child;
        if (pos.x > mBounds.left + mBounds.width / 2.f && pos.x < mBounds.left + mBounds.width)
        {
            if (pos.y > mBounds.top + mBounds.height / 2.f && pos.y < mBounds.top + mBounds.height)
            {
                child = mSE->getObjectsAt(pos);
            }
            else if (pos.y > mBounds.top && pos.y <= mBounds.top + mBounds.height / 2.f)
            {
                child = mNE->getObjectsAt(pos);
            }
        }
        else if (pos.x > mBounds.left && pos.x <= mBounds.left + mBounds.width / 2.f)
        {
            if (pos.y > mBounds.top + mBounds.height / 2.f && pos.y < mBounds.top + mBounds.height)
            {
                child = mSW->getObjectsAt(pos);
            }
            else if (pos.y > mBounds.top && pos.y <= mBounds.top + mBounds.height / 2.f)
            {
                child = mNW->getObjectsAt(pos);
            }
        }

        if (!child.empty())
            tmp.insert(tmp.end(), child.begin(), child.end());
    }

    return tmp;
}
void QuadTree::clear()
{
    if (mLevel == mMaxLevel || !mContained.empty())
    {
        mContained.clear();
    }
    
    if (mLevel != mMaxLevel)
    {
        mNW->clear();
        mNE->clear();
        mSW->clear();
        mSE->clear();
    }
}

void QuadTree::update(float dt)
{
    mUpdate += dt;

    if (mUpdate > 0.1f)
    {
        mUpdate = 0;

        if (!mContained.empty())
            for (auto it = mContained.begin(); it != mContained.end();)
            {
                if (!contains(this, getPosition(it->first)))
                {
                    mParent->addObject(it->first);

                    if (mContained.count(it->first) > 0)
                        mContained.erase(it++);
                    else
                        ++it;
                }
                else
                    ++it;
            }
    }

    if (mLevel != mMaxLevel)
    {
        mNW->update(dt);
        mNE->update(dt);
        mSE->update(dt);
        mSW->update(dt);
    }
}

void QuadTree::draw(sf::RenderTarget& target)
{
    sf::RectangleShape rect;
    rect.setPosition(mBounds.left, mBounds.top);
    rect.setSize(sf::Vector2f(mBounds.width, mBounds.height));
    if (mContained.empty())
        rect.setFillColor(sf::Color::Transparent);
    else
        rect.setFillColor(sf::Color::Green);
    rect.setOutlineColor(sf::Color::White);
    rect.setOutlineThickness(1.f);

    target.draw(rect);

    if (mLevel != mMaxLevel)
    {
        mNW->draw(target);
        mNE->draw(target);
        mSW->draw(target);
        mSE->draw(target);
    }
}

sf::Vector2f QuadTree::getPosition(Kunlaboro::EntityId eid, bool store)
{
    Kunlaboro::Message message(Kunlaboro::Type_Message, &mContainer);
    Kunlaboro::EntitySystem& system = *mContainer.getEntitySystem();
    
    sf::Vector2f pos;
    if (store)
    {
        auto& it = mContained[eid];
        if (it == NULL)
        {
            system.sendLocalMessage(eid, system.getMessageRequestId(Kunlaboro::Reason_Message, "GetPos"), message);
            if (message.handled)
            {
                if (store)
                    it = static_cast<Physical*>(message.sender);
                pos = boost::any_cast<sf::Vector2f>(message.payload);
            }
            else if (store)
            {
                mContained.erase(eid);
            }   
        }
        else
        pos = it->getPos();
    }
    else
    {
        system.sendLocalMessage(eid, system.getMessageRequestId(Kunlaboro::Reason_Message, "GetPos"), message);
        if (message.handled)
        {
            pos = boost::any_cast<sf::Vector2f>(message.payload);
        }
    }

    return pos;
}

bool QuadTree::contains(QuadTree* qt, sf::Vector2f pos)
{
    return qt->mBounds.contains(pos);
}