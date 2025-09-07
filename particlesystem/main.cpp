#include <SFML/Graphics.hpp>
#include "particle.hpp"

int main() {
    // Main window
    sf::RenderWindow window(sf::VideoMode(1000, 800), "Particle Blow-Off Simulation");
    window.setFramerateLimit(60);

    // Particle system with 50,000 particles
    ParticleSystem particles(50000);

    // Clock for consistent time-based movement
    sf::Clock clock;

    // Main game loop
    while (window.isOpen()) {
        // Event handling
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
        }

        // Get the time elapsed since the last frame
        sf::Time elapsed = clock.restart();

        // Update the particle system
        particles.update(elapsed);

        // Clear the window with a dark background
        window.clear(sf::Color::Black);

        // Draw the particle system to the window
        window.draw(particles);

        // Display the window content
        window.display();
    }

    return 0;
}
