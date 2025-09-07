#ifndef GIF_RECORDER_HPP
#define GIF_RECORDER_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <filesystem>

class GIFRecorder {
private:
    std::vector<sf::Image> frames;
    bool isRecording;
    int maxFrames;
    int recordedFrames;
    float targetFPS;
    float frameInterval;
    float accumulatedTime;
    sf::RenderTexture renderTexture;

public:
    GIFRecorder(int width, int height, int maxFrames = 300, float fps = 30.0f) :
        isRecording(false), maxFrames(maxFrames), recordedFrames(0),
        targetFPS(fps), frameInterval(1.0f / fps), accumulatedTime(0.0f) {
        renderTexture.create(width, height);
    }

    void startRecording() {
        isRecording = true;
        recordedFrames = 0;
        accumulatedTime = 0.0f;
        frames.clear();
        std::cout << "Started recording at " << targetFPS << " FPS..." << std::endl;
    }

    void stopRecording() {
        isRecording = false;
        std::cout << "Stopped recording. Saving frames..." << std::endl;
        saveFrames();
    }

    bool isRecordingNow() const {
        return isRecording;
    }

    void update(float deltaTime, const sf::RenderWindow& window) {
        if (isRecording && recordedFrames < maxFrames) {
            accumulatedTime += deltaTime;

            if (accumulatedTime >= frameInterval) {
                captureFrame(window);
                accumulatedTime -= frameInterval;
            }
        }
    }

    void captureFrame(const sf::RenderWindow& window) {
        sf::Texture texture;
        texture.create(window.getSize().x, window.getSize().y);
        texture.update(window);
        frames.push_back(texture.copyToImage());
        recordedFrames++;

        if (recordedFrames >= maxFrames) {
            stopRecording();
        }
    }

    void saveFrames() const {
        std::filesystem::create_directory("frames");
        for (int i = 0; i < frames.size(); i++) {
            std::stringstream filename;
            filename << "frames/frame_" << std::setw(4) << std::setfill('0') << i << ".png";
            if (frames[i].saveToFile(filename.str())) {
                std::cout << "Saved " << filename.str() << std::endl;
            } else {
                std::cerr << "Failed to save " << filename.str() << std::endl;
            }
        }
        std::cout << "Saved " << frames.size() << " frames to the 'frames' directory." << std::endl;

        int delay = static_cast<int>(100.0f / targetFPS);
        std::cout << "Command: convert -delay " << delay << " -loop 0 frames/frame_*.png animation.gif" << std::endl;
    }

    void setFPS(float fps) {
        targetFPS = fps;
        frameInterval = 1.0f / fps;
    }

    float getFPS() const {
        return targetFPS;
    }

    int getRecordedFrames() const {
        return recordedFrames;
    }

    int getMaxFrames() const {
        return maxFrames;
    }
};

#endif // GIF_RECORDER_HPP
