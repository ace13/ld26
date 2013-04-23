#include <Kunlaboro/Component.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <unordered_map>

#pragma once

namespace Components
{
 
class Physical : public Kunlaboro::Component
{
public:
    Physical();

    void addedToEntity();

    inline sf::Vector2f getPos() const { return sf::Vector2f(mX,mY); }
    float getRot() const { return mRot; }
    float getRadius() const { return mRadius; }

private:
    float mX, mY, mRot, mRadius;
};

class Drawable : public Kunlaboro::Component
{
public:
    Drawable();

    void addedToEntity();

private:
    sf::Texture mTex;
};

class SpatialContainer : public Kunlaboro::Component
{
public:
    SpatialContainer();
    ~SpatialContainer();

    void addedToEntity();

    class Impl
    {
    public:
        virtual void draw(sf::RenderTarget&) = 0;
        virtual void update(float dt) = 0;

        virtual void setBounds(sf::FloatRect) = 0;
        virtual sf::FloatRect getBounds() = 0;

        virtual void addObject(Kunlaboro::EntityId) = 0;
        virtual std::vector<Kunlaboro::EntityId> getObjectsAt(sf::Vector2f pos) = 0;
        virtual void clear() = 0;
    };

    void setImpl(Impl*);

private:
    Impl* mImpl;
};

class QuadTree : public SpatialContainer::Impl
{
public:
    QuadTree(SpatialContainer&, sf::FloatRect bounds, int level, int max, QuadTree* parent = NULL);
    ~QuadTree();

    virtual void setBounds(sf::FloatRect);
    virtual sf::FloatRect getBounds();

    virtual void update(float dt);

    virtual void addObject(Kunlaboro::EntityId);
    virtual std::vector<Kunlaboro::EntityId> getObjectsAt(sf::Vector2f pos);
    virtual void clear();

    virtual void draw(sf::RenderTarget&);

private:
    bool contains(QuadTree*, Kunlaboro::EntityId);

    QuadTree* mParent;
    SpatialContainer& mContainer;
    sf::FloatRect mBounds;

    float mUpdate;
    int mLevel;
    int mMaxLevel;

    QuadTree* mNE;
    QuadTree* mSE;
    QuadTree* mSW;
    QuadTree* mNW;

    std::unordered_map<Kunlaboro::EntityId, Physical*> mContained;
};

}
