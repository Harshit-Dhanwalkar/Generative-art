#ifndef PALETTES_HPP
#define PALETTES_HPP

#include <SFML/Graphics.hpp>
#include <map>
#include <vector>
#include <iostream>

const std::vector<sf::Color>& getPalette(const std::string& name) {
    static const std::map<std::string, std::vector<sf::Color>> palettes = {
        {"vibrant", {
            sf::Color(255, 102, 102),   // Red
            sf::Color(255, 255, 102),   // Yellow
            sf::Color(102, 255, 102),   // Light Green
            sf::Color(102, 102, 255),   // Blue
            sf::Color(255, 102, 255)    // Magenta
        }},
        {"pastel", {
            sf::Color(207, 216, 220),   // Blue Gray
            sf::Color(225, 190, 231),   // Lavender
            sf::Color(255, 243, 224),   // Off White
            sf::Color(178, 223, 219),   // Light Cyan
            sf::Color(255, 236, 179)    // Yellow
        }},
        {"earthy", {
            sf::Color(139, 69, 19),
            sf::Color(205, 133, 63),
            sf::Color(160, 82, 45),
            sf::Color(245, 245, 220),
            sf::Color(188, 143, 143)
        }},
        {"default", {
            sf::Color(117, 131, 158),
            sf::Color(219, 142, 60),
            sf::Color(255, 222, 173),
            sf::Color(107, 185, 240),
            sf::Color(189, 195, 199),
            sf::Color(102, 205, 170)
        }}
    };

    auto it = palettes.find(name);
    if (it != palettes.end()) {
        return it->second;
    }

    std::cerr << "Warning: Palette '" << name << "' not found. Using default." << std::endl;
    return palettes.at("default");
}

#endif // PALETTES_HPP
