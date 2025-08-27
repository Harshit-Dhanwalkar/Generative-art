#ifndef GIF_RECORDER_HPP
#define GIF_RECORDER_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <iomanip>
#include <sstream>
#include <iostream>

class GIFRecorder {
private:
    std::vector<sf::Image> frames;
    bool isRecording;
    int maxFrames;
    int recordedFrames;
    sf::RenderTexture renderTexture;
    
public:
    GIFRecorder(int width, int height, int maxFrames = 300) : 
        isRecording(false), maxFrames(maxFrames), recordedFrames(0) {
        renderTexture.create(width, height);
    }
    
    void startRecording() {
        isRecording = true;
        recordedFrames = 0;
        frames.clear();
        std::cout << "Started recording..." << std::endl;
    }
    
    void stopRecording() {
        isRecording = false;
        std::cout << "Stopped recording. Saving frames..." << std::endl;
        saveFrames();
    }
    
    bool isRecordingNow() const {
        return isRecording;
    }
    
    void captureFrame(const sf::RenderWindow& window) {
        if (isRecording && recordedFrames < maxFrames) {
            // Create a screenshot of the current window
            sf::Texture texture;
            texture.create(window.getSize().x, window.getSize().y);
            texture.update(window);
            frames.push_back(texture.copyToImage());
            recordedFrames++;
            
            if (recordedFrames >= maxFrames) {
                stopRecording();
            }
        }
    }
    
    void saveFrames() const {
        for (int i = 0; i < frames.size(); i++) {
            std::stringstream filename;
            filename << "frame_" << std::setw(4) << std::setfill('0') << i << ".png";
            if (frames[i].saveToFile(filename.str())) {
                std::cout << "Saved " << filename.str() << std::endl;
            } else {
                std::cerr << "Failed to save " << filename.str() << std::endl;
            }
        }
        std::cout << "Saved " << frames.size() << " frames. Use a tool like ImageMagick to create a GIF." << std::endl;
        std::cout << "Command: convert -delay 2 -loop 0 frame_*.png animation.gif" << std::endl;
    }
    
    int getRecordedFrames() const {
        return recordedFrames;
    }
    
    int getMaxFrames() const {
        return maxFrames;
    }
};

#endif // GIF_RECORDER_HPP
