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
    requestMessage("GetPos",    [this](Kunlaboro::Message& msg){ msg.payload = sf::Vector2f(mX, mY); msg.handled = true; }, true);
    requestMessage("GetRot",    [this](Kunlaboro::Message& msg){ msg.payload = mRot; msg.handled = true; }, true);
    requestMessage("GetRadius", [this](Kunlaboro::Message& msg){ msg.payload = mRadius; msg.handled = true; }, true);

    requestMessage("SetPos",    [this](const Kunlaboro::Message& msg){ sf::Vector2f pos = boost::any_cast<sf::Vector2f>(msg.payload); mX = pos.x; mY = pos.y; }, true);
    requestMessage("SetRot",    [this](const Kunlaboro::Message& msg){ mRot = boost::any_cast<float>(msg.payload); }, true);
    requestMessage("SetRadius", [this](const Kunlaboro::Message& msg){ mRadius = boost::any_cast<float>(msg.payload); }, true);
}

Drawable::Drawable() : Kunlaboro::Component("Components.Drawable")
{
}

void Drawable::addedToEntity()
{
    requestMessage("SetTexture", [this](const Kunlaboro::Message& msg){ mTex.loadFromFile(boost::any_cast<std::string>(msg.payload)); }, true);
    requestMessage("LD26.Draw",  [this](const Kunlaboro::Message& msg)
        {
            sf::RenderTarget& target = *boost::any_cast<sf::RenderTarget*>(msg.payload);

            sf::Sprite sprite(mTex);
            sprite.setOrigin((sf::Vector2f)mTex.getSize()/2.f);

            sprite.setPosition(boost::any_cast<sf::Vector2f>(sendQuestionToEntity(getOwnerId(), "GetPos").payload));
            sprite.setRotation(boost::any_cast<float>(sendQuestionToEntity(getOwnerId(), "GetRot").payload));

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

    if (contains(mNW, eid))
        mNW->addObject(eid);
    else if (contains(mNE, eid))
        mNE->addObject(eid);
    else if (contains(mSW, eid))
        mSW->addObject(eid);
    else if (contains(mSE, eid))
        mSE->addObject(eid);
    else if (contains(this, eid))
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
            for (auto it = mContained.begin(), end = mContained.end(); it++ != end;)
                if (!contains(this, it->first))
                {
                    mParent->addObject(it->first);
                    mContained.erase(it);
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
    rect.setFillColor(sf::Color::Transparent);
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

bool QuadTree::contains(QuadTree* qt, Kunlaboro::EntityId eid)
{
    Kunlaboro::Message message(Kunlaboro::Type_Message, &mContainer);
    Kunlaboro::EntitySystem& system = *mContainer.getEntitySystem();
    auto& it = qt->mContained[eid];

    sf::Vector2f pos;

    if (it == NULL)
    {
        system.sendLocalMessage(eid, system.getMessageRequestId(Kunlaboro::Reason_Message, "GetPos"), message);
        if (message.handled)
        {
            it = static_cast<Physical*>(message.sender);
            pos = boost::any_cast<sf::Vector2f>(message.payload);
        }
        else
        {
            qt->mContained.erase(eid);
            return false;
        }   
    }
    else
        pos = it->getPos();

    return qt->mBounds.contains(pos);
}