#ifndef TEXT_PARTICLE_SYSTEM_HPP
#define TEXT_PARTICLE_SYSTEM_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

// A single particle for the text effect
struct TextParticle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f originalPosition;
    sf::Color color;
    float lifetime;
    bool isActive;
};

// The main class to manage the text particles
class TextParticleSystem : public sf::Drawable, public sf::Transformable {
public:
    TextParticleSystem();

    // Sets the text and creates particles from it
    void setText(const std::string& text, const sf::Font& font, unsigned int characterSize);

    // Applies a force to all particles to make them "blow off"
    void triggerBlowOff(const sf::Vector2f& forceDirection, float forceStrength);

    // Updates the position, velocity, and lifetime of each particle
    void update(sf::Time elapsed);

    // Resets the particles back to their original text shape
    void reset();

private:
    // Required draw function for SFML
    virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

    std::vector<TextParticle> m_particles;
    sf::VertexArray m_vertices;
    std::string m_currentText;
    sf::Font m_font;
    unsigned int m_characterSize;
    bool m_blowOffTriggered;
    sf::Vector2f m_forceDirection;
    float m_forceStrength;
};

#endif // TEXT_PARTICLE_SYSTEM_HPP
