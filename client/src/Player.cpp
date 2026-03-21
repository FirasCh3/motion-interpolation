#include "Player.h"
#include <algorithm>
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

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::W) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z))
        offset.y -= speed_ * dt;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
        offset.y += speed_ * dt;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
        offset.x += speed_ * dt;
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::A) ||
        sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
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

void Player::moveSpiral(float dt) {
    elapsed_time_ += dt;

    const float cycle_duration = 9.5f;
    const float turns = 2.9f;
    const float min_radius = 8.f;
    const float phase = std::fmod(elapsed_time_, cycle_duration) / cycle_duration;
    const float path_progress = phase <= 0.5f ? phase * 2.f : (1.f - phase) * 2.f;

    // Ease out so the spiral expands faster near the start, then reuse the same
    // progress in reverse so the player retraces the exact path back inward.
    const float eased_progress = 1.f - std::pow(1.f - path_progress, 2.f);
    const float angle = turns * 2.f * 3.14159265f * eased_progress;
    const float radius = min_radius + (spiral_radius_limit_ - min_radius) * eased_progress;

    const float x = initial_position_.x + radius * std::cos(angle);
    const float y = initial_position_.y + radius * std::sin(angle);
    shape_.setPosition({x, y});
}

void Player::moveSquare(float dt) {
    elapsed_time_ += dt;

    const float side = 220.f;
    const float perimeter = side * 4.f;
    const float loop_time = 6.8f;
    const float distance = std::fmod((elapsed_time_ / loop_time) * perimeter, perimeter);

    sf::Vector2f position = initial_position_;
    if (distance < side) {
        position.x += distance;
    } else if (distance < side * 2.f) {
        position.x += side;
        position.y += distance - side;
    } else if (distance < side * 3.f) {
        position.x += side - (distance - side * 2.f);
        position.y += side;
    } else {
        position.y += side - (distance - side * 3.f);
    }

    shape_.setPosition(position);
}

void Player::moveRemotePlayer(float x, float y)
{
    shape_.setPosition(sf::Vector2f{x, y});
}

void Player::setPathOrigin(const sf::Vector2f& position, bool reset_elapsed)
{
    initial_position_ = position;
    shape_.setPosition(position);
    if (reset_elapsed) {
        elapsed_time_ = 0.f;
    }
}

void Player::setSpiralRadiusLimit(float radius)
{
    spiral_radius_limit_ = std::max(radius, 20.f);
}
