#include <Kunlaboro/Component.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <unordered_map>

#pragma once

namespace Components
{
 
class Physical : public Kunlaboro::Component
{
public:
    Physical();

    void addedToEntity();

    void setPos(sf::Vector2f pos) { mX = pos.x; mY = pos.y; }
    void setRot(float rot) { mRot = rot; }
    void setRadius(float rad) { mRadius = rad; }

    inline sf::Vector2f getPos() const { return sf::Vector2f(mX,mY); }
    float getRot() const { return mRot; }
    float getRadius() const { return mRadius; }

private:
    float mX, mY, mRot, mRadius;
};

class TexturedDrawable : public Kunlaboro::Component
{
public:
    TexturedDrawable();

    void addedToEntity();

    void setTexture(const std::string&);

private:
    Physical* mPhysical;
    sf::Texture mTex;
};

class ShapeDrawable : public Kunlaboro::Component
{
public:
    ShapeDrawable();
    ~ShapeDrawable();

    void addedToEntity();

    void setShape(sf::Shape*);
    sf::Shape* getShape() const;

private:
    Physical* mPhysical;
    sf::Shape* mShape;
};

class SpatialContainer : public Kunlaboro::Component
{
public:
    SpatialContainer();
    ~SpatialContainer();

    void addedToEntity();

    void addEntity(Kunlaboro::EntityId);

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
    sf::Vector2f getPosition(Kunlaboro::EntityId, bool store = false);
    bool contains(QuadTree*, sf::Vector2f);

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
