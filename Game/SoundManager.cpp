#include "SoundManager.hpp"
#include <SFML/Audio/Listener.hpp>
#include <SFML/Audio/Sound.hpp>

SoundManager::SoundManager()
{
}

SoundManager::~SoundManager()
{
}

void SoundManager::setListener(const sf::Vector2f& pos, float dir)
{
    sf::Listener::setPosition(pos.x, pos.y, 0);
    sf::Listener::setDirection(cos(dir), sin(dir), 0);
}

void SoundManager::playSound(const std::string& name, const sf::Vector2f& pos)
{
    bool loaded = mBuffers.count(name) > 0;
    sf::SoundBuffer& buf = mBuffers[name];
    if (!loaded)
        if (!buf.loadFromFile(name))
        {
            mBuffers.erase(name);
            return;
        }
    
    sf::Sound* snd = new sf::Sound();
    snd->setBuffer(buf);

    if (pos != sf::Vector2f())
    {
        snd->setRelativeToListener(true);
        snd->setPosition(pos.x, pos.y, 0);
    }

    snd->play();
}

void SoundManager::update(float dt)
{
    mUpdated += dt;

    if (mUpdated > 0.33f)
    {
        mUpdated = 0;

        for (auto it = mPlaying.begin(); it != mPlaying.end();)
        {
            if ((*it)->getStatus() == sf::Sound::Stopped)
            {
                delete *it;
                mPlaying.erase(it++);
            }
            else
                ++it;
        }
    }
}