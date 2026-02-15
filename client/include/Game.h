#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "NetworkClient.h"

class Game {
public:
    Game(sf::RenderWindow& window);

    void run();

private:
    sf::RenderWindow& window;
    Player local_player;
    Player remote_player;
    sf::Clock clock;

    sf::Font font;
    sf::Text* local_text;
    sf::Text* remote_text;

    NetworkClient network_client;
    sf::Clock send_timer;
    sf::Time send_interval;

    void handleEvents();
    void update(float dt);
    void render();
};
