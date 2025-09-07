#include "particle.hpp"
#include <random>

ParticleSystem::ParticleSystem(unsigned int count)
    : m_vertices(sf::Points, count) {
    m_particles.resize(count);
    create(count);
}

void ParticleSystem::create(unsigned int count) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> velDist(-100.0f, 100.0f);
    std::uniform_real_distribution<float> lifeDist(0.5f, 2.5f);
    std::uniform_int_distribution<int> colorDist(150, 255);

    // Initial position for the blow-off effect
    sf::Vector2f center(500.0f, 400.0f);

    for (unsigned int i = 0; i < count; ++i) {
        // Random initial velocity, lifetime, and color
        m_particles[i].velocity = sf::Vector2f(velDist(gen), velDist(gen));
        m_particles[i].lifetime = lifeDist(gen);
        m_particles[i].position = center;
        m_particles[i].color = sf::Color(colorDist(gen), colorDist(gen), colorDist(gen));

        // Vertex position and color
        m_vertices[i].position = m_particles[i].position;
        m_vertices[i].color = m_particles[i].color;
    }
}

void ParticleSystem::update(sf::Time elapsed) {
    float dt = elapsed.asSeconds();

    for (unsigned int i = 0; i < m_particles.size(); ++i) {
        // Particle velocity and position
        m_particles[i].position += m_particles[i].velocity * dt;
        // Deceleration force
        m_particles[i].velocity *= 0.99f; // Simple drag
        // Particle lifetime
        m_particles[i].lifetime -= dt;
        // Vertex position
        m_vertices[i].position = m_particles[i].position;
        // Alpha based on lifetime to create a fading effect
        sf::Color color = m_particles[i].color;
        color.a = static_cast<sf::Uint8>(255 * (m_particles[i].lifetime / 2.5f));
        m_vertices[i].color = color;

        if (m_particles[i].lifetime <= 0) {
            std::random_device rd;
            std::mt19937 gen(rd());
            std::uniform_real_distribution<float> velDist(-100.0f, 100.0f);
            std::uniform_real_distribution<float> lifeDist(0.5f, 2.5f);
            std::uniform_int_distribution<int> colorDist(150, 255);

            sf::Vector2f center(500.0f, 400.0f);
            m_particles[i].velocity = sf::Vector2f(velDist(gen), velDist(gen));
            m_particles[i].lifetime = lifeDist(gen);
            m_particles[i].position = center;
            m_particles[i].color = sf::Color(colorDist(gen), colorDist(gen), colorDist(gen));
        }
    }
}

void ParticleSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    states.texture = NULL;
    target.draw(m_vertices, states);
}
