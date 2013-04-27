#include <Kunlaboro/Component.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Shape.hpp>
#include <unordered_map>

#pragma once

namespace Components
{
 
class SpatialContainer;

class MetaPhysical : public Kunlaboro::Component
{
public:
    MetaPhysical();

    void addedToEntity();

    inline float getHealth() const { return mHealth; }
    inline float getMaxHealth() const { return mMaxHealth; }
    inline float getRegen() const { return mRegen; }
    inline void setHealth(float v) { mHealth = v; }
    inline void setMaxHealth(float v) { mMaxHealth = v; }
    inline void setRegen(float v) { mRegen = v; }

private:
    float mHealth;
    float mMaxHealth;
    float mRegen;
};

class Physical : public Kunlaboro::Component
{
public:
    Physical();

    void addedToEntity();

    inline void setPos(const sf::Vector2f& pos) { mX = pos.x; mY = pos.y; }
    inline void setRot(float rot) { mRot = rot; }
    inline void setRadius(float rad) { mRadius = rad; }

    inline sf::Vector2f getPos() const { return sf::Vector2f(mX,mY); }
    inline float getRot() const { return mRot; }
    inline float getRadius() const { return mRadius; }

    inline bool hasContainer() const { return mContainer > 0; }
    inline SpatialContainer* getContainer() const { return mContainer; }
    inline void setContainer(SpatialContainer* id) { mContainer = id; }

private:
    float mX, mY, mRot, mRadius;
    SpatialContainer* mContainer;
};

class Inertia : public Kunlaboro::Component
{
public:
    Inertia();

    void addedToEntity();

    inline sf::Vector2f getSpeed() const { return mInertia; }
    inline void setSpeed(const sf::Vector2f& in) { mInertia = in; }
    inline float getRotSpeed() const { return mRotSpeed; }
    inline void setRotSpeed(float s) { mRotSpeed = s; }

private:
    Physical* mPhysical;
    sf::Vector2f mInertia;
    float mRotSpeed;
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
    inline void setOrigin() { setOrigin(getCenter()); }
    void setOrigin(const sf::Vector2f&);

private:
    sf::Vector2f getCenter();

    Physical* mPhysical;
    sf::Shape* mShape;
};

class SharpCorners : public Kunlaboro::Component
{
public:
    SharpCorners();
    ~SharpCorners();

    void addedToEntity();

    const std::vector<std::pair<sf::Vector2f, float> >* getPoints() const;

private:
    ShapeDrawable* mShape;
    bool mModified;
    std::vector<std::pair<sf::Vector2f, float> > mPoints;
};

class SpatialContainer : public Kunlaboro::Component
{
public:
    SpatialContainer();
    ~SpatialContainer();

    void addedToEntity();

    void addEntity(Kunlaboro::EntityId);

    inline sf::FloatRect getBounds() const { if (mImpl) return mImpl->getBounds(); }
    inline void setBounds(const sf::FloatRect& r) { if (mImpl) mImpl->setBounds(r); }
    inline std::vector<Kunlaboro::EntityId> getObjectsAt(const sf::Vector2f& pos) { if (mImpl) return mImpl->getObjectsAt(pos); return std::vector<Kunlaboro::EntityId>(); }
    inline void clear() { if (mImpl) mImpl->clear(); }

    class Impl
    {
    public:
        virtual void draw(sf::RenderTarget&) = 0;
        virtual void update(float dt) = 0;

        virtual void setBounds(const sf::FloatRect&) = 0;
        virtual sf::FloatRect getBounds() = 0;

        virtual void addObject(Kunlaboro::EntityId) = 0;
        virtual std::vector<Kunlaboro::EntityId> getObjectsAt(const sf::Vector2f& pos) = 0;
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

    virtual void setBounds(const sf::FloatRect&);
    virtual sf::FloatRect getBounds();

    virtual void update(float dt);

    virtual void addObject(Kunlaboro::EntityId eid) { addObject(eid, NULL); }
    virtual std::vector<Kunlaboro::EntityId> getObjectsAt(const sf::Vector2f& pos);
    virtual void clear();

    virtual void draw(sf::RenderTarget&);

private:
    void addObject(Kunlaboro::EntityId, Physical* p);
    sf::Vector2f getPosition(Kunlaboro::EntityId, Physical** p = NULL);
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
