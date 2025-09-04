#include <SFML/Graphics.hpp>
#include "Monograph.hpp"
#include <iostream>
#include <string>
#include <map>
#include <limits>
#include  <filesystem>

// Print a color block to the terminal
void printColorBlock(const sf::Color& color) {
    std::cout << "\033[48;2;" << (int)color.r << ";" << (int)color.g << ";" << (int)color.b << "m    \033[0m";
}

std::string getPaletteChoice() {
    std::map<int, std::string> paletteOptions;
    int optionNumber = 1;
 
    // Static map of all available palettes
    const std::map<std::string, std::vector<sf::Color>>& allPalettes = {
        {"vibrant", getPalette("vibrant")},
        {"pastel", getPalette("pastel")},
        {"earthy", getPalette("earthy")},
        {"neon", getPalette("neon")},
        {"monochrome", getPalette("monochrome")}
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

    // Clear the input buffer in case of bad input
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (paletteOptions.count(choice)) {
        return paletteOptions.at(choice);
    } else {
        std::cerr << "Invalid choice. Using default palette 'vibrant'." << std::endl;
        return "vibrant";
    }
}

int main() {
    int windowWidth = 800;
    int windowHeight = 600;

    std::string paletteChoice = getPaletteChoice();

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Monograph");
    window.setFramerateLimit(60);

    // Load font for UI
    sf::Font font;
    if (!font.loadFromFile("fonts/montana-light.ttf")) {
        std::cerr << "Warning: Could not load font. UI text will not be displayed." << std::endl;
    }

    Monograph monograph(window, windowWidth, windowHeight, paletteChoice);

    // UI text
    sf::Text instructions;
    instructions.setFont(font);
    instructions.setCharacterSize(16);
    instructions.setFillColor(sf::Color::Black);
    instructions.setPosition(10, 10);
    instructions.setString("R: Regenerate | S: Save Image | Q: Quit");

    bool needsRedraw = true;
    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
           if (event.type == sf::Event::KeyPressed) {
                // Exit on Q key press
                if (event.key.code == sf::Keyboard::Q) {
                    window.close();
                }
                if (event.key.code == sf::Keyboard::R) {
                    monograph.generate();
                    needsRedraw = true;
                    std::cout << "Regenerated artwork." << std::endl;
                }
                if (event.key.code == sf::Keyboard::S) {
                    // Save current frame to image
                    sf::Texture texture;
                    texture.create(window.getSize().x, window.getSize().y);
                    texture.update(window);
                    if (texture.copyToImage().saveToFile("monograph_output.png")) {
                        std::cout << "Saved image to monograph_output.png" << std::endl;
                    } else {
                        std::cerr << "Failed to save image." << std::endl;
                    }
                }
            }
        }

        if (needsRedraw) {
            window.clear(sf::Color::White);
            monograph.draw();

            // Draw UI if font is loaded
            if (font.getInfo().family != "") {
                window.draw(instructions);
            }

            window.display();
            needsRedraw = false;
        }

        // Small delay to reduce CPU usage
        sf::sleep(sf::milliseconds(16));
    }

    return 0;
}
