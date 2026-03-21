#include "Game.h"

#include <algorithm>
#include <iostream>
#include <thread>

namespace {
constexpr float kButtonWidth = 120.f;
constexpr float kButtonHeight = 36.f;
constexpr float kButtonSpacing = 12.f;
constexpr float kSmallButtonWidth = 40.f;
constexpr float kCountBoxWidth = 56.f;
constexpr float kPathButtonWidth = 110.f;
constexpr float kPanelTop = 16.f;
constexpr float kPanelLeft = 16.f;
constexpr float kPanelWidth = 740.f;
constexpr float kTrailLifetimeSeconds = 2.8f;
constexpr int kMinPolynomialSamples = 2;
constexpr int kMaxPolynomialSamples = 6;
}

Game::Game(sf::RenderWindow& window, int client_id)
    : window(window),
      client_id(client_id),
      local_player(
          client_id == 0 ? sf::Color::Green : sf::Color::Red,
          client_id == 0 ? sf::Vector2f{100.f, 520.f}
                         : sf::Vector2f{100.f, 320.f}
      ),
      remote_player(
          client_id == 0 ? sf::Color::Red : sf::Color::Green,
          client_id == 0 ? sf::Vector2f{100.f, 320.f}
                         : sf::Vector2f{100.f, 520.f}
      )
{
    if (!font.openFromFile("assets/fonts/ARIAL.TTF")) {
        std::cerr << "Failed to load font\n";
    }

    local_text = std::make_unique<sf::Text>(font, "Local", 14);
    local_text->setFillColor(sf::Color::White);

    remote_text = std::make_unique<sf::Text>(font, "Remote", 14);
    remote_text->setFillColor(sf::Color::White);

    title_text = std::make_unique<sf::Text>(font, "Interpolation Controls", 20);
    title_text->setFillColor(sf::Color(240, 240, 240));
    title_text->setPosition({kPanelLeft, kPanelTop});

    status_text = std::make_unique<sf::Text>(font, "", 16);
    status_text->setFillColor(sf::Color(220, 220, 220));
    status_text->setPosition({kPanelLeft, kPanelTop + 148.f});

    path_label_text = std::make_unique<sf::Text>(font, "Path", 13);
    path_label_text->setFillColor(sf::Color(190, 190, 205));
    path_label_text->setPosition({kPanelLeft, kPanelTop + 86.f});

    sample_label_text = std::make_unique<sf::Text>(font, "Samples", 13);
    sample_label_text->setFillColor(sf::Color(190, 190, 205));
    sample_label_text->setPosition({kPanelLeft + 560.f, kPanelTop + 28.f});

    sample_count_text = std::make_unique<sf::Text>(font, "", 15);
    sample_count_text->setFillColor(sf::Color::White);

    network_client.connect();
    send_interval = sf::milliseconds(250);
    send_timer.restart();
    interpolation_delay = 0.6f;
    configureLocalPath(true);
    resetTrails(0.f);

    initializeUi();
    updateButtonStyles();
}

void Game::run() {
    while (window.isOpen()) {
        if (!window.hasFocus()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }

        const float dt = frame_clock.restart().asSeconds();
        handleEvents();
        update(dt);
        render();
    }
}

