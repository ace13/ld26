#include "Telemetry.hpp"

Telemetry::Telemetry(int count): mFrames(0), mCurFPS(0), mCurDT(0), mCurAvFPS(0), mCurAvDT(0)
{
    mDT.resize(count, 0);
    mFPS.resize(count, 0);

    mLastSecond = mClock.getElapsedTime();
}

Telemetry::~Telemetry() 
{
}

void Telemetry::setSampleSize(int size)
{
    mDT.resize(size, 0);
    mFPS.resize(size, 0);
}

void Telemetry::startFrame()
{
    mStart = mClock.getElapsedTime();
}

void Telemetry::endFrame()
{
    sf::Time end = mClock.getElapsedTime();
    sf::Time frameLen = end-mStart;
    mLastSecond += frameLen;

    ++mFrames;

    pushDT(frameLen.asSeconds());

    if (mLastSecond.asSeconds() > 1)
    {
        mLastSecond = sf::milliseconds(0);

        pushFPS(mFrames);
        mFrames = 0;

        newSecond();
    }
}

void Telemetry::newSecond()
{
    float dt = 0;
    {
        float* DTs = &mDT[0];
        for (int i = 0; i < mDT.size(); ++i)
            dt += DTs[i];
        dt /= mDT.size();
    }

    float fps = 0;
    {
        int* FPSs = &mFPS[0];
        for (int i = 0; i < mFPS.size(); ++i)
            fps += FPSs[i];
        fps /= mFPS.size();
    }

    mCurAvDT = dt;
    mCurAvFPS = fps;
}

void Telemetry::pushFPS(int fps)
{
    int i = mFPS.size()-1;
    int* FPSs = &mFPS[0];
    while (i --> 0)
        FPSs[i+1] = FPSs[i];
    FPSs[0] = fps;

    mCurFPS = fps;
}
void Telemetry::pushDT(float dt)
{
    int i = mDT.size()-1;
    float* DTs = &mDT[0];
    while (i --> 0)
        DTs[i+1] = DTs[i];
    DTs[0] = dt;

    mCurDT = dt;
}