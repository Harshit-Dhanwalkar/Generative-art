#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <SFML/Graphics.hpp>
#include <vector>

struct Particle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Color color;
    float lifetime;
};

class ParticleSystem : public sf::Drawable, public sf::Transformable {
public:
    ParticleSystem(unsigned int count);
    void create(unsigned int count);
    void update(sf::Time elapsed);

private:
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    std::vector<Particle> m_particles;
    sf::VertexArray m_vertices;
};

#endif // PARTICLE_HPP