void Game::initializeUi() {
    const float button_top = kPanelTop + 44.f;
    const float path_top = kPanelTop + 104.f;

    linear_button.shape.setSize({kButtonWidth, kButtonHeight});
    linear_button.shape.setPosition({kPanelLeft, button_top});
    linear_button.label = std::make_unique<sf::Text>(font, "Linear", 15);
    linear_button.label->setPosition({kPanelLeft + 22.f, button_top + 8.f});

    lagrange_button.shape.setSize({kButtonWidth, kButtonHeight});
    lagrange_button.shape.setPosition({kPanelLeft + kButtonWidth + kButtonSpacing, button_top});
    lagrange_button.label = std::make_unique<sf::Text>(font, "Lagrange", 15);
    lagrange_button.label->setPosition({kPanelLeft + kButtonWidth + kButtonSpacing + 12.f, button_top + 8.f});

    newton_button.shape.setSize({kButtonWidth, kButtonHeight});
    newton_button.shape.setPosition({kPanelLeft + 2.f * (kButtonWidth + kButtonSpacing), button_top});
    newton_button.label = std::make_unique<sf::Text>(font, "Newton", 15);
    newton_button.label->setPosition({kPanelLeft + 2.f * (kButtonWidth + kButtonSpacing) + 18.f, button_top + 8.f});

    simulation_button.shape.setSize({130.f, kButtonHeight});
    simulation_button.shape.setPosition({kPanelLeft + 3.f * (kButtonWidth + kButtonSpacing), button_top});
    simulation_button.label = std::make_unique<sf::Text>(font, "", 15);
    simulation_button.label->setPosition({kPanelLeft + 3.f * (kButtonWidth + kButtonSpacing) + 14.f, button_top + 8.f});

    sinusoidal_button.shape.setSize({kPathButtonWidth, kButtonHeight});
    sinusoidal_button.shape.setPosition({kPanelLeft, path_top});
    sinusoidal_button.label = std::make_unique<sf::Text>(font, "Sinusoidal", 14);
    sinusoidal_button.label->setPosition({kPanelLeft + 12.f, path_top + 9.f});

    spiral_button.shape.setSize({kPathButtonWidth, kButtonHeight});
    spiral_button.shape.setPosition({kPanelLeft + kPathButtonWidth + kButtonSpacing, path_top});
    spiral_button.label = std::make_unique<sf::Text>(font, "Spiral", 14);
    spiral_button.label->setPosition({kPanelLeft + kPathButtonWidth + kButtonSpacing + 29.f, path_top + 9.f});

    square_button.shape.setSize({kPathButtonWidth, kButtonHeight});
    square_button.shape.setPosition({kPanelLeft + 2.f * (kPathButtonWidth + kButtonSpacing), path_top});
    square_button.label = std::make_unique<sf::Text>(font, "Square", 14);
    square_button.label->setPosition({kPanelLeft + 2.f * (kPathButtonWidth + kButtonSpacing) + 26.f, path_top + 9.f});

    sample_decrease_button.shape.setSize({kSmallButtonWidth, kButtonHeight});
    sample_decrease_button.shape.setPosition({kPanelLeft + 560.f, button_top});
    sample_decrease_button.label = std::make_unique<sf::Text>(font, "-", 20);
    sample_decrease_button.label->setPosition({kPanelLeft + 575.f, button_top + 4.f});

    sample_count_box.setSize({kCountBoxWidth, kButtonHeight});
    sample_count_box.setPosition({kPanelLeft + 606.f, button_top});
    sample_count_box.setFillColor(sf::Color(38, 38, 44));
    sample_count_box.setOutlineThickness(2.f);
    sample_count_box.setOutlineColor(sf::Color(92, 92, 92));

    sample_increase_button.shape.setSize({kSmallButtonWidth, kButtonHeight});
    sample_increase_button.shape.setPosition({kPanelLeft + 670.f, button_top});
    sample_increase_button.label = std::make_unique<sf::Text>(font, "+", 20);
    sample_increase_button.label->setPosition({kPanelLeft + 684.f, button_top + 4.f});
}

bool Game::contains(const Button& button, sf::Vector2f point) {
    return button.shape.getGlobalBounds().contains(point);
}

void Game::configureLocalPath(bool reset_motion) {
    sf::Vector2f origin = client_id == 0 ? sf::Vector2f{100.f, 520.f}
                                         : sf::Vector2f{100.f, 320.f};
    float spiral_radius_limit = 180.f;

    if (movement_path == MovementPath::Spiral) {
        const sf::Vector2u window_size = window.getSize();
        const float player_radius = local_player.shape().getRadius();
        origin = {
            window_size.x * 0.5f - player_radius,
            window_size.y * 0.5f - player_radius
        };
        spiral_radius_limit = std::min(window_size.x, window_size.y) * 0.32f;
    }

    local_player.setSpiralRadiusLimit(spiral_radius_limit);
    local_player.setPathOrigin(origin, reset_motion);
}

void Game::resetTrails(float now) {
    local_trail.clear();
    remote_trail.clear();
    local_trail.push_back({local_player.shape().getPosition(), now});
    remote_trail.push_back({remote_player.shape().getPosition(), now});
}

void Game::handleMouseClick(sf::Vector2f mouse_position) {
    if (contains(linear_button, mouse_position)) {
        interpolation_method = InterpolationMethod::Linear;
    } else if (contains(lagrange_button, mouse_position)) {
        interpolation_method = InterpolationMethod::Lagrange;
    } else if (contains(newton_button, mouse_position)) {
        interpolation_method = InterpolationMethod::Newton;
    } else if (contains(simulation_button, mouse_position)) {
        simulation_running = !simulation_running;
    } else if (contains(sinusoidal_button, mouse_position)) {
        movement_path = MovementPath::Sinusoidal;
        configureLocalPath(true);
        resetTrails(net_clock.getElapsedTime().asSeconds());
    } else if (contains(spiral_button, mouse_position)) {
        movement_path = MovementPath::Spiral;
        configureLocalPath(true);
        resetTrails(net_clock.getElapsedTime().asSeconds());
    } else if (contains(square_button, mouse_position)) {
        movement_path = MovementPath::Square;
        configureLocalPath(true);
        resetTrails(net_clock.getElapsedTime().asSeconds());
    } else if (contains(sample_decrease_button, mouse_position)) {
        polynomial_sample_count = std::max(kMinPolynomialSamples, polynomial_sample_count - 1);
    } else if (contains(sample_increase_button, mouse_position)) {
        polynomial_sample_count = std::min(kMaxPolynomialSamples, polynomial_sample_count + 1);
    }

    updateButtonStyles();
}

