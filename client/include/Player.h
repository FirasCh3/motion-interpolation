#ifndef CLIENT_PLAYER_H
#define CLIENT_PLAYER_H
#include "NetworkClient.h"
#include "SFML/Graphics/RectangleShape.hpp"
#include "SFML/System/Vector2.hpp"
using namespace sf;
class Player{
public:
    Player(): speed_(200) {
        shape_.setFillColor(Color::White);
        shape_.setSize({50, 50});
        shape_.setPosition({20, 20});
    }
    RectangleShape shape();
    float speed();
    void movePlayer(float dt, NetworkClient& network_client);


private:
    RectangleShape shape_;
    float speed_;
};
#endif //CLIENT_PLAYER_H
