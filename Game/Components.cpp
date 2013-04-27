#include "Components.hpp"
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <Kunlaboro/EntitySystem.hpp>

using namespace Components;

Physical::Physical() : Kunlaboro::Component("Components.Physical"), mX(0), mY(0), mRot(0), mRadius(0), mContainer(NULL)
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

    requestMessage("You're Stored", [this](const Kunlaboro::Message& msg) { setContainer(static_cast<SpatialContainer*>(msg.sender)); }, true);

    requestMessage("LD26.Update", [this](const Kunlaboro::Message& msg) {
        if (!hasContainer())
            return;

        std::vector<Kunlaboro::EntityId> ents = mContainer->getObjectsAt(getPos());
        for (int i = 0; i < ents.size(); ++i)
        {
            if (ents[i] == getOwnerId())
                continue;

            sf::Vector2f otherPos;
            float otherRadius;

            Kunlaboro::Message msg = sendQuestionToEntity(ents[i], "GetPos");
            if (!msg.handled)
                continue;

            otherPos = boost::any_cast<sf::Vector2f>(msg.payload);

            msg = sendQuestionToEntity(ents[i], "GetRadius");
            if (!msg.handled)
                continue;

            otherRadius = boost::any_cast<float>(msg.payload);

            sf::Vector2f diff = (getPos() - otherPos);
            float dist = diff.x * diff.x + diff.y * diff.y;
            float radiuses = mRadius*mRadius + otherRadius*otherRadius;

            if (dist < radiuses)
            {
                sendMessageToEntity(ents[i], "Collision");
            }
        }
    });
}

Inertia::Inertia() : Kunlaboro::Component("Components.Inertia"), mPhysical(NULL), mInertia(0,0), mRotSpeed(0)
{
}

void Inertia::addedToEntity()
{
    requireComponent("Components.Physical", [this](const Kunlaboro::Message& msg){ mPhysical = static_cast<Physical*>(msg.sender); });

    requestMessage("GetSpeed",    [this](Kunlaboro::Message& msg) {
        msg.payload = getSpeed(); msg.handled = true;
    }, true);
    requestMessage("SetSpeed",    [this](const Kunlaboro::Message& msg) {
        setSpeed(boost::any_cast<sf::Vector2f>(msg.payload));
    }, true);
    requestMessage("SetRotSpeed", [this](const Kunlaboro::Message& msg) {
        setRotSpeed(boost::any_cast<float>(msg.payload));
    }, true);

    requestMessage("LD26.Update", [this](const Kunlaboro::Message& msg) {
        float dt = boost::any_cast<float>(msg.payload);

        mPhysical->setPos(mPhysical->getPos() + mInertia * dt);
        mPhysical->setRot(mPhysical->getRot() + mRotSpeed * dt);
    });
}

TexturedDrawable::TexturedDrawable() : Kunlaboro::Component("Components.TexturedDrawable")
{
}

void TexturedDrawable::setTexture(const std::string& file)
{
    mTex.loadFromFile(file);
}

void TexturedDrawable::addedToEntity()
{
    requireComponent("Components.Physical", [this](const Kunlaboro::Message& msg){ mPhysical = static_cast<Physical*>(msg.sender); });

    requestMessage("SetTexture", [this](const Kunlaboro::Message& msg){ setTexture(boost::any_cast<std::string>(msg.payload)); }, true);
    requestMessage("LD26.Draw",  [this](const Kunlaboro::Message& msg)
        {
            if (mPhysical == NULL || mTex.getSize() == sf::Vector2u())
                return;

            sf::RenderTarget& target = *boost::any_cast<sf::RenderTarget*>(msg.payload);

            sf::Sprite sprite(mTex);
            sprite.setOrigin((sf::Vector2f)mTex.getSize()/2.f);

            sprite.setPosition(mPhysical->getPos());
            sprite.setRotation(mPhysical->getRot());

            target.draw(sprite);
        });
}

ShapeDrawable::ShapeDrawable() : Kunlaboro::Component("Components.ShapeDrawable"), mPhysical(NULL), mShape(NULL)
{
}

ShapeDrawable::~ShapeDrawable()
{
    if (mShape != NULL)
        delete mShape;
}

void ShapeDrawable::setShape(sf::Shape* shape)
{
    if (mShape != NULL)
        delete mShape;
    mShape = shape;
}

sf::Shape* ShapeDrawable::getShape() const
{
    return mShape;
}

void ShapeDrawable::addedToEntity()
{
    requireComponent("Components.Physical", [this](const Kunlaboro::Message& msg) {
        mPhysical = static_cast<Physical*>(msg.sender);
    });

    requestMessage("GetShape", [this](Kunlaboro::Message& msg){ msg.payload = getShape(); msg.handled = true; }, true);
    requestMessage("SetShape", [this](const Kunlaboro::Message& msg){
        setShape(boost::any_cast<sf::Shape*>(msg.payload));
    }, true);
    requestMessage("LD26.Draw",  [this](const Kunlaboro::Message& msg) {
            if (mShape == NULL || mPhysical == NULL)
                return;

            sf::RenderTarget& target = *boost::any_cast<sf::RenderTarget*>(msg.payload);

            sf::Vector2f size;
            {
                sf::FloatRect rect = mShape->getLocalBounds();
                size.x = rect.left + rect.width / 2.f;
                size.y = rect.top + rect.height / 2.f;
            }
            mShape->setOrigin(size);

            mShape->setPosition(mPhysical->getPos());
            mShape->setRotation(mPhysical->getRot());

            target.draw(*mShape);
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
    changeRequestPriority("LD26.Draw", -1);

    requestMessage("StoreMe", [this](Kunlaboro::Message& msg){ addEntity(msg.sender->getOwnerId()); msg.handled = true; }, true);
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
    {
        mImpl->addObject(eid);

        sendMessageToEntity(eid, "You're Stored");
    }
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

void QuadTree::setBounds(const sf::FloatRect& bounds)
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

void QuadTree::addObject(Kunlaboro::EntityId eid, Physical* inp)
{
    if (mLevel == mMaxLevel)
    {
        mContained[eid] = inp;
        return;
    }

    Physical* phys = inp;
    sf::Vector2f pos = getPosition(eid, &phys);

    if (contains(mNW, pos))
        mNW->addObject(eid, phys);
    else if (contains(mNE, pos))
        mNE->addObject(eid, phys);
    else if (contains(mSW, pos))
        mSW->addObject(eid, phys);
    else if (contains(mSE, pos))
        mSE->addObject(eid, phys);
    else if (contains(this, pos))
        mContained[eid] = phys;
    else if (mParent != NULL)
        mParent->addObject(eid, phys);
}
std::vector<Kunlaboro::EntityId> QuadTree::getObjectsAt(const sf::Vector2f& pos)
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
                    mParent->addObject(it->first, it->second);

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

sf::Vector2f QuadTree::getPosition(Kunlaboro::EntityId eid, Physical** store)
{
    Kunlaboro::Message message(Kunlaboro::Type_Message, &mContainer);
    Kunlaboro::EntitySystem& system = *mContainer.getEntitySystem();
    
    sf::Vector2f pos;
    if (store)
    {
        if (*store == NULL)
        {
            system.sendLocalMessage(eid, system.getMessageRequestId(Kunlaboro::Reason_Message, "GetPos"), message);
            if (message.handled)
            {
                *store = static_cast<Physical*>(message.sender);
                pos = boost::any_cast<sf::Vector2f>(message.payload);
            }
        }
        else
            pos = (*store)->getPos();
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