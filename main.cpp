#include <SFML/Graphics.hpp>
#include <random>

int main() {
    // Window dimensions
    const int windowWidth = 1000;
    const int windowHeight = 1000;

    // Create the main window
    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "Generative Art", sf::Style::Close);

    // Create a render texture for off-screen drawing
    sf::RenderTexture renderTexture;
    if (!renderTexture.create(windowWidth, windowHeight)) {
        return -1;
    }
    renderTexture.clear(sf::Color::Black); // Or any background color

    // Set up random number generation
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> posX(0.0, static_cast<double>(windowWidth));
    std::uniform_real_distribution<> posY(0.0, static_cast<double>(windowHeight));
    std::uniform_real_distribution<> size(5.0, 150.0);
    std::uniform_int_distribution<> colorVal(0, 255);
    std::uniform_int_distribution<> alphaVal(20, 120); // transparency
    std::uniform_real_distribution<> rotationAngle(0.0, 360.0);

    const int numberOfRects = 5000; // density

    // Loop to draw the rectangles
    for (int i = 0; i < numberOfRects; ++i) {
        sf::RectangleShape rect(sf::Vector2f(size(gen), size(gen)));
        
        // Set position, color, and alpha
        rect.setPosition(posX(gen), posY(gen));
        rect.setFillColor(sf::Color(colorVal(gen), colorVal(gen), colorVal(gen), alphaVal(gen)));
        
        // Add a black border to the rectangle
        rect.setOutlineColor(sf::Color::Black);
        rect.setOutlineThickness(1.0f); // 1px border
        
        // Set a random rotation and rotate around the center
        rect.setRotation(rotationAngle(gen));
        rect.setOrigin(rect.getLocalBounds().width / 2, rect.getLocalBounds().height / 2);

        // Draw the rectangle to the off-screen texture
        renderTexture.draw(rect);
    }
    
    // Finalize the drawing process on the render texture
    renderTexture.display();

    // Save the final image to a file
    renderTexture.getTexture().copyToImage().saveToFile("generated_art.png");

    // Main program loop to display the image
    sf::Sprite sprite(renderTexture.getTexture());
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
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
