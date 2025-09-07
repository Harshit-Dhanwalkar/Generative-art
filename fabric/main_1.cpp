#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <cmath>
#include <iostream>
#include <random>
#include <ctime>

const int GRID_WIDTH = 50;
const int GRID_HEIGHT = 50;
const float CELL_SIZE = 20.0f;
const float DAMPING_FACTOR = 0.995f;
const int PHYSICS_ITERATIONS = 5;
const float MOUSE_FORCE_RADIUS = 150.0f;
const float MOUSE_FORCE_STRENGTH = 200.0f;
const int SEGMENTS_PER_CONSTRAINT = 8;

struct Node {
    sf::Vector2f position;
    sf::Vector2f previousPosition;
    bool isPinned = false;
};

// A constraint (or "spring")
struct Constraint {
    int nodeAIndex;
    int nodeBIndex;
    float length;
};

sf::Color getRandomColor() {
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_int_distribution<> dist(50, 255);
    return sf::Color(dist(gen), dist(gen), dist(gen));
}

class FabricSimulation {
public:
    FabricSimulation() {
        initialize();
    }

    void initialize() {
        m_nodes.clear();
        m_constraints.clear();

        m_nodes.resize(GRID_WIDTH * GRID_HEIGHT);
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            for (int x = 0; x < GRID_WIDTH; ++x) {
                int index = y * GRID_WIDTH + x;
                m_nodes[index].position = sf::Vector2f(x * CELL_SIZE, y * CELL_SIZE);
                m_nodes[index].previousPosition = m_nodes[index].position;

                // Pin the nodes on the top border
                if (y == 0) {
                    m_nodes[index].isPinned = true;
                }
            }
        }

        for (int y = 0; y < GRID_HEIGHT; ++y) {
            for (int x = 0; x < GRID_WIDTH; ++x) {
                int index = y * GRID_WIDTH + x;

                if (x < GRID_WIDTH - 1) {
                    addConstraint(index, index + 1);
                }
                if (y < GRID_HEIGHT - 1) {
                    addConstraint(index, (y + 1) * GRID_WIDTH + x);
                }
            }
        }
    }

    void update(float deltaTime, sf::Vector2f mousePosition) {
        for (auto& node : m_nodes) {
            if (!node.isPinned) {
                sf::Vector2f diff = mousePosition - node.position;
                float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);
                if (distance < MOUSE_FORCE_RADIUS) {
                    sf::Vector2f force = -diff / (distance + 1.0f) * MOUSE_FORCE_STRENGTH;
                    node.position += force * deltaTime;
                }
            }
        }

        for (auto& node : m_nodes) {
            if (!node.isPinned) {
                sf::Vector2f velocity = node.position - node.previousPosition;
                node.previousPosition = node.position;
                node.position += velocity * DAMPING_FACTOR; // damping
                node.position += sf::Vector2f(0.0f, 9.8f * 5.0f * deltaTime); // gravity
            }
        }

        for (int i = 0; i < PHYSICS_ITERATIONS; ++i) {
            for (const auto& constraint : m_constraints) {
                Node& nodeA = m_nodes[constraint.nodeAIndex];
                Node& nodeB = m_nodes[constraint.nodeBIndex];

                sf::Vector2f delta = nodeB.position - nodeA.position;
                float currentLength = std::sqrt(delta.x * delta.x + delta.y * delta.y);
                float difference = (currentLength - constraint.length) / currentLength;

                sf::Vector2f correction = delta * difference * 0.5f;

                if (!nodeA.isPinned) {
                    nodeA.position += correction;
                }
                if (!nodeB.isPinned) {
                    nodeB.position -= correction;
                }
            }
        }
    }

    void draw(sf::RenderTarget& target) {
        m_vertices.clear();
        m_vertices.setPrimitiveType(sf::Lines);

        for (const auto& constraint : m_constraints) {
            const Node& nodeA = m_nodes[constraint.nodeAIndex];
            const Node& nodeB = m_nodes[constraint.nodeBIndex];

            sf::Vector2f delta = nodeB.position - nodeA.position;

            for (int i = 0; i < SEGMENTS_PER_CONSTRAINT; ++i) {
                float ratio1 = (float)i / (float)SEGMENTS_PER_CONSTRAINT;
                float ratio2 = (float)(i + 1) / (float)SEGMENTS_PER_CONSTRAINT;

                sf::Vector2f p1 = nodeA.position + (delta * ratio1);
                sf::Vector2f p2 = nodeA.position + (delta * ratio2);

                sf::Color color1 = getRandomColor();
                sf::Color color2 = getRandomColor();

                m_vertices.append(sf::Vertex(p1, color1));
                m_vertices.append(sf::Vertex(p2, color2));
            }
        }
        target.draw(m_vertices);
    }

private:
    void addConstraint(int nodeA, int nodeB) {
        Constraint c;
        c.nodeAIndex = nodeA;
        c.nodeBIndex = nodeB;
        sf::Vector2f delta = m_nodes[nodeB].position - m_nodes[nodeA].position;
        c.length = std::sqrt(delta.x * delta.x + delta.y * delta.y);
        m_constraints.push_back(c);
    }

    std::vector<Node> m_nodes;
    std::vector<Constraint> m_constraints;
    sf::VertexArray m_vertices;
};

int main() {
    sf::RenderWindow window(sf::VideoMode(GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE), "Interactive Fabric Grid");
    window.setFramerateLimit(60);

    // Load font for UI
    sf::Font font;
    if (!font.loadFromFile("fonts/montana-light.ttf")) {
        std::cerr << "Warning: Could not load font. UI text will not be displayed." << std::endl;
    }

    FabricSimulation fabric;
    sf::Clock clock;

    // UI text
    sf::Text instructions;
    instructions.setFont(font);
    instructions.setCharacterSize(16);
    instructions.setFillColor(sf::Color::White);
    instructions.setPosition(10, 10);
    instructions.setString("R: Reset | S: Save Image | Q: Quit");

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
                // Reset on R key press
                if (event.key.code == sf::Keyboard::R) {
                    fabric.initialize();
                    std::cout << "Fabric simulation reset." << std::endl;
                }
                // Save on S key press
                if (event.key.code == sf::Keyboard::S) {
                    sf::RenderTexture renderTexture;
                    if (renderTexture.create(window.getSize().x, window.getSize().y)) {
                        renderTexture.clear(sf::Color::Black);
                        fabric.draw(renderTexture);
                        renderTexture.display();

                        std::time_t now = std::time(nullptr);
                        std::tm* localTime = std::localtime(&now);
                        char filename[100];
                        std::strftime(filename, sizeof(filename), "fabric_%Y%m%d_%H%M%S.png", localTime);

                        if (renderTexture.getTexture().copyToImage().saveToFile(filename)) {
                            std::cout << "Saved image to " << filename << std::endl;
                        } else {
                            std::cerr << "Failed to save image." << std::endl;
                        }
                    }
                }
            }
        }

        float deltaTime = clock.restart().asSeconds();
        sf::Vector2f mousePosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));

        fabric.update(deltaTime, mousePosition);

        window.clear(sf::Color::Black);
        fabric.draw(window);

        // Draw UI if font is loaded
        if (font.getInfo().family != "") {
            window.draw(instructions);
        }

        window.display();
    }

    return 0;
}
