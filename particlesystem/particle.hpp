#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <SFML/Graphics.hpp>
#include <vector>

// Particle struct to hold properties of a single particle
struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
    float lifetime;
};

// ParticleSystem class to manage the collection of particles
class ParticleSystem : public sf::Drawable, public sf::Transformable {
public:
    ParticleSystem(unsigned int count);

    // Creates new particles with random initial properties
    void create(unsigned int count);

    // Update the state of all particles
    void update(sf::Time elapsed);

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    std::vector<Particle> m_particles;
    sf::VertexArray m_vertices;
};

#endif // PARTICLE_HPP
