#include "SoundManager.hpp"
#include <SFML/Audio/Listener.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/Music.hpp>

SoundManager::SoundManager(SettingsManager& s) : mSettings(s), mUpdated(0)
{
}

SoundManager::~SoundManager()
{
    for (auto it = mPlaying.begin(); it != mPlaying.end(); mPlaying.erase(it++))
        delete *it;
    for (auto it = mMusic.begin(); it != mMusic.end(); mMusic.erase(it++))
        delete it->second;
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

    snd->setVolume(mSettings.getFloat("Sound Volume"));

    snd->play();
}

void SoundManager::addMusic(const std::string& file, const std::string& n)
{
    std::string name = n;
    if (name.empty())
        name = file;

    mMusic[name] = new sf::Music();
    sf::Music* m = mMusic[name];

    if (!m->openFromFile(file))
    {
        mMusic.erase(name);
        return;
    }
}

void SoundManager::registerMusicCallback(const std::function<void(const std::string&)>& callback)
{
    mMusicCallback = callback;
}

void SoundManager::startMusic()
{
    if (!mCurrentSong.empty() && mMusic[mCurrentSong]->getStatus() != sf::Music::Stopped)
        mMusic[mCurrentSong]->stop();

    auto it = mMusic.begin();

    do
    {
        int id = rand()%mMusic.size(), i = 0;

        for (; it != mMusic.end() && i < id; ++it, ++i)
            ;
    
        if (it == mMusic.end())
            return;
    }
    while (it->first == mCurrentSong);

    mCurrentSong = it->first;
    it->second->play();
    it->second->setVolume(mSettings.getFloat("Music Volume"));

    if (mMusicCallback)
        mMusicCallback(mCurrentSong);
}
void SoundManager::stopMusic()
{
    mMusic[mCurrentSong]->stop();
    mCurrentSong = "";
}

void SoundManager::update(float dt)
{
    if (!mCurrentSong.empty() && mMusic[mCurrentSong]->getStatus() == sf::Music::Stopped)
        startMusic();

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