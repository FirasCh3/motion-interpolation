#include <iostream>
#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;
int main() {
    RenderWindow window(VideoMode({1920, 1080}), "motion interpolation");
    RectangleShape player({50, 50});
    player.setFillColor(Color::White);
    player.setPosition({10, 20});
    while (window.isOpen()) {
        while (optional event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                window.close();
            }
        }
        window.clear();
        window.draw(player);
        window.display();
    }
    return 0;
}