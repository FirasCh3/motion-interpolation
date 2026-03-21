#include <SFML/Graphics.hpp>
#include "Game.h"

int main(int argc, char** argv) {
    int client_id = 0;
    if (argc > 1) {
        client_id = std::stoi(argv[1]);
    }

    const sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    const unsigned screen_width = static_cast<unsigned>((desktop.size.x / 2.f) * 0.95f);
    const unsigned screen_height = static_cast<unsigned>(desktop.size.y * 0.85f);

    sf::RenderWindow window(
    sf::VideoMode({screen_width, screen_height}), "Motion Interpolation");
    window.setFramerateLimit(60);
    window.setPosition({client_id == 0 ? 0 : int(screen_width), 0});

    Game game(window, client_id);
    game.run();

    return 0;
}
