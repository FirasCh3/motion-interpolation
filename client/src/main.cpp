#include <iostream>
#include <unistd.h>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>

#include "NetworkClient.h"
#include "Player.h"
unsigned SCREEN_WIDTH = 800;
unsigned SCREEN_HEIGHT = 600;
using namespace std;
using namespace sf;
int main() {
    std::cout << "Process ID: " << getpid() << std::endl;
    RenderWindow window(VideoMode({SCREEN_WIDTH, SCREEN_HEIGHT}), "motion interpolation");
    Player player = Player();
    Player remote_player = Player();
    NetworkClient network_client = NetworkClient();
    network_client.connect();
    Clock game_clock;
    float dt;
    while (window.isOpen()) {
        dt = game_clock.restart().asSeconds();
        while (optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
            }
        }

        window.clear();
        window.draw(player.shape());
        window.draw(remote_player.shape());
        if (window.hasFocus()) {
            player.movePlayer(dt, network_client);
        }
        window.display();
    }
    return 0;
}