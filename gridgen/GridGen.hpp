#ifndef GRIDGEN_HPP
#define GRIDGEN_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include "../lib/Palettes.hpp"

class GridGen {
private:
    sf::RenderWindow& window;
    std::vector<std::vector<sf::Vector2f>> points;
    std::vector<std::vector<sf::Vector2f>> originalPoints;
    int cols;
    int rows;
    float pointRadius;
    std::vector<sf::Color> palette;

    // Circle
    struct AttractorCircle {
        sf::Vector2f center;
        float radius;
        float strength;
    };
    std::vector<AttractorCircle> circles;

    // Random number generator
    std::mt19937 gen;
    std::uniform_real_distribution<> dis;

public:
    GridGen(sf::RenderWindow& win, int c, int r, const std::string& paletteName) 
        : window(win), cols(c), rows(r), dis(0.0, 1.0) {
        pointRadius = 4.0f;
        palette = getPalette(paletteName);

        std::random_device rd;
        gen.seed(rd());

        generateGrid();
        generateCircles();
        applyGravityDistortion();
    }

    void generateGrid() {
        points.clear();
        originalPoints.clear();
        points.resize(rows, std::vector<sf::Vector2f>(cols));
        originalPoints.resize(rows, std::vector<sf::Vector2f>(cols));

        float marginX = window.getSize().x * 0.15f;
        float marginY = window.getSize().y * 0.15f;
        float drawableWidth = window.getSize().x - 2 * marginX;
        float drawableHeight = window.getSize().y - 2 * marginY;

        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                float x = marginX + (j / (float)(cols - 1)) * drawableWidth;
                float y = marginY + (i / (float)(rows - 1)) * drawableHeight;
                originalPoints[i][j] = sf::Vector2f(x, y);
            }
        }
        points = originalPoints;
    }

    void generateCircles() {
        circles.clear();

        // Generate 3-6 random circles
        int numCircles = 3 + static_cast<int>(dis(gen) * 4);

        for (int i = 0; i < numCircles; ++i) {
            AttractorCircle circle;

            // Random position within central area
            float margin = 0.3f;
            circle.center.x = window.getSize().x * (margin + dis(gen) * (1 - 2*margin));
            circle.center.y = window.getSize().y * (margin + dis(gen) * (1 - 2*margin));

            // Random radius and strength
            circle.radius = 30.0f + dis(gen) * 70.0f;
            circle.strength = 0.3f + dis(gen) * 0.5f;

            circles.push_back(circle);
        }
    }

    // Distortion to a point based on all attractor circles
    sf::Vector2f applyDistortionToPoint(const sf::Vector2f& originalPoint) {
        sf::Vector2f distortedPoint = originalPoint;

        for (const auto& circle : circles) {
            // Calculate distance to circle center
            float dx = distortedPoint.x - circle.center.x;
            float dy = distortedPoint.y - circle.center.y;
            float dist = std::sqrt(dx*dx + dy*dy);

            if (dist < circle.radius * 3.0f) {
                // Normalize distance
                float normDist = std::max(0.0f, (dist - circle.radius) / (circle.radius * 2.0f));

                // Calculate displacement
                float displacement = circle.strength * (1.0f - normDist) * circle.radius * 0.5f;

                if (dist > 0) {
                    distortedPoint.x -= displacement * dx / dist;
                    distortedPoint.y -= displacement * dy / dist;
                }
            }
        }

        return distortedPoint;
    }

    void applyGravityDistortion() {
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                points[i][j] = applyDistortionToPoint(originalPoints[i][j]);
            }
        }
    }

    float getCircleSizeForPoint(const sf::Vector2f& point) {
        float maxSizeFactor = 0.0f;

        for (const auto& circle : circles) {
            float dx = point.x - circle.center.x;
            float dy = point.y - circle.center.y;
            float dist = std::sqrt(dx*dx + dy*dy);

            if (dist < circle.radius) {
                float normDist = dist / circle.radius;
                float sizeFactor = (1.0f - normDist) * 2.0f;
                maxSizeFactor = std::max(maxSizeFactor, sizeFactor);
            }
        }

        return pointRadius * (1.0f + maxSizeFactor);
    }

    sf::Color getColorForPoint(const sf::Vector2f& point) {
        if (palette.empty()) return sf::Color::White;

        float minDist = std::numeric_limits<float>::max();
        float nearestRadius = 1.0f;

        // Nearest attractor circle
        for (const auto& circle : circles) {
            float dx = point.x - circle.center.x;
            float dy = point.y - circle.center.y;
            float dist = std::sqrt(dx*dx + dy*dy);

            if (dist < minDist) {
                minDist = dist;
                nearestRadius = circle.radius;
            }
        }

        float normalizedDist = std::min(1.0f, minDist / (nearestRadius * 1.5f));
        int colorIndex = static_cast<int>((1.0f - normalizedDist) * (palette.size() - 1));
        colorIndex = std::min(static_cast<int>(palette.size() - 1), 
                             std::max(0, colorIndex));

        return palette[colorIndex];
    }

    sf::Color getLineColor(const sf::Vector2f& start, const sf::Vector2f& end) {
        if (palette.size() < 2) return sf::Color::White;

        // Midpoint of the line for color calculation
        sf::Vector2f midpoint(
            (start.x + end.x) / 2.0f,
            (start.y + end.y) / 2.0f
        );

        return getColorForPoint(midpoint);
    }

    void draw() {
        window.clear(sf::Color::Black);

        // Draw grid lines with color gradient
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                // Draw horizontal lines
                if (j < cols - 1) {
                    sf::Vertex line[] = {
                        sf::Vertex(points[i][j], getLineColor(points[i][j], points[i][j+1])),
                        sf::Vertex(points[i][j+1], getLineColor(points[i][j], points[i][j+1]))
                    };
                    window.draw(line, 2, sf::Lines);
                }

                // Draw vertical lines
                if (i < rows - 1) {
                    sf::Vertex line[] = {
                        sf::Vertex(points[i][j], getLineColor(points[i][j], points[i+1][j])),
                        sf::Vertex(points[i+1][j], getLineColor(points[i][j], points[i+1][j]))
                    };
                    window.draw(line, 2, sf::Lines);
                }
            }
        }

        // Draw circles at each grid point with color and size
        for (int i = 0; i < rows; ++i) {
            for (int j = 0; j < cols; ++j) {
                sf::CircleShape circle;
                float circleSize = getCircleSizeForPoint(points[i][j]);
                circle.setRadius(circleSize);
                circle.setOrigin(circleSize, circleSize);
                circle.setPosition(points[i][j]);

                circle.setFillColor(getColorForPoint(points[i][j]));

                // Add outline
                if (circleSize > pointRadius * 1.8f) {
                    circle.setOutlineThickness(1.0f);
                    int outlineIndex = (std::distance(palette.begin(), 
                        std::find(palette.begin(), palette.end(), circle.getFillColor())) + 1) % palette.size();
                    circle.setOutlineColor(palette[outlineIndex]);
                } else {
                    circle.setOutlineThickness(0.0f);
                }

                window.draw(circle);
            }
        }
    }

    void regenerate() {
        generateCircles();
        applyGravityDistortion();
    }
};

#endif // GRIDGEN_HPP
