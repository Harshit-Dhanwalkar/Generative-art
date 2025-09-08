#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <cmath>
#include <iostream>
#include <random>
#include <ctime>
#include <deque>
#include <map>
#include "../lib/Palettes.hpp"
#include "../lib/GIFRecorder.hpp"

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const float HORN_LENGTH = 10.0f;
const int SEGMENTS = 200;
const int CIRCLE_POINTS = 30;
const float SCALE = 50.0f;
const float ROTATION_SPEED = 0.3f;

// Print a color block to the terminal
void printColorBlock(const sf::Color& color) {
    std::cout << "\033[48;2;" << (int)color.r << ";" << (int)color.g << ";" << (int)color.b << "m    \033[0m";
}

// Function to get the user's palette choice from the terminal
std::string getPaletteChoice() {
    std::map<int, std::string> paletteOptions;
    int optionNumber = 1;

    std::map<std::string, std::vector<sf::Color>> allPalettes = {
        {"vibrant", getPalette("vibrant")},
        {"pastel", getPalette("pastel")},
        {"earthy", getPalette("earthy")},
        {"neon", getPalette("neon")},
        {"monochrome", getPalette("monochrome")},
        {"default", getPalette("default")}
    };
    
    std::cout << "Available palettes:\n";
    for (const auto& pair : allPalettes) {
        std::cout << optionNumber << ". " << pair.first << ": ";
        paletteOptions[optionNumber] = pair.first;
        for (const auto& color : pair.second) {
            printColorBlock(color);
        }
        std::cout << std::endl;
        optionNumber++;
    }

    std::cout << "Enter the number of your desired palette: ";
    int choice;
    std::cin >> choice;

    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (paletteOptions.count(choice)) {
        return paletteOptions.at(choice);
    } else {
        std::cerr << "Invalid choice. Using default palette 'vibrant'." << std::endl;
        return "vibrant";
    }
}

class GabrielsHorn {
private:
    sf::RenderWindow& window;
    std::vector<sf::VertexArray> hornSegments;
    float rotationAngleY;
    float rotationAngleX;
    const std::vector<sf::Color>& palette;
    float currentHornLength;

public:
    GabrielsHorn(sf::RenderWindow& win, const std::string& paletteName) 
        : window(win), rotationAngleY(0.0f), rotationAngleX(0.0f), currentHornLength(HORN_LENGTH), palette(getPalette(paletteName)) {
        generateHorn();
    }
    
    void regenerateHorn() {
        generateHorn();
    }

    void setHornLengthFromMouse(int mouseY) {
        currentHornLength = 2.0f + (static_cast<float>(mouseY) / WINDOW_HEIGHT) * 18.0f;
        generateHorn();
    }

    void setRotationFromMouseY(int mouseX) {
        rotationAngleY = (static_cast<float>(mouseX) / WINDOW_WIDTH) * 2.0f * M_PI;
    }

    void setRotationFromMouseX(int mouseX) {
        // Map mouseX from 0 to WINDOW_WIDTH to an angle from -PI to PI
        rotationAngleX = (static_cast<float>(mouseX) / WINDOW_WIDTH) * 2.0f * M_PI - M_PI;
    }

    void generateHorn() {
        hornSegments.clear();

        for (int i = 0; i < SEGMENTS; ++i) {
            float y = 1.0f + (currentHornLength * i) / SEGMENTS;
            float radius = 1.0f / y;

            sf::VertexArray circle(sf::LineStrip);

            for (int j = 0; j <= CIRCLE_POINTS; ++j) {
                float angle = 2.0f * M_PI * j / CIRCLE_POINTS;
                float x = radius * std::cos(angle);
                float z = radius * std::sin(angle);

                sf::Vector2f projected = isometricProjection(x, -y, z);

                sf::Color color = palette[j % palette.size()];
                circle.append(sf::Vertex(projected, color));
            }

            hornSegments.push_back(circle);
        }

        for (int j = 0; j < CIRCLE_POINTS; ++j) {
            sf::VertexArray line(sf::LineStrip);

            for (int i = 0; i < SEGMENTS; ++i) {
                float y = 1.0f + (currentHornLength * i) / SEGMENTS;
                float radius = 1.0f / y;
                float angle = 2.0f * M_PI * j / CIRCLE_POINTS;
                float x = radius * std::cos(angle);
                float z = radius * std::sin(angle);

                sf::Vector2f projected = isometricProjection(x, -y, z);

                sf::Color color = palette[i % palette.size()];
                line.append(sf::Vertex(projected, color));
            }

            hornSegments.push_back(line);
        }
    }