void Game::updateButtonStyles() {
    const auto set_button_style =
        [](Button& button, bool active, const sf::Color& active_color, const std::string& label_text) {
            button.shape.setFillColor(active ? active_color : sf::Color(48, 48, 48));
            button.shape.setOutlineThickness(2.f);
            button.shape.setOutlineColor(active ? sf::Color::White : sf::Color(92, 92, 92));
            button.label->setString(label_text);
            button.label->setFillColor(sf::Color::White);
        };

    set_button_style(linear_button,
                     interpolation_method == InterpolationMethod::Linear,
                     sf::Color(36, 120, 184),
                     "Linear");
    set_button_style(lagrange_button,
                     interpolation_method == InterpolationMethod::Lagrange,
                     sf::Color(160, 108, 34),
                     "Lagrange");
    set_button_style(newton_button,
                     interpolation_method == InterpolationMethod::Newton,
                     sf::Color(47, 143, 92),
                     "Newton");
    set_button_style(simulation_button,
                     simulation_running,
                     simulation_running ? sf::Color(182, 72, 60) : sf::Color(60, 140, 86),
                     simulation_running ? "Pause Test" : "Start Test");
    set_button_style(sinusoidal_button,
                     movement_path == MovementPath::Sinusoidal,
                     sf::Color(126, 92, 168),
                     "Sinusoidal");
    set_button_style(spiral_button,
                     movement_path == MovementPath::Spiral,
                     sf::Color(205, 98, 52),
                     "Spiral");
    set_button_style(square_button,
                     movement_path == MovementPath::Square,
                     sf::Color(112, 132, 52),
                     "Square");
    set_button_style(sample_decrease_button, false, sf::Color(48, 48, 48), "-");
    set_button_style(sample_increase_button, false, sf::Color(48, 48, 48), "+");
    sample_count_box.setOutlineColor(sf::Color(92, 92, 92));

    std::string method_name = "Linear";
    if (interpolation_method == InterpolationMethod::Lagrange) {
        method_name = "Lagrange";
    } else if (interpolation_method == InterpolationMethod::Newton) {
        method_name = "Newton";
    }

    std::string path_name = "Sinusoidal";
    if (movement_path == MovementPath::Spiral) {
        path_name = "Spiral";
    } else if (movement_path == MovementPath::Square) {
        path_name = "Square";
    }

    status_text->setString("Mode: " + method_name +
                           "    Path: " + path_name +
                           "    Test: " + (simulation_running ? "Running" : "Paused"));
    sample_count_text->setString(std::to_string(polynomial_sample_count));
    sample_count_text->setPosition({kPanelLeft + 627.f, kPanelTop + 52.f});
}

void Game::handleEvents() {
    while (auto event = window.pollEvent()) {
        if (event->is<sf::Event::Closed>()) {
            window.close();
        }

        if (const auto* mouse_pressed = event->getIf<sf::Event::MouseButtonPressed>()) {
            if (mouse_pressed->button == sf::Mouse::Button::Left) {
                handleMouseClick(window.mapPixelToCoords(mouse_pressed->position));
            }
        }
    }
}

void Game::updateRemotePlayer(float render_time) {
    if (remote_buffer.size() < 2) {
        return;
    }

    while (remote_buffer.size() >= 2 && remote_buffer[1].time <= render_time) {
        remote_buffer.pop_front();
    }

    if (remote_buffer.empty()) {
        return;
    }

    const sf::Vector2f interpolated_position = interpolatePosition(render_time);
    remote_player.moveRemotePlayer(interpolated_position.x, interpolated_position.y);
}

void Game::updateTrail(std::deque<TrailPoint>& trail, sf::Vector2f position, float now) {
    if (trail.empty()) {
        trail.push_back({position, now});
        return;
    }

    if (trail.back().position != position) {
        trail.push_back({position, now});
    } else {
        trail.back().time = now;
    }
}

