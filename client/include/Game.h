#pragma once
#include <SFML/Graphics.hpp>
#include <deque>
#include <memory>
#include "Player.h"
#include "NetworkClient.h"
#include "Interpolation.h"

class Game {
public:
    Game(sf::RenderWindow& window, int client_id);
    void run();

private:
    enum class InterpolationMethod {
        Linear,
        Lagrange,
        Newton
    };

    enum class MovementPath {
        Sinusoidal,
        Spiral,
        Square
    };

    struct Button {
        sf::RectangleShape shape;
        std::unique_ptr<sf::Text> label;
    };

    struct TrailPoint {
        sf::Vector2f position;
        float time;
    };

    sf::RenderWindow& window;
    int client_id;

    Player local_player;
    Player remote_player;

    sf::Clock frame_clock;
    sf::Clock net_clock;
    float interpolation_delay;
    std::deque<Interpolation::Sample> remote_buffer;
    std::deque<TrailPoint> local_trail;
    std::deque<TrailPoint> remote_trail;

    sf::Font font;
    std::unique_ptr<sf::Text> local_text;
    std::unique_ptr<sf::Text> remote_text;
    std::unique_ptr<sf::Text> title_text;
    std::unique_ptr<sf::Text> status_text;
    std::unique_ptr<sf::Text> path_label_text;
    std::unique_ptr<sf::Text> sample_label_text;
    std::unique_ptr<sf::Text> sample_count_text;
    Button linear_button;
    Button lagrange_button;
    Button newton_button;
    Button simulation_button;
    Button sinusoidal_button;
    Button spiral_button;
    Button square_button;
    Button sample_decrease_button;
    sf::RectangleShape sample_count_box;
    Button sample_increase_button;

    NetworkClient network_client;
    sf::Clock send_timer;
    sf::Time send_interval;
    Interpolation interpolation_;
    InterpolationMethod interpolation_method = InterpolationMethod::Linear;
    MovementPath movement_path = MovementPath::Sinusoidal;
    bool simulation_running = false;
    int polynomial_sample_count = 3;

    void handleEvents();
    void update(float dt);
    void render();
    void initializeUi();
    void handleMouseClick(sf::Vector2f mouse_position);
    void updateButtonStyles();
    void configureLocalPath(bool reset_motion);
    void resetTrails(float now);
    void updateRemotePlayer(float render_time);
    void updateTrail(std::deque<TrailPoint>& trail, sf::Vector2f position, float now);
    void pruneTrail(std::deque<TrailPoint>& trail, float now) const;
    void drawTrail(const std::deque<TrailPoint>& trail, const sf::Color& color, float now);
    sf::Vector2f interpolatePosition(float render_time);
    static bool contains(const Button& button, sf::Vector2f point);
};