    sf::Vector2f isometricProjection(float x, float y, float z) {
        // Rotation around the X-axis
        float tempY = y * std::cos(rotationAngleX) - z * std::sin(rotationAngleX);
        float tempZ = y * std::sin(rotationAngleX) + z * std::cos(rotationAngleX);
        y = tempY;
        z = tempZ;

        // Rotation around the Y-axis
        float tempX = x * std::cos(rotationAngleY) - z * std::sin(rotationAngleY);
        float rotatedZ = x * std::sin(rotationAngleY) + z * std::cos(rotationAngleY);
        float rotatedX = tempX;

        float screenX = WINDOW_WIDTH / 2.0f + SCALE * (rotatedX - rotatedZ);
        float screenY = WINDOW_HEIGHT / 2.0f - SCALE * (y + (rotatedX + rotatedZ) * 0.3f);
        return sf::Vector2f(screenX, screenY);
    }

    void draw(const std::string& statusText) {
        window.clear(sf::Color::Black);

        for (auto& segment : hornSegments) {
            window.draw(segment);
        }

        sf::Font font;
        if (font.loadFromFile("fonts/montana-bold.ttf")) {
            sf::Text infoText;
            infoText.setFont(font);
            infoText.setCharacterSize(16);
            infoText.setFillColor(sf::Color::White);
            infoText.setPosition(10, 10);
            infoText.setString("R: Regenerate | S: Save Image | G: Record GIF | Q: Quit\n\n" + statusText);
            window.draw(infoText);
        }
    }

    void saveImage() {
        sf::Texture texture;
        texture.create(window.getSize().x, window.getSize().y);
        texture.update(window);

        std::time_t now = std::time(nullptr);
        std::tm* localTime = std::localtime(&now);
        char filename[100];
        std::strftime(filename, sizeof(filename), "gabriels_horn_%Y%m%d_%H%M%S.png", localTime);

        if (texture.copyToImage().saveToFile(filename)) {
            std::cout << "Saved image to " << filename << std::endl;
        } else {
            std::cerr << "Failed to save image." << std::endl;
        }
    }
};

int main() {
    std::string paletteName = getPaletteChoice();

    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Gabriel's Horn");
    window.setFramerateLimit(60);

    GabrielsHorn horn(window, paletteName);
    sf::Clock clock;

    GIFRecorder recorder(WINDOW_WIDTH, WINDOW_HEIGHT, 300, 30.0f);
    std::string statusText = "";

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::MouseMoved) {
                horn.setHornLengthFromMouse(event.mouseMove.y);
                if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                    horn.setRotationFromMouseX(event.mouseMove.x);
                } else {
                    horn.setRotationFromMouseY(event.mouseMove.x);
                }
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Q) {
                    window.close();
                } else if (event.key.code == sf::Keyboard::R) {
                    horn.regenerateHorn();
                    std::cout << "Horn regenerated." << std::endl;
                } else if (event.key.code == sf::Keyboard::S) {
                    horn.saveImage();
                } else if (event.key.code == sf::Keyboard::G) {
                    if (recorder.isRecordingNow()) {
                        recorder.stopRecording();
                        statusText = "Recording stopped. Check terminal for instructions.";
                    } else {
                        recorder.startRecording();
                        statusText = "Recording...";
                    }
                }
            }
        }

        float deltaTime = clock.restart().asSeconds();
        recorder.update(deltaTime, window);

        if (recorder.isRecordingNow()) {
            statusText = "Recording: " + std::to_string(recorder.getRecordedFrames()) + "/" + std::to_string(recorder.getMaxFrames()) + " frames";
        }

        horn.draw(statusText);

        window.display();
    }

    return 0;
}
