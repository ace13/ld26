#include <Kunlaboro/Component.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <unordered_map>

#pragma once

class SoundManager
{
public:
    SoundManager();
    ~SoundManager();

    void setListener(const sf::Vector2f& pos, float dir);
    void playSound(const std::string& sound, const sf::Vector2f& pos = sf::Vector2f());
    void update(float dt);

private:
    float mUpdated;
    std::unordered_map<std::string, sf::SoundBuffer> mBuffers;
    std::list<sf::Sound*> mPlaying;
};