void Game::pruneTrail(std::deque<TrailPoint>& trail, float now) const {
    while (trail.size() > 1 && now - trail.front().time > kTrailLifetimeSeconds) {
        trail.pop_front();
    }
}

void Game::drawTrail(const std::deque<TrailPoint>& trail, const sf::Color& color, float now) {
    if (trail.size() < 2) {
        return;
    }

    sf::VertexArray vertices(sf::PrimitiveType::LineStrip, trail.size());
    for (std::size_t i = 0; i < trail.size(); ++i) {
        const float age = now - trail[i].time;
        const float fade = std::clamp(1.f - age / kTrailLifetimeSeconds, 0.f, 1.f);
        vertices[i].position = trail[i].position + sf::Vector2f(10.f, 10.f);
        vertices[i].color = sf::Color(color.r, color.g, color.b, static_cast<std::uint8_t>(fade * 180.f));
    }

    window.draw(vertices);
}

sf::Vector2f Game::interpolatePosition(float render_time) {
    switch (interpolation_method) {
        case InterpolationMethod::Linear:
            return interpolation_.lerp(remote_buffer, render_time);
        case InterpolationMethod::Lagrange:
            return interpolation_.lagrange(remote_buffer, render_time, polynomial_sample_count);
        case InterpolationMethod::Newton:
            return interpolation_.newton(remote_buffer, render_time, polynomial_sample_count);
    }

    return remote_buffer.back().position;
}

void Game::update(float dt) {
    const float now = net_clock.getElapsedTime().asSeconds();

    if (window.hasFocus()) {
        if (simulation_running) {
            switch (movement_path) {
                case MovementPath::Sinusoidal:
                    local_player.moveSinusoidal(dt);
                    break;
                case MovementPath::Spiral:
                    local_player.moveSpiral(dt);
                    break;
                case MovementPath::Square:
                    local_player.moveSquare(dt);
                    break;
            }
        } else {
            local_player.movePlayer(dt);
        }
    }

    sf::Vector2f received_pos;
    if (network_client.receive_data(received_pos)) {
        remote_buffer.push_back({received_pos, now});
    }

    const float render_time = now - interpolation_delay;
    updateRemotePlayer(render_time);
    updateTrail(local_trail, local_player.shape().getPosition(), now);
    updateTrail(remote_trail, remote_player.shape().getPosition(), now);
    pruneTrail(local_trail, now);
    pruneTrail(remote_trail, now);

    if (send_timer.getElapsedTime() >= send_interval) {
        network_client.send_data(local_player.shape().getPosition());
        send_timer.restart();
    }
}

void Game::render() {
    window.clear(sf::Color(24, 24, 28));

    sf::RectangleShape panel({kPanelWidth, 178.f});
    panel.setPosition({8.f, 8.f});
    panel.setFillColor(sf::Color(30, 30, 36, 230));
    panel.setOutlineThickness(2.f);
    panel.setOutlineColor(sf::Color(75, 75, 90));
    window.draw(panel);

    const sf::Vector2f local_pos = local_player.shape().getPosition();
    local_text->setPosition({local_pos.x - 10.f, local_pos.y - 20.f});

    const sf::Vector2f remote_pos = remote_player.shape().getPosition();
    remote_text->setPosition({remote_pos.x - 10.f, remote_pos.y - 20.f});

    window.draw(*title_text);
    window.draw(*status_text);
    window.draw(linear_button.shape);
    window.draw(*linear_button.label);
    window.draw(lagrange_button.shape);
    window.draw(*lagrange_button.label);
    window.draw(newton_button.shape);
    window.draw(*newton_button.label);
    window.draw(simulation_button.shape);
    window.draw(*simulation_button.label);
    window.draw(*sample_label_text);
    window.draw(sample_decrease_button.shape);
    window.draw(*sample_decrease_button.label);
    window.draw(sample_count_box);
    window.draw(*sample_count_text);
    window.draw(sample_increase_button.shape);
    window.draw(*sample_increase_button.label);
    window.draw(*path_label_text);
    window.draw(sinusoidal_button.shape);
    window.draw(*sinusoidal_button.label);
    window.draw(spiral_button.shape);
    window.draw(*spiral_button.label);
    window.draw(square_button.shape);
    window.draw(*square_button.label);

    const float now = net_clock.getElapsedTime().asSeconds();
    drawTrail(local_trail, local_player.shape().getFillColor(), now);
    drawTrail(remote_trail, remote_player.shape().getFillColor(), now);
    window.draw(local_player.shape());
    window.draw(remote_player.shape());
    window.draw(*local_text);
    window.draw(*remote_text);

    window.display();
}
