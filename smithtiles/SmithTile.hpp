#ifndef SMITHTILE_HPP
#define SMITHTILE_HPP

#include <SFML/Graphics.hpp>

class SmithTile {
private:
    sf::Vector2f position;
    float size;
    sf::Color color;
    int variant;

public:
    SmithTile(sf::Vector2f pos, float s, sf::Color c, int v);
    void draw(sf::RenderTarget& target);
};

#endif // SMITHTILE_HPP
