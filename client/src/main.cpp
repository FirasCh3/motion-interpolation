#include <SFML/Graphics.hpp>
#include "Game.h"

unsigned SCREEN_WIDTH = 600;
unsigned SCREEN_HEIGHT = 600;

int main() {
    sf::RenderWindow window(
    sf::VideoMode({SCREEN_WIDTH, SCREEN_HEIGHT}), "Motion Interpolation");

    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    window.setPosition({
        int(desktop.size.x / 2 - SCREEN_WIDTH / 2),
        int(desktop.size.y / 2 - SCREEN_HEIGHT / 2)
    });

    Game game(window);
    game.run();

    return 0;
}
