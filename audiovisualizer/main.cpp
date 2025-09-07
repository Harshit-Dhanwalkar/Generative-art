#include <SFML/Graphics.hpp>
#include <random>
#include <vector>
#include <iostream>
#include <map>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <filesystem>
#include "../lib/Palettes.hpp"
#include "../lib/GIFRecorder.hpp"
#include "../lib/AudioVisualizer.hpp"

// Print a color block to the terminal
void printColorBlock(const sf::Color& color) {
    std::cout << "\033[48;2;" << (int)color.r << ";" << (int)color.g << ";" << (int)color.b << "m    \033[0m";
}

const std::vector<sf::Color>& printAllPalettesAndGetChoice() {
    std::map<int, std::string> paletteOptions;
    int optionNumber = 1;

    // palettes menu
    static const std::map<std::string, std::vector<sf::Color>> allPalettes = {
        {"default", getPalette("default")},
        {"vibrant", getPalette("vibrant")},
        {"pastel", getPalette("pastel")},
        {"earthy", getPalette("earthy")},
        {"neon", getPalette("neon")},
        {"monochrome", getPalette("monochrome")}
    };

    std::cout << "Available palettes:\n";
    for (const auto& pair : allPalettes) {
        std::cout << optionNumber << ". " << pair.first << ": ";
        for (const auto& color : pair.second) {
            printColorBlock(color);
        }
        std::cout << std::endl;
        paletteOptions[optionNumber] = pair.first;
        optionNumber++;
    }

    int choice;
    std::cout << "Enter the number of the palette you want to use: ";
    std::cin >> choice;

    if (paletteOptions.count(choice)) {
        return allPalettes.at(paletteOptions.at(choice));
    } else {
        std::cerr << "Invalid choice. Using default palette." << std::endl;
        return allPalettes.at("default");
    }
}

