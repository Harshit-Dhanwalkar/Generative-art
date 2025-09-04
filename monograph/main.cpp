#include <SFML/Graphics.hpp>
#include "Monograph.hpp"
#include <iostream>
#include <string>
#include <map>
#include <limits> // Required for numeric_limits

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

    Monograph monograph(window, windowWidth, windowHeight, paletteChoice);

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
            }
        }

        window.clear(sf::Color(255, 255, 255));
        monograph.draw();
        window.display();
    }

    return 0;
}
