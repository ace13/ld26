#include "SettingsManager.hpp"
#include <Kunlaboro/Component.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <unordered_map>
#include <functional>

#pragma once

namespace sf { class Sound; class Music; } 

class SoundManager
{
public:
    SoundManager(SettingsManager&);
    ~SoundManager();

    void setListener(const sf::Vector2f& pos, float dir);
    void playSound(const std::string& sound, const sf::Vector2f& pos = sf::Vector2f());

    void addMusic(const std::string& file, const std::string& name = "");
    
    void registerMusicCallback(const std::function<void(const std::string&)>& callback);

    void startMusic();
    void stopMusic();

    void update(float dt);

private:
    SettingsManager& mSettings;

    float mUpdated;
    std::unordered_map<std::string, sf::SoundBuffer> mBuffers;
    std::string mCurrentSong;
    std::unordered_map<std::string, sf::Music*> mMusic;
    std::list<sf::Sound*> mPlaying;
    std::function<void(const std::string&)> mMusicCallback;
};