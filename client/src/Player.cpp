#include <iostream>
#include <Player.h>

#include "../../cmake-build-debug/_deps/sfml-src/include/SFML/System/Time.hpp"
#include "SFML/System/Clock.hpp"
#include "SFML/Window/Event.hpp"
using namespace std;
using namespace sf;
RectangleShape Player::shape() {
    return shape_;
}
float Player::speed() {
    return speed_;
}

void Player::movePlayer(float dt) {
    Vector2f offset;
  if (Keyboard::isKeyPressed(Keyboard::Key::W)) {
      offset = {0, -speed_*2*dt};

    shape_.move(offset);
  }else if (Keyboard::isKeyPressed(Keyboard::Key::S)) {
      offset = {0, speed_*2*dt};
      shape_.move(offset);
  }else if (Keyboard::isKeyPressed(Keyboard::Key::D)) {
      offset = {speed_*2*dt, 0};
      shape_.move(offset);
  }else if (Keyboard::isKeyPressed(Keyboard::Key::A)) {
      offset = {-speed_*2*dt, 0};
      shape_.move(offset);
  }
}
