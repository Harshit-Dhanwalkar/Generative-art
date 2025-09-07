#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <vector>
#include <cmath>
#include <iostream>
#include <random>
#include <ctime>
#include <deque>
#include <map>
#include "../lib/Palettes.hpp"
#include "../lib/GIFRecorder.hpp"

const int GRID_WIDTH = 50;
const int GRID_HEIGHT = 50;
const float CELL_SIZE = 20.0f;
const float DAMPING_FACTOR = 0.995f;
const int PHYSICS_ITERATIONS = 5;
const float MOUSE_FORCE_RADIUS = 150.0f;
const float MOUSE_FORCE_STRENGTH = 200.0f;
const int SEGMENTS_PER_CONSTRAINT = 8;
const int NUM_LAYERS = 5;
const float LAYER_DEPTH_OFFSET = 5.0f; 
const int MOUSE_HISTORY_SIZE = 10;

struct Node {
    sf::Vector2f position;
    sf::Vector2f previousPosition;
    bool isPinned = false;
    int layerIndex = 0;
};

// A constraint (or "spring")
struct Constraint {
    int nodeAIndex;
    int nodeBIndex;
    float length;
    bool isInterLayer = false;
};

// Print a color block to the terminal
void printColorBlock(const sf::Color& color) {
    std::cout << "\033[48;2;" << (int)color.r << ";" << (int)color.g << ";" << (int)color.b << "m    \033[0m";
}

