#ifndef MONOGRAPH_HPP
#define MONOGRAPH_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <random>
#include <iostream>
#include <cmath>
#include "../lib/Palettes.hpp"

// Represents a 2D grid of values from 0 to 1
class Lightmap {
private:
    std::vector<std::vector<double>> map;
    int width, height;

public:
    Lightmap(int w, int h) : width(w), height(h) {
        map.resize(w, std::vector<double>(h, 0.0));
    }

    void generateRandom() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<> dis(0.0, 1.0);
        for (int i = 0; i < width; ++i) {
            for (int j = 0; j < height; ++j) {
                map[i][j] = dis(gen);
            }
        }
    }

    double getValue(int x, int y) const {
        if (x < 0 || x >= width || y < 0 || y >= height) {
            return 0.0;
        }
        // Simple bilinear interpolation to smooth the values
        int ix = static_cast<int>(x);
        int iy = static_cast<int>(y);
        double fx = x - ix;
        double fy = y - iy;

        double v00 = map[ix][iy];
        double v01 = map[ix][std::min(iy + 1, height - 1)];
        double v10 = map[std::min(ix + 1, width - 1)][iy];
        double v11 = map[std::min(ix + 1, width - 1)][std::min(iy + 1, height - 1)];

        double v0 = v00 * (1 - fx) + v10 * fx;
        double v1 = v01 * (1 - fx) + v11 * fy;

        return v0 * (1 - fy) + v1 * fy;
    }
};

// Enum to represent a corner
enum class Corner {
    TOP_LEFT,
    TOP_RIGHT,
    BOTTOM_LEFT,
    BOTTOM_RIGHT,
    NONE
};

// Represents a geometric shape with a rounded corner
struct Shape {
    sf::RectangleShape rect;
    sf::CircleShape cornerCircle;
    Corner roundedCorner;
    sf::Color color;
};

class Monograph {
private:
    std::vector<Shape> shapes;
    sf::RenderWindow& window;
    Lightmap detailMap;
    std::vector<sf::Color> palette;
    std::random_device rd;
    std::mt19937 gen;

    void subdivide(sf::FloatRect bounds, int depth) {
        double detail = detailMap.getValue(
            static_cast<int>(bounds.left + bounds.width / 2),
            static_cast<int>(bounds.top + bounds.height / 2)
        );

        if (depth <= 0 || (bounds.width < 10 && bounds.height < 10) || detail < 0.1) {
            Shape s;
            s.rect.setSize(sf::Vector2f(bounds.width, bounds.height));
            s.rect.setPosition(bounds.left, bounds.top);
            s.color = palette[gen() % palette.size()];
            s.rect.setFillColor(s.color);

            // a random corner for this specific shape
            std::uniform_int_distribution<> cornerDist(0, 3);
            s.roundedCorner = static_cast<Corner>(cornerDist(gen));

            float radius = std::min(bounds.width, bounds.height) * 0.3f;
            s.cornerCircle.setRadius(radius);
            s.cornerCircle.setPointCount(30);
            s.cornerCircle.setFillColor(s.color);

            // origin and position based on the rounded corner
            if (s.roundedCorner == Corner::TOP_LEFT) {
                s.cornerCircle.setOrigin(0, 0);
                s.cornerCircle.setPosition(bounds.left, bounds.top);
            } else if (s.roundedCorner == Corner::TOP_RIGHT) {
                s.cornerCircle.setOrigin(radius * 2, 0);
                s.cornerCircle.setPosition(bounds.left + bounds.width, bounds.top);
            } else if (s.roundedCorner == Corner::BOTTOM_LEFT) {
                s.cornerCircle.setOrigin(0, radius * 2);
                s.cornerCircle.setPosition(bounds.left, bounds.top + bounds.height);
            } else if (s.roundedCorner == Corner::BOTTOM_RIGHT) {
                s.cornerCircle.setOrigin(radius * 2, radius * 2);
                s.cornerCircle.setPosition(bounds.left + bounds.width, bounds.top + bounds.height);
            }

            shapes.push_back(s);
            return;
        }

        std::uniform_real_distribution<> dis(0.0, 1.0);
        bool splitHorizontally = (dis(gen) > 0.5);

        if (detail > 0.7) {
            splitHorizontally = true;
        } else if (detail < 0.3) {
            splitHorizontally = false;
        }

        if (splitHorizontally) {
            float splitPoint = bounds.width * (0.3 + dis(gen) * 0.4);
            subdivide(sf::FloatRect(bounds.left, bounds.top, splitPoint, bounds.height), depth - 1);
            subdivide(sf::FloatRect(bounds.left + splitPoint, bounds.top, bounds.width - splitPoint, bounds.height), depth - 1);
        } else {
            float splitPoint = bounds.height * (0.3 + dis(gen) * 0.4);
            subdivide(sf::FloatRect(bounds.left, bounds.top, bounds.width, splitPoint), depth - 1);
            subdivide(sf::FloatRect(bounds.left, bounds.top + splitPoint, bounds.width, bounds.height - splitPoint), depth - 1);
        }
    }

public:
    Monograph(sf::RenderWindow& win, int width, int height, const std::string& paletteName)
        : window(win), detailMap(width, height), gen(rd()) {
        palette = getPalette(paletteName);
        detailMap.generateRandom();
        generate();
    }

    void generate() {
        shapes.clear();
        sf::FloatRect initialBounds(0, 0, window.getSize().x, window.getSize().y);
        subdivide(initialBounds, 6);
    }

    void draw() {
        window.clear(sf::Color::White);
        for (const auto& shape : shapes) {
            sf::Color cornerColor = sf::Color::White;

            sf::RectangleShape cutoutRect;
            cutoutRect.setFillColor(sf::Color::White);
            float radius = shape.cornerCircle.getRadius();

            if (shape.roundedCorner == Corner::TOP_LEFT) {
                cutoutRect.setSize(sf::Vector2f(radius, radius));
                cutoutRect.setPosition(shape.rect.getPosition());
            } else if (shape.roundedCorner == Corner::TOP_RIGHT) {
                cutoutRect.setSize(sf::Vector2f(radius, radius));
                cutoutRect.setPosition(shape.rect.getPosition().x + shape.rect.getSize().x - radius, shape.rect.getPosition().y);
            } else if (shape.roundedCorner == Corner::BOTTOM_LEFT) {
                cutoutRect.setSize(sf::Vector2f(radius, radius));
                cutoutRect.setPosition(shape.rect.getPosition().x, shape.rect.getPosition().y + shape.rect.getSize().y - radius);
            } else if (shape.roundedCorner == Corner::BOTTOM_RIGHT) {
                cutoutRect.setSize(sf::Vector2f(radius, radius));
                cutoutRect.setPosition(shape.rect.getPosition().x + shape.rect.getSize().x - radius, shape.rect.getPosition().y + shape.rect.getSize().y - radius);
            }

            window.draw(shape.rect);
            window.draw(cutoutRect);
            window.draw(shape.cornerCircle);
        }
    }

    void update(float deltaTime) {
    }
};

#endif // MONOGRAPH_HPP
