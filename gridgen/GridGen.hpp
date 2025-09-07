#ifndef GRIDGEN_HPP
#define GRIDGEN_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include "../lib/Palettes.hpp"

class GridGen {
private:
    sf::RenderWindow& window;
    std::vector<std::vector<sf::Vector2f>> points;
    int cols;
    int rows;
    float cellWidth;
    float cellHeight;
    float pointRadius;
    std::vector<sf::Color> palette;
    
public:
    GridGen(sf::RenderWindow& win, int c, int r, const std::string& paletteName) 
        : window(win), cols(c), rows(r) {
        cellWidth = (float)window.getSize().x / (cols - 1);
        cellHeight = (float)window.getSize().y / (rows - 1);
        pointRadius = 4.0f;
        palette = getPalette(paletteName);
        generateGrid();
    }

    void generateGrid() {
        points.clear();
        points.resize(rows, std::vector<sf::Vector2f>(cols));

        float marginX = window.getSize().x * 0.15f;
        float marginY = window.getSize().y * 0.15f;
        float drawableWidth = window.getSize().x - 2 * marginX;
        float drawableHeight = window.getSize().y - 2 * marginY;

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                float x = marginX + (j / (float)(cols - 1)) * drawableWidth;
                float y = marginY + (i / (float)(rows - 1)) * drawableHeight;
                points[i][j] = sf::Vector2f(x, y);
            }
        }
    }

    void warpGrid(float centerX, float centerY, float strength, float falloff) {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                sf::Vector2f& p = points[i][j];
                float dx = p.x - centerX;
                float dy = p.y - centerY;
                float dist = std::sqrt(dx * dx + dy * dy);

                if (dist < falloff) {
                    float normalizedDist = dist / falloff;
                    float warpFactor = strength * std::cos(normalizedDist * M_PI / 2.0f);
                    p.x += dx * warpFactor;
                    p.y += dy * warpFactor;
                }
            }
        }
    }

    void draw() {
        // background color
        window.clear(sf::Color::Black);

        // Draw grid lines
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                sf::Vertex line[] = {
                    sf::Vertex(points[i][j], sf::Color::White),
                    sf::Vertex(points[i][j], sf::Color::White)
                };

                // Horizontal line
                if (j < cols - 1) {
                    line[1].position = points[i][j+1];
                    window.draw(line, 2, sf::Lines);
                }

                // Vertical line
                if (i < rows - 1) {
                    line[1].position = points[i+1][j];
                    window.draw(line, 2, sf::Lines);
                }
            }
        }

        // Draw circles at each grid point
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                sf::CircleShape circle;
                circle.setRadius(pointRadius);
                circle.setOrigin(pointRadius, pointRadius);
                circle.setPosition(points[i][j]);

                float centerX = window.getSize().x / 2.0f;
                float centerY = window.getSize().y / 2.0f;
                float dist = std::sqrt(std::pow(points[i][j].x - centerX, 2) + std::pow(points[i][j].y - centerY, 2));

                if (dist < 150.0f) {
                    circle.setRadius(pointRadius * (1.0f + (150.0f - dist) / 150.0f));
                    circle.setOrigin(circle.getRadius(), circle.getRadius());
                    circle.setFillColor(palette[1]);
                    circle.setOutlineThickness(0.0f);
                } else {
                    circle.setFillColor(palette[2]);
                    circle.setOutlineThickness(0.0f);
                }

                window.draw(circle);
            }
        }
    }
};

#endif // GRIDGEN_HPP
