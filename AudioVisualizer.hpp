#ifndef AUDIO_VISUALIZER_HPP
#define AUDIO_VISUALIZER_HPP

#include <SFML/Audio.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <random>

class MusicAnalyzer {
private:
    sf::Music music;
    std::vector<float> samples;
    std::vector<float> spectrum;
    float volume;
    float bass;
    float mid;
    float treble;
    bool useRealMusic;
    float simulatedTime;
    sf::Time previousOffset;
    
public:
    MusicAnalyzer() : volume(0), bass(0), mid(0), treble(0), useRealMusic(false), simulatedTime(0) {
        samples.resize(1024);
        spectrum.resize(512);
    }
    
    bool loadMusic(const std::string& filename) {
        if (!music.openFromFile(filename)) {
            std::cerr << "Error: Could not load music file: " << filename << std::endl;
            return false;
        }
        useRealMusic = true;
        return true;
    }
    
    void play() {
        if (useRealMusic) {
            music.play();
        }
    }
    
    void stop() {
        if (useRealMusic) {
            music.stop();
        }
    }
    
    void update(float deltaTime = 0) {
        if (useRealMusic && music.getStatus() == sf::Music::Playing) {
            // Get current playing offset
            sf::Time currentOffset = music.getPlayingOffset();
            
            // Calculate a simple volume based on time progression
            // This is a simulation since we can't easily access raw samples
            float time = currentOffset.asSeconds();
            volume = 0.5f + 0.4f * std::sin(time * 2.0f);
            bass = 0.6f + 0.3f * std::sin(time * 1.0f);
            mid = 0.5f + 0.4f * std::sin(time * 3.0f);
            treble = 0.4f + 0.5f * std::sin(time * 5.0f);
            
            // Add some randomness to make it more interesting
            static std::default_random_engine generator;
            static std::uniform_real_distribution<float> distribution(0.8f, 1.2f);
            
            volume *= distribution(generator);
            bass *= distribution(generator);
            mid *= distribution(generator);
            treble *= distribution(generator);
            
            // Clamp values
            volume = std::min(1.0f, std::max(0.0f, volume));
            bass = std::min(1.0f, std::max(0.0f, bass));
            mid = std::min(1.0f, std::max(0.0f, mid));
            treble = std::min(1.0f, std::max(0.0f, treble));
            
            previousOffset = currentOffset;
        } else {
            // Simulate music analysis for demonstration
            simulatedTime += deltaTime;
            volume = 0.5f + 0.4f * std::sin(simulatedTime * 0.5f);
            bass = 0.6f + 0.3f * std::sin(simulatedTime * 1.0f);
            mid = 0.5f + 0.4f * std::sin(simulatedTime * 2.0f);
            treble = 0.4f + 0.5f * std::sin(simulatedTime * 3.0f);
        }
    }
    
    float getVolume() const { return volume; }
    float getBass() const { return bass; }
    float getMid() const { return mid; }
    float getTreble() const { return treble; }
    const std::vector<float>& getSamples() const { return samples; }
    const std::vector<float>& getSpectrum() const { return spectrum; }
    
    sf::Time getDuration() const { return music.getDuration(); }
    sf::Time getPlayingOffset() const { return music.getPlayingOffset(); }
    sf::SoundSource::Status getStatus() const { return music.getStatus(); }
    bool isUsingRealMusic() const { return useRealMusic; }
};

class RuleBasedPlacer {
private:
    int windowWidth, windowHeight;
    
public:
    RuleBasedPlacer(int width, int height) : windowWidth(width), windowHeight(height) {}
    
    // Different placement rules
    sf::Vector2f circularPlacement(float angle, float radius, float beat) {
        float x = windowWidth / 2 + radius * std::cos(angle) * (0.9f + 0.1f * beat);
        float y = windowHeight / 2 + radius * std::sin(angle) * (0.9f + 0.1f * beat);
        return sf::Vector2f(x, y);
    }
    
    sf::Vector2f gridPlacement(int xIndex, int yIndex, int totalX, int totalY, float pulse) {
        float spacingX = windowWidth / static_cast<float>(totalX);
        float spacingY = windowHeight / static_cast<float>(totalY);
        float x = xIndex * spacingX + spacingX / 2;
        float y = yIndex * spacingY + spacingY / 2;
        
        // Add some movement based on music
        x += 5.0f * pulse * std::sin(yIndex * 0.5f);
        y += 5.0f * pulse * std::cos(xIndex * 0.5f);
        
        return sf::Vector2f(x, y);
    }
    
    sf::Vector2f wavePlacement(float t, float frequency, float amplitude, float phase) {
        float x = windowWidth * t;
        float y = windowHeight / 2 + amplitude * std::sin(t * frequency * 2 * M_PI + phase);
        return sf::Vector2f(x, y);
    }
    
    sf::Vector2f randomPlacement(std::mt19937& gen, float chaos) {
        std::uniform_real_distribution<> distX(0, windowWidth);
        std::uniform_real_distribution<> distY(0, windowHeight);
        
        float x = distX(gen);
        float y = distY(gen);
        
        // Add some chaos based on music
        std::uniform_real_distribution<> distChaos(-chaos * 50, chaos * 50);
        x += distChaos(gen);
        y += distChaos(gen);
        
        return sf::Vector2f(x, y);
    }
    
    sf::Vector2f spiralPlacement(float angle, float radius, float growth, float time) {
        float r = radius * (1.0f + growth * time);
        float x = windowWidth / 2 + r * std::cos(angle);
        float y = windowHeight / 2 + r * std::sin(angle);
        return sf::Vector2f(x, y);
    }
};

#endif // AUDIO_VISUALIZER_HPP
