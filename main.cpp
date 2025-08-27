#include <SFML/Graphics.hpp>
#include <random>
#include <vector>
#include <iostream>
#include <map>
#include "Palettes.hpp"

// print a color block to the terminal
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
        {"earthy", getPalette("earthy")}
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
    const int windowHeight = 1000;

    // Create the main window
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Generative Art", sf::Style::Close);

    // Create a render texture
    sf::RenderTexture renderTexture;
    if (!renderTexture.create(windowWidth, windowHeight)) {
        return -1;
    }
    renderTexture.clear(sf::Color::Black);

    // Set up random number generation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> posX(0.0, static_cast<double>(windowWidth));
    std::uniform_real_distribution<> posY(0.0, static_cast<double>(windowHeight));
    std::uniform_real_distribution<> size(5.0, 150.0);
    std::uniform_int_distribution<> alphaVal(20, 120); // transparency
    std::uniform_real_distribution<> rotationAngle(0.0, 360.0);
    std::uniform_int_distribution<> paletteIndex(0, palette.size() - 1);

    const int numberOfRects = 5000; // density

    // Loop to draw the rectangles
    for (int i = 0; i < numberOfRects; ++i) {
        sf::RectangleShape rect(sf::Vector2f(size(gen), size(gen)));

        rect.setPosition(posX(gen), posY(gen));
        sf::Color selectedColor = palette[paletteIndex(gen)];
        selectedColor.a = alphaVal(gen);
        rect.setFillColor(selectedColor);

        rect.setOutlineColor(sf::Color::Black);
        rect.setOutlineThickness(1.0f);

        rect.setRotation(rotationAngle(gen));
        rect.setOrigin(rect.getLocalBounds().width / 2, rect.getLocalBounds().height / 2);

        renderTexture.draw(rect);
    }

    renderTexture.display();
    renderTexture.getTexture().copyToImage().saveToFile("generated_art.png");

    sf::Sprite sprite(renderTexture.getTexture());
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Q) {
                    window.close();
                }
            }
        }
        window.clear();
        window.draw(sprite);
        window.display();
    }

    return 0;
}
