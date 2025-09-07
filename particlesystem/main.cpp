#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include "text_particle_system.hpp"
#include <iostream>
#include <random>
#include <string>

int main() {
    sf::RenderWindow window(sf::VideoMode(1000, 800), "Text Particles Blow-Off Simulation");
    window.setFramerateLimit(60);

    // Load font
    sf::Font font;
    if (!font.loadFromFile("fonts/montana-bold.ttf")) {
        std::cerr << "Failed to load font!" << std::endl;
        return -1;
    }

    std::string userText;
    std::cout << "Enter text for the particle effect: ";
    std::getline(std::cin, userText);

    if (userText.empty()) {
        userText = "HELLO"; // Default text
    }

    TextParticleSystem textParticles;
    textParticles.setText(userText, font, 72);

    sf::Clock clock;
    bool blowOffTriggered = false;

    sf::Text instructions("Press Q to quit\nPress SPACE to blow off text\nPress R to reset", font, 20);
    instructions.setPosition(20, 20);
    instructions.setFillColor(sf::Color::White);

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            else if (event.type == sf::Event::KeyPressed) {
                // Exit on Q key press
                if (event.key.code == sf::Keyboard::Q) {
                    window.close();
                }
                if (event.key.code == sf::Keyboard::Space && !blowOffTriggered) {
                    // Blow off in a random direction
                    std::random_device rd;
                    std::mt19937 gen(rd());
                    std::uniform_real_distribution<float> dirDist(-1.0f, 1.0f);

                    sf::Vector2f direction(dirDist(gen), dirDist(gen));
                    textParticles.triggerBlowOff(direction, 200.0f);
                    blowOffTriggered = true;
                }
                // Regenerate on R key press
                else if (event.key.code == sf::Keyboard::R) {
                    textParticles.reset();
                    blowOffTriggered = false;
                }
            }
        }

        sf::Time elapsed = clock.restart();
        textParticles.update(elapsed);

        window.clear(sf::Color::Black);
        window.draw(textParticles);
        window.draw(instructions);
        window.display();
    }

    return 0;
}
