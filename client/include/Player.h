#ifndef CLIENT_PLAYER_H
#define CLIENT_PLAYER_H

#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/System/Vector2.hpp>

class Player {
public:
    Player(const sf::Color& color,
           const sf::Vector2f& position,
           float speed = 200.f);

    sf::CircleShape shape() const;
    float speed() const;

    void movePlayer(float dt);

private:
    sf::CircleShape shape_;
    float speed_;
};

#endif // CLIENT_PLAYER_H
