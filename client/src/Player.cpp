#include "Player.h"
#include <SFML/Window/Keyboard.hpp>

Player::Player(const sf::Color& color,
               const sf::Vector2f& position,
               float speed)
    : speed_(speed) {

    shape_.setRadius(10.f);
    shape_.setFillColor(color);
    shape_.setPosition(position);
}

sf::CircleShape Player::shape() const {
    return shape_;
}

float Player::speed() const {
    return speed_;
}

void Player::movePlayer(float dt) {
    sf::Vector2f offset(0.f, 0.f);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W))
        offset.y -= speed_ * dt;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        offset.y += speed_ * dt;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        offset.x += speed_ * dt;
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A))
        offset.x -= speed_ * dt;

    shape_.move(offset);
}

void Player::moveRemotePlayer(float x, float y)
{
    shape_.setPosition(sf::Vector2f{x, y});
}