#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include <random>
#include "SmithTile.hpp"
#include "../lib/Palettes.hpp"

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
    int windowHeight = 800;
    int tilesize = 100;
    int gridsize = windowWidth / tilesize;

    std::string paletteChoice = getPaletteChoice();
    std::vector<sf::Color> palette = getPalette(paletteChoice);

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Smith tiles");
    window.setFramerateLimit(60);

    // Load font for UI
    sf::Font font;
    if (!font.loadFromFile("../fonts/montana-light.ttf")) {
        std::cerr << "Warning: Could not load font. UI text will not be displayed." << std::endl;
    }

    // UI text
    sf::Text instructions;
    instructions.setFont(font);
    instructions.setCharacterSize(16);
    instructions.setFillColor(sf::Color::White);
    instructions.setPosition(10, 10);
    instructions.setString("R: Regenerate | S: Save Image | Q: Quit");

    bool needsRedraw = true;
    sf::Clock clock;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);

    std::vector<SmithTile> tiles;

    for (int y = 0; y < gridsize; ++y) {
        for (int x = 0; x < gridsize; ++x) {
            sf::Vector2f position(x * tilesize, y * tilesize);
            int variant = dis(gen);

            sf::Color color = palette[(y * gridsize + x) % palette.size()];

            tiles.emplace_back(position, tilesize, color, variant);
        }
    }

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
                // Regenerate on R key press
                if (event.key.code == sf::Keyboard::R) {
                    tiles.clear();
                    for (int y = 0; y < gridsize; ++y) {
                        for (int x = 0; x < gridsize; ++x) {
                            sf::Vector2f position(x * tilesize, y * tilesize);
                            int variant = dis(gen);
                            sf::Color color = palette[(y * gridsize + x) % palette.size()];
                            tiles.emplace_back(position, tilesize, color, variant);
                        }
                    }
                    needsRedraw = true;
                    std::cout << "Regenerated artwork." << std::endl;
                }
                // Save on S key press
                if (event.key.code == sf::Keyboard::S) {
                    sf::Texture texture;
                    texture.create(window.getSize().x, window.getSize().y);
                    texture.update(window);
                    if (texture.copyToImage().saveToFile("smithtiles_output.png")) {
                        std::cout << "Saved image to smithtiles_output.png" << std::endl;
                    } else {
                        std::cerr << "Failed to save image." << std::endl;
                    }
                }
            }
        }

        window.clear(sf::Color::Black);

        for (auto& tile : tiles) {
            tile.draw(window);
        }

        window.display();
    }

    return 0;
}