// Function to get the user's palette choice from the terminal
std::string getPaletteChoice() {
    std::map<int, std::string> paletteOptions;
    int optionNumber = 1;

    // Static map of all available palettes
    const std::map<std::string, std::vector<sf::Color>> allPalettes = {
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

class FabricLayer {
public:
    FabricLayer(int layerIndex, float depthOffset, const std::vector<sf::Color>& palette) 
        : m_layerIndex(layerIndex), m_depthOffset(depthOffset), m_palette(palette) {
        initialize();
    }

    void initialize() {
        m_nodes.clear();
        m_constraints.clear();

        m_nodes.resize(GRID_WIDTH * GRID_HEIGHT);
        for (int y = 0; y < GRID_HEIGHT; ++y) {
            for (int x = 0; x < GRID_WIDTH; ++x) {
                int index = y * GRID_WIDTH + x;
                m_nodes[index].position = sf::Vector2f(x * CELL_SIZE, y * CELL_SIZE + m_depthOffset);
                m_nodes[index].previousPosition = m_nodes[index].position;
                m_nodes[index].layerIndex = m_layerIndex;

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
                    addConstraint(index, index + 1, false);
                }
                if (y < GRID_HEIGHT - 1) {
                    addConstraint(index, (y + 1) * GRID_WIDTH + x, false);
                }
            }
        }
    }

    void update(float deltaTime, sf::Vector2f mousePosition, float forceMultiplier = 1.0f) {
        for (auto& node : m_nodes) {
            if (!node.isPinned) {
                sf::Vector2f diff = mousePosition - node.position;
                float distance = std::sqrt(diff.x * diff.x + diff.y * diff.y);
                if (distance < MOUSE_FORCE_RADIUS) {
                    sf::Vector2f force = -diff / (distance + 1.0f) * MOUSE_FORCE_STRENGTH * forceMultiplier;
                    node.position += force * deltaTime;
                }
            }
        }

        for (auto& node : m_nodes) {
            if (!node.isPinned) {
                sf::Vector2f velocity = node.position - node.previousPosition;
                node.previousPosition = node.position;
                node.position += velocity * DAMPING_FACTOR; // damping
                node.position += sf::Vector2f(0.0f, 9.8f * deltaTime); // gravity
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

                // Use colors from the palette based on layer index
                sf::Color color = m_palette[m_layerIndex % m_palette.size()];
                color.a = 200; // Set alpha transparency

                m_vertices.append(sf::Vertex(p1, color));
                m_vertices.append(sf::Vertex(p2, color));
            }
        }
        target.draw(m_vertices);
    }

    Node& getNode(int x, int y) {
        return m_nodes[y * GRID_WIDTH + x];
    }

    const std::vector<Node>& getNodes() const {
        return m_nodes;
    }

    void addConstraint(int nodeA, int nodeB, bool isInterLayer) {
        Constraint c;
        c.nodeAIndex = nodeA;
        c.nodeBIndex = nodeB;
        sf::Vector2f delta = m_nodes[nodeB].position - m_nodes[nodeA].position;
        c.length = std::sqrt(delta.x * delta.x + delta.y * delta.y);
        c.isInterLayer = isInterLayer;
        m_constraints.push_back(c);
    }

    void addExternalConstraint(int nodeIndex, const Node& externalNode, float length) {
        Constraint c;
        c.nodeAIndex = nodeIndex;
        sf::Vector2f delta = externalNode.position - m_nodes[nodeIndex].position;
        c.length = length;
        c.isInterLayer = true;
        m_constraints.push_back(c);
    }

private:
    std::vector<Node> m_nodes;
    std::vector<Constraint> m_constraints;
    sf::VertexArray m_vertices;
    int m_layerIndex;
    float m_depthOffset;
    std::vector<sf::Color> m_palette;
};

class MultiLayerFabricSimulation {
public:
    MultiLayerFabricSimulation(const std::vector<sf::Color>& palette) : m_palette(palette) {
        for (int i = 0; i < NUM_LAYERS; ++i) {
            float depthOffset = i * LAYER_DEPTH_OFFSET;
            m_layers.emplace_back(i, depthOffset, m_palette);
        }
        connectAllCorners();
    }

    void initialize() {
        for (auto& layer : m_layers) {
            layer.initialize();
        }
        m_mouseHistory.clear();

        // Reconnect all corners after initialization
        connectAllCorners();
    }

    void connectAllCorners() {
        // Define the four corners of the fabric
        std::vector<std::pair<int, int>> corners = {
            {0, 0},                     // Top-left
            {GRID_WIDTH - 1, 0},        // Top-right
            {0, GRID_HEIGHT - 1},       // Bottom-left
            {GRID_WIDTH - 1, GRID_HEIGHT - 1} // Bottom-right
        };

        // Connect each corner across all layers
        for (const auto& corner : corners) {
            int x = corner.first;
            int y = corner.second;

            // For each corner, connect all layers together
            for (int i = 0; i < NUM_LAYERS - 1; ++i) {
                Node& nodeA = m_layers[i].getNode(x, y);
                Node& nodeB = m_layers[i + 1].getNode(x, y);
                float distance = std::abs(nodeB.position.y - nodeA.position.y);
 
                // Constraints between layers
                m_layers[i].addConstraint(
                    y * GRID_WIDTH + x, 
                    y * GRID_WIDTH + x, 
                    true
                );

                // Reverse constraint to the next layer
                m_layers[i + 1].addConstraint(
                    y * GRID_WIDTH + x, 
                    y * GRID_WIDTH + x, 
                    true
                );

                // Pin corners
                if (i > 0) {
                    nodeB.isPinned = true;
                    nodeB.position = nodeA.position;
                }
            }

            // Pin first layer's corners
            m_layers[0].getNode(x, y).isPinned = true;
        }
    }

    void update(float deltaTime, sf::Vector2f mousePosition) {
        // Store current mouse position in history
        m_mouseHistory.push_back(mousePosition);
        if (m_mouseHistory.size() > MOUSE_HISTORY_SIZE) {
            m_mouseHistory.pop_front();
        }

        // Layer with a delayed mouse position
        for (int i = 0; i < m_layers.size(); ++i) {
            int delayIndex = std::max(0, static_cast<int>(m_mouseHistory.size()) - 1 - i);

            if (delayIndex < m_mouseHistory.size()) {
                sf::Vector2f delayedMousePos = m_mouseHistory[delayIndex];
                float forceMultiplier = 1.0f - (i * 0.15f / m_layers.size());
                m_layers[i].update(deltaTime, delayedMousePos, forceMultiplier);
            } else {
                m_layers[i].update(deltaTime, mousePosition, 1.0f);
            }
        }

        maintainCornerConnections();
    }

    void maintainCornerConnections() {
        std::vector<std::pair<int, int>> corners = {
            {0, 0},                     // Top-left
            {GRID_WIDTH - 1, 0},        // Top-right
            {0, GRID_HEIGHT - 1},       // Bottom-left
            {GRID_WIDTH - 1, GRID_HEIGHT - 1} // Bottom-right
        };

        for (const auto& corner : corners) {
            int x = corner.first;
            int y = corner.second;

            sf::Vector2f basePosition = m_layers[0].getNode(x, y).position;
            for (int i = 1; i < NUM_LAYERS; ++i) {
                Node& node = m_layers[i].getNode(x, y);
                node.position = basePosition;
                node.previousPosition = basePosition;
            }
        }
    }

    void draw(sf::RenderTarget& target) {
        for (auto& layer : m_layers) {
            layer.draw(target);
        }

        drawInterLayerConnections(target);
    }

    void drawInterLayerConnections(sf::RenderTarget& target) {
        sf::VertexArray vertices(sf::Lines);

        std::vector<std::pair<int, int>> corners = {
            {0, 0},                     // Top-left
            {GRID_WIDTH - 1, 0},        // Top-right
            {0, GRID_HEIGHT - 1},       // Bottom-left
            {GRID_WIDTH - 1, GRID_HEIGHT - 1} // Bottom-right
        };

        for (const auto& corner : corners) {
            int x = corner.first;
            int y = corner.second;

            for (int i = 0; i < NUM_LAYERS - 1; ++i) {
                sf::Vector2f pos1 = m_layers[i].getNode(x, y).position;
                sf::Vector2f pos2 = m_layers[i + 1].getNode(x, y).position;

                sf::Color connectionColor = m_palette[(i + 1) % m_palette.size()];
                connectionColor.a = 150; // Semi-transparent

                vertices.append(sf::Vertex(pos1, connectionColor));
                vertices.append(sf::Vertex(pos2, connectionColor));
            }
        }

        target.draw(vertices);
    }

private:
    std::vector<FabricLayer> m_layers;
    std::deque<sf::Vector2f> m_mouseHistory;
    std::vector<sf::Color> m_palette;
};

int main() {
    // Get palette choice from user
    std::string paletteName = getPaletteChoice();
    std::vector<sf::Color> palette = getPalette(paletteName);

    std::cout << "Using palette: " << paletteName << std::endl;

    sf::RenderWindow window(sf::VideoMode(GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE + 100), "Fabric - " + paletteName + " Palette");
    window.setFramerateLimit(60);

    // Load font for UI
    sf::Font font;
    if (!font.loadFromFile("fonts/montana-light.ttf")) {
        std::cerr << "Warning: Could not load font. UI text will not be displayed." << std::endl;
    }

    MultiLayerFabricSimulation fabric(palette);
    sf::Clock clock;
    
    // Initialize GIF recorder
    GIFRecorder gifRecorder(window.getSize().x, window.getSize().y, 300, 30.0f);
    
    // FPS tracking
    sf::Clock fpsClock;
    int frameCount = 0;
    float fps = 0.0f;

    // UI text
    sf::Text instructions;
    instructions.setFont(font);
    instructions.setCharacterSize(16);
    instructions.setFillColor(sf::Color::White);
    instructions.setPosition(10, GRID_HEIGHT * CELL_SIZE + 10);
    
    // FPS text
    sf::Text fpsText;
    fpsText.setFont(font);
    fpsText.setCharacterSize(16);
    fpsText.setFillColor(sf::Color::White);
    fpsText.setPosition(10, GRID_HEIGHT * CELL_SIZE + 30);

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
                    // Create a render texture to capture the current frame
                    sf::RenderTexture renderTexture;
                    if (renderTexture.create(window.getSize().x, window.getSize().y)) {
                        renderTexture.clear(sf::Color::Black);
                        fabric.draw(renderTexture);

                        // Draw UI if font is loaded
                        if (font.getInfo().family != "") {
                            renderTexture.draw(instructions);
                            renderTexture.draw(fpsText);
                        }
                        renderTexture.display();

                        std::time_t now = std::time(nullptr);
                        std::tm* localTime = std::localtime(&now);
                        char filename[100];
                        std::strftime(filename, sizeof(filename), ("fabric_" + paletteName + "_%Y%m%d_%H%M%S.png").c_str(), localTime);
                        if (renderTexture.getTexture().copyToImage().saveToFile(filename)) {
                            std::cout << "Saved image to " << filename << std::endl;
                        } else {
                            std::cerr << "Failed to save image." << std::endl;
                        }
                    }
                }
                // Start/Stop GIF recording on G key press
                if (event.key.code == sf::Keyboard::G) {
                    if (gifRecorder.isRecordingNow()) {
                        gifRecorder.stopRecording();
                    } else {
                        gifRecorder.startRecording();
                    }
                }
            }
        }

        float deltaTime = clock.restart().asSeconds();
        sf::Vector2f mousePosition = static_cast<sf::Vector2f>(sf::Mouse::getPosition(window));

        fabric.update(deltaTime, mousePosition);
        
        // Update GIF recorder
        gifRecorder.update(deltaTime, window);

        // Calculate FPS
        frameCount++;
        if (fpsClock.getElapsedTime().asSeconds() >= 1.0f) {
            fps = frameCount / fpsClock.restart().asSeconds();
            frameCount = 0;
        }

        window.clear(sf::Color::Black);
        fabric.draw(window);
        
        // Update UI text
        std::string instructionStr = "R: Reset | S: Save Image | G: GIF Record | Q: Quit | Layers: " + 
                                    std::to_string(NUM_LAYERS) + " | Palette: " + paletteName;
        if (gifRecorder.isRecordingNow()) {
            instructionStr += " | Recording: " + std::to_string(gifRecorder.getRecordedFrames()) + 
                             "/" + std::to_string(gifRecorder.getMaxFrames());
        }
        instructions.setString(instructionStr);
        
        // Update FPS text
        std::string fpsStr = "FPS: " + std::to_string(static_cast<int>(fps)) + 
                            " | GIF FPS: " + std::to_string(static_cast<int>(gifRecorder.getFPS()));
        fpsText.setString(fpsStr);

        // Draw UI if font is loaded
        if (font.getInfo().family != "") {
            window.draw(instructions);
            window.draw(fpsText);
        }

        window.display();
    }

    return 0;
}
