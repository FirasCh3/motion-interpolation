#include "Player.h"
#include <SFML/Window/Keyboard.hpp>
#include <cmath>

Player::Player(const sf::Color& color,
               const sf::Vector2f& position,
               float speed)
    : speed_(speed), initial_position_(position){

    shape_.setRadius(10.f);
    shape_.setFillColor(color);
    shape_.setPosition(position);
}

const sf::CircleShape& Player::shape() const {
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

void Player::moveSinusoidal(float dt) {
    elapsed_time_ += dt;

    const float amplitude_y = 50.f;
    const float horizontal_distance = 500.f;
    const float period = 10.f;

    float t = fmod(elapsed_time_, period) / period;  

    float factor = (t < 0.5f) ? (t * 2.f) : ((1.f - t) * 2.f);  
    float x = initial_position_.x + factor * horizontal_distance;
    float y = initial_position_.y + amplitude_y * std::sin(8.f * 3.14159265f * factor);

    shape_.setPosition({x, y});
}

void Player::moveRemotePlayer(float x, float y)
{
    shape_.setPosition(sf::Vector2f{x, y});
}