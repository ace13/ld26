#include <SFML/System/Clock.hpp>
#include <vector>

#pragma once

class Telemetry
{
public:
    Telemetry(int sampleCount = 5);
    ~Telemetry();

    void setSampleSize(int size);

    void startFrame();
    void endFrame();

    inline float getDT() { return mCurDT; }
    inline int getFPS() { return mCurFPS; }

    inline float getAverageDT() { return mCurAvDT; }
    inline float getAverageFPS() { return mCurAvFPS; }

private:
    void newSecond();

    void pushFPS(int fps);
    void pushDT(float dt);

    sf::Clock mClock;

    sf::Time mLastSecond;
    sf::Time mStart;

    int mFrames;

    int mCurFPS;
    float mCurDT;

    float mCurAvFPS;
    float mCurAvDT;

    std::vector<int> mFPS;
    std::vector<float> mDT;
};