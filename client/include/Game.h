#pragma once
#include <SFML/Graphics.hpp>
#include "Player.h"
#include "NetworkClient.h"
#include <deque>

class Game {
public:
    Game(sf::RenderWindow& window, int client_id);

    struct Sample {
        sf::Vector2f position;
        float time;
    };

    sf::Clock frame_clock;
    sf::Clock net_clock;
    float interpolation_delay;
    std::deque<Sample> remote_buffer;

    void interpolate_remote();
    void run();

private:
    sf::RenderWindow& window;
    int client_id;
    
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
