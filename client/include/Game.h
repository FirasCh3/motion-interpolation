#include <SFML/Graphics.hpp>
#include "Player.h"
#include "NetworkClient.h"

class Game {
private:
    sf::RenderWindow& window;
    Player local_player;
    Player remote_player;
    NetworkClient network_client;
    sf::Clock clock;

    sf::Font font;
    sf::Text* local_text;
    sf::Text* remote_text;

    void handleEvents();
    void update(float dt);
    void render();

public:
    Game(sf::RenderWindow& window);
    void run();
};
