#include "SmithTile.hpp"
#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>

SmithTile::SmithTile(sf::Vector2f pos, float s, sf::Color c, int v) 
    : position(pos), size(s), color(c), variant(v) {}

void SmithTile::draw(sf::RenderTarget& target) {
    // Circle
    sf::CircleShape circle(size/2);
    circle.setFillColor(color);
    circle.setOrigin(size/2, size/2);

    // Render texture to compose tile
    sf::RenderTexture texture;
    texture.create(size, size);
    texture.clear(sf::Color::Transparent);

    // Position the circle at the corners based on variant
    switch(variant) {
        case 0: // Top-left and bottom-right
            circle.setPosition(0, 0);
            texture.draw(circle);
            circle.setPosition(size, size);
            texture.draw(circle);
            break;
        case 1: // Top-right and bottom-left
            circle.setPosition(size, 0);
            texture.draw(circle);
            circle.setPosition(0, size);
            texture.draw(circle);
            break;
        case 2: // All four corners (full circle)
            circle.setPosition(0, 0);
            texture.draw(circle);
            circle.setPosition(size, 0);
            texture.draw(circle);
            circle.setPosition(0, size);
            texture.draw(circle);
            circle.setPosition(size, size);
            texture.draw(circle);
            break;
        case 3: // Empty (no circles)
            break;
    }

    texture.display();

    sf::Sprite sprite(texture.getTexture());
    sprite.setPosition(position);
    target.draw(sprite);
}
