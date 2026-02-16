#include "Game.h"
#include <iostream>

Game::Game(sf::RenderWindow& window, int client_id)
    : window(window),
      client_id(client_id),
      local_player(
          client_id == 0 ? sf::Color::Green : sf::Color::Red,
          client_id == 0 ? sf::Vector2f{100.f, 300.f}
                         : sf::Vector2f{100.f, 100.f}
      ),
      remote_player(
          client_id == 0 ? sf::Color::Red : sf::Color::Green,
          client_id == 0 ? sf::Vector2f{100.f, 100.f}
                         : sf::Vector2f{100.f, 300.f}
      )
{
    if (!font.openFromFile("assets/fonts/ARIAL.TTF")) {
        std::cerr << "Failed to load font\n";
    }

    local_text = new sf::Text(font, "Local", 14);
    local_text->setFillColor(sf::Color::White);

    remote_text = new sf::Text(font, "Remote", 14);
    remote_text->setFillColor(sf::Color::White);

    network_client.connect();
    send_interval = sf::seconds(0.5f);
    send_timer.restart();
    interpolation_delay = send_interval.asSeconds() * 1.1f;
}


void Game::run() {
    while (window.isOpen()) {
        float dt = frame_clock.restart().asSeconds();
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

    sf::Vector2f received_pos;
    if (network_client.receive_data(received_pos))
    {
        float now = net_clock.getElapsedTime().asSeconds();

        remote_buffer.push_back({received_pos, now});

        if (remote_buffer.size() > 10)
            remote_buffer.pop_front();
    }
    interpolate_remote();

    if (send_timer.getElapsedTime() >= send_interval) {
        network_client.send_data(local_player.shape().getPosition());
        send_timer.restart();
    }
}

float clamp(float v, float min, float max)
{
    if (v < min) return min;
    if (v > max) return max;
    return v;
}

void Game::interpolate_remote()
{
    if (remote_buffer.size() < 2)
        return;

    float render_time = net_clock.getElapsedTime().asSeconds() - interpolation_delay;

    while (remote_buffer.size() >= 2 &&
           remote_buffer[1].time <= render_time)
    {
        remote_buffer.pop_front();
    }

    if (remote_buffer.size() < 2)
        return;

    const Sample& a = remote_buffer[0];
    const Sample& b = remote_buffer[1];

    float t = (render_time - a.time) / (b.time - a.time);
    t = clamp(t, 0.f, 1.f);

    sf::Vector2f interpolated =
        a.position + t * (b.position - a.position);

    remote_player.moveRemotePlayer(interpolated.x, interpolated.y);
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
