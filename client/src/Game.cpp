#include "Game.h"
#include <iostream>

Game::Game(sf::RenderWindow& window)
    : window(window),
      local_player(sf::Color::Green, {100.f, 300.f}),
      remote_player(sf::Color::Red, {100.f, 200.f})
{
    if (!font.openFromFile("assets/fonts/ARIAL.TTF")) {
        std::cerr << "Failed to load font\n";
    }

    local_text = new sf::Text(font, "Local", 14);
    local_text->setFillColor(sf::Color::White);

    remote_text = new sf::Text(font, "Remote", 14);
    remote_text->setFillColor(sf::Color::White);

    network_client.connect();
    send_interval = sf::seconds(1.f);
    send_timer.restart();
}


void Game::run() {
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();
        handleEvents();
        update(dt);
        render();
    }
}

void Game::handleEvents() {
    while (auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }
    }
}

void Game::update(float dt) {
    if (window.hasFocus()) {
        local_player.movePlayer(dt);
    }

    network_client.receive_data(remote_player);
    if (send_timer.getElapsedTime() >= send_interval) {
        network_client.send_data(local_player.shape().getPosition());
        send_timer.restart();
    }
}

void Game::render() {
    window.clear();

    sf::Vector2f local_pos = local_player.shape().getPosition();
    local_text->setPosition({local_pos.x - 10.f, local_pos.y - 20.f});

    sf::Vector2f remote_pos = remote_player.shape().getPosition();
    remote_text->setPosition({remote_pos.x - 10.f, remote_pos.y - 20.f});

    window.draw(local_player.shape());
    window.draw(remote_player.shape());
    window.draw(*local_text);
    window.draw(*remote_text);

    window.display();
}
