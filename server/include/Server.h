#pragma once

#include <vector>
#include <atomic>
#include <mutex>
#include <SFML/Network.hpp>
#include "ClientSession.h"

class Server {
private:
    unsigned short server_port;
    sf::UdpSocket server_socket;

    std::vector<ClientSession> clients;
    std::atomic<bool> is_running;
    std::mutex clients_mutex;

public:
    explicit Server(unsigned short server_port);

    void start();
    void stop();

private:
    void handle_packet(sf::Packet& packet,
                       const sf::IpAddress& sender,
                       unsigned short sender_port);

    void register_client(const sf::IpAddress& sender,
                         unsigned short sender_port);

    void forward_to_others(sf::Packet& packet,
                           const sf::IpAddress& sender,
                           unsigned short sender_port);
};