int main() {
    const std::vector<sf::Color>& palette = printAllPalettesAndGetChoice();

    // Window dimensions
    const int windowWidth = 1000;
    const int windowHeight = 800;

    // Create the main window
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Music Responsive Generative Art", sf::Style::Close);

    window.setFramerateLimit(10);

    // Initialize music analyzer
    MusicAnalyzer analyzer;
    std::string musicFile;
    std::cout << "Enter music filename (or press enter for simulated music): ";
    std::cin.ignore();
    std::getline(std::cin, musicFile);

    bool useRealMusic = false;
    if (!musicFile.empty()) {
        useRealMusic = analyzer.loadMusic(musicFile);
        if (useRealMusic) {
            analyzer.play();
            std::cout << "Playing music file: " << musicFile << std::endl;
        }
    }

    if (!useRealMusic) {
        std::cout << "Using simulated music analysis." << std::endl;
    }

    // Create rule-based placer
    RuleBasedPlacer placer(windowWidth, windowHeight);
    GIFRecorder gifRecorder(windowWidth, windowHeight, 300, 15.0f);

    // Random number generation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> sizeDist(5.0, 100.0);
    std::uniform_int_distribution<> alphaDist(50, 200);
    std::uniform_real_distribution<> rotationDist(0.0, 360.0);
    std::uniform_int_distribution<> paletteIndexDist(0, palette.size() - 1);
    std::uniform_real_distribution<> floatDist(0.0, 1.0);

    // Render texture for saving frames
    sf::RenderTexture renderTexture;
    if (!renderTexture.create(windowWidth, windowHeight)) {
        return -1;
    }

    sf::Clock clock;
    sf::Time deltaTime;
    float time = 0;
    int frameCount = 0;

    // Initial shapes
    const int numberOfShapes = 500;
    std::vector<sf::RectangleShape> rects;
    std::vector<sf::CircleShape> circles;
    std::vector<int> shapeTypes;

    // Store original sizes to prevent continuous growth
    std::vector<sf::Vector2f> originalRectSizes;
    std::vector<float> originalCircleRadii;

    // Initialize shapes with different rules
    for (int i = 0; i < numberOfShapes; ++i) {
        int shapeType = i % 3;

        if (shapeType == 0 || shapeType == 2) {
            sf::Vector2f originalSize(sizeDist(gen), sizeDist(gen));
            sf::RectangleShape rect(originalSize);
            rects.push_back(rect);
            originalRectSizes.push_back(originalSize);
            shapeTypes.push_back(0); // 0 for rectangle
        } else {
            float originalRadius = sizeDist(gen) / 2;
            sf::CircleShape circle(originalRadius);
            circles.push_back(circle);
            originalCircleRadii.push_back(originalRadius);
            shapeTypes.push_back(1); // 1 for circle
        }
    }

    // Load font for UI
    sf::Font font;
    if (!font.loadFromFile("fonts/montana-light.ttf")) {
        std::cerr << "Warning: Could not load font. UI text will not be displayed." << std::endl;
    }

    // Main loop
    while (window.isOpen()) {
        deltaTime = clock.restart();
        float deltaSeconds = deltaTime.asSeconds();
        time += deltaSeconds;

        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Q) {
                    window.close();
                } else if (event.key.code == sf::Keyboard::R) {
                    if (gifRecorder.isRecordingNow()) {
                        gifRecorder.stopRecording();
                    } else {
                        gifRecorder.startRecording();
                    }
                } else if (event.key.code == sf::Keyboard::S) {
                    std::filesystem::create_directory("image");
                    renderTexture.clear(sf::Color::Transparent);
                    for (const auto& shape : rects) {
                        renderTexture.draw(shape);
                    }
                    for (const auto& shape : circles) {
                        renderTexture.draw(shape);
                    }
                    renderTexture.display();
                    renderTexture.getTexture().copyToImage().saveToFile("image/generated_art.png");
                    std::cout << "Saved image to image/generated_art.png" << std::endl;
                }
                // else if (event.key.code == sf::Keyboard::Num1) {
                //     gifRecorder.setFPS(10.0f);
                //     std::cout << "GIF FPS set to 10" << std::endl;
                // }
                // else if (event.key.code == sf::Keyboard::Num2) {
                //     gifRecorder.setFPS(15.0f);
                //     std::cout << "GIF FPS set to 15" << std::endl;
                // }
                // else if (event.key.code == sf::Keyboard::Num3) {
                //     gifRecorder.setFPS(24.0f);
                //     std::cout << "GIF FPS set to 24" << std::endl;
                // }
                // else if (event.key.code == sf::Keyboard::Num4) {
                //     gifRecorder.setFPS(30.0f);
                //     std::cout << "GIF FPS set to 30" << std::endl;
                // }
            }
        }

        analyzer.update(deltaSeconds);
        float volume = analyzer.getVolume();
        float bass = analyzer.getBass();
        float mid = analyzer.getMid();
        float treble = analyzer.getTreble();

        window.clear(sf::Color(10, 10, 30));

        int rectIndex = 0;
        int circleIndex = 0;

        for (size_t i = 0; i < shapeTypes.size(); ++i) {
            float beat = std::fmod(time * (2.0f + bass * 3.0f), 1.0f);
            float pulse = 1.0f + 0.2f * std::sin(time * 10.0f) * volume;

            sf::Vector2f position;
            int ruleType = i % 5;

            switch (ruleType) {
                case 0: // Circular placement
                    {
                        float angle = time * 0.5f + i * 0.1f;
                        float radius = 100 + i % 200 * (1.0f + volume);
                        position = placer.circularPlacement(angle, radius, beat);
                    }
                    break;
                case 1: // Grid placement
                    {
                        int xIndex = i % 20;
                        int yIndex = (i / 20) % 20;
                        position = placer.gridPlacement(xIndex, yIndex, 20, 20, pulse);
                    }
                    break;
                case 2: // Wave placement
                    {
                        float t = float(i) / shapeTypes.size();
                        float frequency = 2.0f + treble * 5.0f;
                        float amplitude = 100 + mid * 200;
                        float phase = time * 2.0f;
                        position = placer.wavePlacement(t, frequency, amplitude, phase);
                    }
                    break;
                case 3: // Spiral placement
                    {
                        float angle = time * 0.5f + i * 0.05f;
                        float radius = 50 + i % 100 * (1.0f + bass);
                        float growth = 0.1f + 0.2f * mid;
                        position = placer.spiralPlacement(angle, radius, growth, time);
                    }
                    break;
                default: // Random placement
                    position = placer.randomPlacement(gen, volume);
                    break;
            }

            sf::Color selectedColor = palette[paletteIndexDist(gen)];
            selectedColor.a = alphaDist(gen) * (0.7f + 0.3f * volume);

            float sizeFactor = 0.8f + 0.4f * bass;
            float rotation = rotationDist(gen) + time * 20.0f * treble;

            if (shapeTypes[i] == 0) { // Rectangle
                if (rectIndex < rects.size()) {
                    sf::Vector2f newSize = originalRectSizes[rectIndex] * sizeFactor;
                    rects[rectIndex].setSize(newSize);
                    rects[rectIndex].setPosition(position);
                    rects[rectIndex].setFillColor(selectedColor);
                    rects[rectIndex].setRotation(rotation);
                    rects[rectIndex].setOrigin(newSize.x / 2, newSize.y / 2);
                    window.draw(rects[rectIndex]);
                    rectIndex++;
                }
            } else { // Circle
                if (circleIndex < circles.size()) {
                    float newRadius = originalCircleRadii[circleIndex] * sizeFactor;
                    circles[circleIndex].setRadius(newRadius);
                    circles[circleIndex].setPosition(position);
                    circles[circleIndex].setFillColor(selectedColor);
                    circles[circleIndex].setRotation(rotation);
                    circles[circleIndex].setOrigin(newRadius, newRadius);
                    window.draw(circles[circleIndex]);
                    circleIndex++;
                }
            }
        }

        // Display UI
        if (font.getInfo().family != "") {
            sf::Text instructions;
            instructions.setFont(font);
            instructions.setCharacterSize(14);
            instructions.setFillColor(sf::Color::White);
            instructions.setPosition(10, 10);
            instructions.setString("R: Record | S: Save image | Q: Quit | 1-4: Set FPS");
            window.draw(instructions);

            std::stringstream musicData;
            musicData << std::fixed << std::setprecision(2);
            musicData << "Volume: " << volume << " | Bass: " << bass << " | Mid: " << mid << " | Treble: " << treble;

            sf::Text dataText;
            dataText.setFont(font);
            dataText.setCharacterSize(14);
            dataText.setFillColor(sf::Color::White);
            dataText.setPosition(10, 30);
            dataText.setString(musicData.str());
            window.draw(dataText);

            sf::Text fpsText;
            fpsText.setFont(font);
            fpsText.setCharacterSize(14);
            fpsText.setFillColor(sf::Color::Yellow);
            fpsText.setPosition(10, 50);
            fpsText.setString("GIF FPS: " + std::to_string(static_cast<int>(gifRecorder.getFPS())));
            window.draw(fpsText);

            if (gifRecorder.isRecordingNow()) {
                sf::Text recordingText;
                recordingText.setFont(font);
                recordingText.setCharacterSize(14);
                recordingText.setFillColor(sf::Color::Red);
                recordingText.setPosition(10, 70);
                recordingText.setString("Recording: " + std::to_string(gifRecorder.getRecordedFrames()) +
                                       "/" + std::to_string(gifRecorder.getMaxFrames()) + " frames");
                window.draw(recordingText);
            }
        }

        // Display the window
        window.display();
        gifRecorder.update(deltaSeconds, window);
        frameCount++;
    }

    return 0;
}
