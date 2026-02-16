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
      ),
      runButtonText(font, "Run Simulation", 14),
      interpolatorText(font, "Interpolator: Linear", 14),
      intervalText(font, "Send Interval: 1.0s", 14)
{
    if (!font.openFromFile("assets/fonts/ARIAL.TTF")) {
        std::cerr << "Failed to load font\n";
    }

    local_text = new sf::Text(font, "Local", 14);
    local_text->setFillColor(sf::Color::White);

    remote_text = new sf::Text(font, "Remote", 14);
    remote_text->setFillColor(sf::Color::White);

    runButtonText.setFillColor(sf::Color::White);
    runButtonText.setPosition({20.f, 15.f});

    interpolatorText.setFillColor(sf::Color::White);
    interpolatorText.setPosition({180.f, 15.f});

    intervalText.setFillColor(sf::Color::White);
    intervalText.setPosition({350.f, 15.f});

    runButton.setSize({150.f, 30.f});
    runButton.setPosition({10.f, 10.f});
    runButton.setFillColor(sf::Color(60, 60, 60));

    interpolatorBox.setSize({160.f, 30.f});
    interpolatorBox.setPosition({170.f, 10.f});
    interpolatorBox.setFillColor(sf::Color(60, 60, 60));

    intervalBox.setSize({170.f, 30.f});
    intervalBox.setPosition({340.f, 10.f});
    intervalBox.setFillColor(sf::Color(60, 60, 60));

    interpolatorIndex = 0;
    intervalIndex = 0;

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

        if (const auto* mouse = event->getIf<sf::Event::MouseButtonPressed>()) {
            sf::Vector2f mpos(mouse->position.x, mouse->position.y);

            if (runButton.getGlobalBounds().contains(mpos)) {
                runButton.setFillColor(sf::Color(100, 100, 100));
                // trigger sinusoidal movement for testing
            }

            if (interpolatorBox.getGlobalBounds().contains(mpos)) {
                // change interpolator
                interpolatorIndex = (interpolatorIndex + 1) % 3;
                if (interpolatorIndex == 0)
                    interpolatorText.setString("Interpolator: Linear");
                if (interpolatorIndex == 1)
                    interpolatorText.setString("Interpolator: Lagrange");
                if (interpolatorIndex == 2)
                    interpolatorText.setString("Interpolator: Bezier");
            }

            if (intervalBox.getGlobalBounds().contains(mpos)) {
                // change send interval
                intervalIndex = (intervalIndex + 1) % 3;

                if (intervalIndex == 0)
                    intervalText.setString("Send: 1.0s");
                if (intervalIndex == 1)
                    intervalText.setString("Send: 0.5s");
                if (intervalIndex == 2)
                    intervalText.setString("Send: 0.1s");
            }
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

    window.draw(runButton);
    window.draw(runButtonText);

    window.draw(interpolatorBox);
    window.draw(interpolatorText);

    window.draw(intervalBox);
    window.draw(intervalText);

    window.display();
}
