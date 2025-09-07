#include "text_particle_system.hpp"
#include <random>
#include <iostream>
#include <cmath>
#include <SFML/Graphics/RenderTexture.hpp>

TextParticleSystem::TextParticleSystem()
    : m_vertices(sf::Points), m_characterSize(48), m_blowOffTriggered(false),
      m_forceDirection(0, 0), m_forceStrength(0) {
}

void TextParticleSystem::setText(const std::string& text, const sf::Font& font, unsigned int characterSize) {
    m_currentText = text;
    m_font = font;
    m_characterSize = characterSize;
    m_particles.clear();
    m_vertices.clear();

    sf::Text sfText(text, font, characterSize);

    sf::RenderTexture renderTexture;
    sf::FloatRect textBounds = sfText.getLocalBounds();
    renderTexture.create(static_cast<unsigned int>(textBounds.width * 2), static_cast<unsigned int>(textBounds.height * 2));
    renderTexture.clear(sf::Color::Transparent);
    sfText.setPosition(renderTexture.getSize().x / 2 - textBounds.width / 2, renderTexture.getSize().y / 2 - textBounds.height / 2); // Center the text on the render texture
    renderTexture.draw(sfText);
    renderTexture.display();

    sf::Image image = renderTexture.getTexture().copyToImage();

    for (unsigned int x = 0; x < image.getSize().x; ++x) {
        for (unsigned int y = 0; y < image.getSize().y; ++y) {
            if (image.getPixel(x, y).a > 0) {
                TextParticle particle;
                particle.position = sf::Vector2f(x, y) + sfText.getPosition();
                particle.velocity = sf::Vector2f(0, 0);
                particle.originalPosition = particle.position;
                particle.color = image.getPixel(x,y);
                particle.lifetime = 4.0f;
                particle.isActive = true;
                m_particles.push_back(particle);
            }
        }
    }

    m_vertices.setPrimitiveType(sf::Points);
    m_vertices.resize(m_particles.size());
}

void TextParticleSystem::triggerBlowOff(const sf::Vector2f& forceDirection, float forceStrength) {
    m_blowOffTriggered = true;
    m_forceDirection = forceDirection;
    m_forceStrength = forceStrength;

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<float> velDist(-2.0f, 2.0f);

    for (size_t i = 0; i < m_particles.size(); ++i) {
        // Initial random velocity to each particle
        m_particles[i].velocity = sf::Vector2f(velDist(gen), velDist(gen)) * m_forceStrength;
        // Reset lifetime
        m_particles[i].lifetime = 4.0f;
    }
}

void TextParticleSystem::update(sf::Time elapsed) {
    float dt = elapsed.asSeconds();
    if (m_blowOffTriggered) {
        for (size_t i = 0; i < m_particles.size(); ++i) {
            if (m_particles[i].isActive) {
                // Position based on velocity
                m_particles[i].position += m_particles[i].velocity * dt;
                // Drag effect to slow particles down
                m_particles[i].velocity *= 0.98f;
                // Lifetime and fade out
                m_particles[i].lifetime -= dt;
                if (m_particles[i].lifetime <= 0) {
                    m_particles[i].isActive = false;
                    continue;
                }
                // Fade out based on lifetime
                sf::Color color = m_particles[i].color;
                color.a = static_cast<sf::Uint8>(255 * (m_particles[i].lifetime / 4.0f));
                m_particles[i].color = color;
            }

            m_vertices[i].position = m_particles[i].position;
            m_vertices[i].color = m_particles[i].color;
        }
    }
}

void TextParticleSystem::reset() {
    m_blowOffTriggered = false;
    for (size_t i = 0; i < m_particles.size(); ++i) {
        m_particles[i].position = m_particles[i].originalPosition;
        m_particles[i].velocity = sf::Vector2f(0, 0);
        m_particles[i].lifetime = 4.0f;
        m_particles[i].isActive = true;
        m_particles[i].color.a = 255;

        m_vertices[i].position = m_particles[i].position;
        m_vertices[i].color = m_particles[i].color;
    }
}

void TextParticleSystem::draw(sf::RenderTarget& target, sf::RenderStates states) const {
    states.transform *= getTransform();
    states.texture = NULL;
    target.draw(m_vertices, states);
}
