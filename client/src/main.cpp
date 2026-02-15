#include <SFML/Graphics.hpp>
#include "Game.h"

unsigned SCREEN_WIDTH = 600;
unsigned SCREEN_HEIGHT = 600;

int main(int argc, char** argv) {
    int client_id = 0;
    if (argc > 1) {
        client_id = std::stoi(argv[1]);
    }

    sf::RenderWindow window(
    sf::VideoMode({SCREEN_WIDTH, SCREEN_HEIGHT}), "Motion Interpolation");

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.setPosition({
        int(desktop.size.x / 2 - SCREEN_WIDTH / 2),
        int(desktop.size.y / 2 - SCREEN_HEIGHT / 2)
    });

    Game game(window, client_id);
    game.run();

    return 0;
}
